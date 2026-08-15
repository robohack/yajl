/*
 * Copyright (c) 2007-2014, Lloyd Hilaiel <me@lloyd.io>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "yajl/yajl_parse.h"
#include "yajl_lex.h"
#include "yajl_parser.h"
#include "yajl_encode.h"
#include "yajl_bytestack.h"

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#define MAX_VALUE_TO_MULTIPLY ((LLONG_MAX / 10) + (LLONG_MAX % 10))
#ifndef MAXULLONG_B10_DIGITS
# define MAXULLONG_B10_DIGITS	(20)	/* for a 64-bit unsigned long long: 18,446,744,073,709,551,615 */
#endif

/*
 * also used in yajl_tree_parse()'s handle_number() callback
 */
long long
yajl_parse_integer(const unsigned char *number, /* pointer to the beginning of
                                                 * the number string, including
                                                 * any sign character */
                   size_t length)       /* length of the number string */
{
    char buf[MAXULLONG_B10_DIGITS + 2]; /* room for a sign and a NUL */
    char *oldlocale;
    long long ret  = 0;

    /*
     * copy number (up to length) to point to a NUL-terminated string
     *
     * note we don't have access to the yajl_handle so we can't use the
     * pre-existing decodeBuf it has.  However since numbers are quite short
     * strings we can just use a buffer on the stack.
     *
     * However since we use a fixed-length buffer on the stack we have to make
     * sure the input string isn't longer than the longest possible base-10
     * representation of LLONG_MAX (plus a sign char).
     */
    if (length > (MAXULLONG_B10_DIGITS + 1)) {
        errno = ERANGE;
        if (*number == '-') {
            return LLONG_MIN;
        } else {
            return LLONG_MAX;
        }
    }

    memcpy(buf, number, length);
    buf[length] = '\0';

    oldlocale = setlocale(LC_NUMERIC, NULL);
    (void) setlocale(LC_NUMERIC, "C");
    ret = strtoll(buf, NULL, 10);
    (void) setlocale(LC_NUMERIC, oldlocale);

    return ret;
}

static long long int
yajl_verify_integer(yajl_handle hand,
                    const unsigned char *buf,
                    size_t bufLen,
                    size_t *offset)
{
    long long int i = 0;

    errno = 0;
    i = yajl_parse_integer(buf, bufLen);
    if (errno) {
        yajl_bs_set(hand->stateStack, yajl_state_parse_error);
        if (i == LLONG_MAX) {
            hand->parseError = "integer overflow";
        } else {
            hand->parseError = "integer underflow";
        }

        /* try to restore error offset */
        if (*offset >= bufLen) {
            *offset -= bufLen - 1;
        } else {
            *offset = 0;
        }
        yajl_lex_adjust_charOff(hand->lexer, -((intmax_t) bufLen - 1));

        return 0;
    }

    return i;
}

/*
 * RFC 7159 suggests JSON parsers should use long double (IEEE 754 64-bit)
 */
static double
yajl_verify_double(yajl_handle hand,
                   const unsigned char *buf,
                   size_t bufLen,
                   size_t *offset)
{
    double d = 0.0;
    char *end;
    char *oldl;

    /* convert buf to point to a NUL-terminated string */
    yajl_buf_clear(hand->decodeBuf);
    yajl_buf_append(hand->decodeBuf, buf, bufLen);
    buf = yajl_buf_data(hand->decodeBuf);

    errno = 0;
    /*
     * N.B.:  the "values" INF, INFINITY, and NAN, (i.e. as unquoted inputs to
     * strtod()) are not actually valid for JSON numbers
     */
    oldl = setlocale(LC_NUMERIC, NULL);
    (void) setlocale(LC_NUMERIC, "C");
    d = strtod((const char *) buf, &end);
    (void) setlocale(LC_NUMERIC, oldl);
    /*
     * these 'end' checks should be prevented from triggering by the lexer
     * having pre-validated the token, but double-check!
     */
    if (end == (const char *) buf) {
        yajl_bs_set(hand->stateStack, yajl_state_parse_error);
        hand->parseError = "invalid numeric (floating point), invalid syntax";
    }
    if (end[0] != '\0') {
        yajl_bs_set(hand->stateStack, yajl_state_parse_error);
        hand->parseError = "invalid numeric (floating point), trailing garbage";
    }
    if ((d == HUGE_VAL || d == -HUGE_VAL) && errno == ERANGE) {
        yajl_bs_set(hand->stateStack, yajl_state_parse_error);
        hand->parseError = "numeric (floating point) overflow";
    } else if (errno == ERANGE) {
        yajl_bs_set(hand->stateStack, yajl_state_parse_error);
        hand->parseError = "numeric (floating point) underflow";
    } else if (errno) {
        yajl_bs_set(hand->stateStack, yajl_state_parse_error);
        hand->parseError = "invalid numeric (floating point), unknown error";
    }
    if (errno) {
        /* try to restore error offset */
        if (*offset >= bufLen) {
            *offset -= bufLen - 1;
        } else {
            *offset = 0;
        }
        yajl_lex_adjust_charOff(hand->lexer, -((intmax_t) bufLen - 1));

        return 0.0;
    }

    return d;
}

unsigned char *
yajl_render_error_string(yajl_handle hand, const unsigned char * jsonText,
                         size_t jsonTextLen, int verbose)
{
    size_t offset = hand->bytesConsumed;
    unsigned char * str;
    const char * errorType = NULL;
    const char * errorText = NULL;
    unsigned char text[72];
    const char * arrow = "                    (right here) ------^\n";

    if (yajl_bs_current(hand->stateStack) == yajl_state_parse_error) {
        errorType = "parse";
        errorText = hand->parseError;
    } else if (yajl_bs_current(hand->stateStack) == yajl_state_lexical_error) {
        errorType = "lexical";
        errorText = yajl_lex_error_to_string(yajl_lex_get_error(hand->lexer));
    } else {
        errorType = "unknown";
    }

    {
        size_t memneeded = 0;
        size_t line = 0;
        size_t coff = 0;
        char anumber[MAXULLONG_B10_DIGITS+1];

        /*
         * N.B.:  the GNU coding standards, as followed by GCC, suggest the
         * format for error messages indicating line and column should be:
         *
         *     file:L:C: message
         *
         * However perhaps it makes more sense to indicate what the numbers are
         * so as to avoid any possible confusion.
         *
         * The filename must be added by the caller of course.
         */
        memneeded += strlen("line: ");
        memneeded += sizeof(anumber) - 1;

        memneeded += strlen(": offset: ");
        memneeded += sizeof(anumber) - 1;

        memneeded += strlen(": ");

        memneeded += strlen(errorType);
        memneeded += strlen(" error");
        if (errorText != NULL) {
            memneeded += strlen(": ");
            memneeded += strlen(errorText);
        }

        str = (unsigned char *) YA_MALLOC(&(hand->alloc), memneeded + 2);
        str[0] = '\0';

        line = yajl_lex_current_line(hand->lexer);
        strcat((char *) str, "line: ");
        snprintf(anumber, sizeof(anumber), "%ju", (intmax_t) line);
        strcat((char *) str, anumber);

        coff = yajl_lex_current_char(hand->lexer);
        strcat((char *) str, ": offset: ");
        snprintf(anumber, sizeof(anumber), "%ju", (intmax_t) coff);
        strcat((char *) str, anumber);

        strcat((char *) str, ": ");

        strcat((char *) str, errorType);
        strcat((char *) str, " error");
        if (errorText != NULL) {
            strcat((char *) str, ": ");
            strcat((char *) str, errorText);
        }
        strcat((char *) str, "\n");
    }

    /* now, if verbose was specified, we append as many spaces as needed to make
     * sure the error falls at char 40 */
    if (verbose) {
        size_t start, end, i;
        size_t spacesNeeded;

        spacesNeeded = (offset < 30 ? 40 - offset : 10);
        start = (offset >= 30 ? offset - 30 : 0);
        end = (offset + 30 > jsonTextLen ? jsonTextLen : offset + 30);

        for (i=0; i < spacesNeeded; i++) {
            text[i] = ' ';
        }

        for (; start < end; start++, i++) {
            if (jsonText[start] != '\n' && jsonText[start] != '\r') {
                text[i] = jsonText[start];
            } else {
                text[i] = ' ';
            }
        }
        assert(i <= 71);
        text[i++] = '\n';
        text[i] = '\0';
        {
            char *newStr =
                YA_MALLOC(&(hand->alloc), (size_t)(strlen((char *) str) +
                                                   strlen((char *) text) +
                                                   strlen(arrow) + 1));
            newStr[0] = '\0';
            strcat(newStr, (char *) str);
            strcat(newStr, (char *) text);
            strcat(newStr, arrow);
            YA_FREE(&(hand->alloc), str);
            str = (unsigned char *) newStr;
        }
    }
    return str;
}

/* check for client cancelation */
#define _CC_CHK(x)                                                \
    if (!(x)) {                                                   \
        yajl_bs_set(hand->stateStack, yajl_state_parse_error);    \
        hand->parseError =                                        \
            "client cancelled parse via callback return value";   \
        return yajl_status_client_canceled;                       \
    }


yajl_status
yajl_do_finish(yajl_handle hand)
{
    yajl_status stat;
    size_t offset = hand->bytesConsumed;

    stat = yajl_do_parse(hand, (const unsigned char *) " ", (size_t) 1);
    hand->bytesConsumed = offset;

    if (stat != yajl_status_ok) {
        return stat;
    }

    switch (yajl_bs_current(hand->stateStack)) {
    case yajl_state_parse_error:
    case yajl_state_lexical_error:
        return yajl_status_error;

    case yajl_state_got_value:
    case yajl_state_parse_complete:
        return yajl_status_ok;

    default:
        break;
    }
    if (!(hand->flags & yajl_allow_partial_values)) {
        yajl_bs_set(hand->stateStack, yajl_state_parse_error);
        hand->parseError = "premature EOF";

        return yajl_status_error;
    }
    return yajl_status_ok;
}

yajl_status
yajl_do_parse(yajl_handle hand, const unsigned char * jsonText,
              size_t jsonTextLen)
{
    yajl_tok tok;
    const unsigned char * buf;
    size_t bufLen;
    size_t * offset = &(hand->bytesConsumed);

    *offset = 0;

  around_again:
    switch (yajl_bs_current(hand->stateStack)) {
        case yajl_state_parse_complete:
            if (hand->flags & yajl_allow_multiple_values) {
                yajl_bs_set(hand->stateStack, yajl_state_got_value);
                goto around_again;
            }
            if (!(hand->flags & yajl_allow_trailing_garbage)) {
                if (*offset != jsonTextLen) {
                    tok = yajl_lex_lex(hand->lexer, jsonText, jsonTextLen,
                                       offset, &buf, &bufLen);
                    if (tok != yajl_tok_eof) {
                        yajl_bs_set(hand->stateStack, yajl_state_parse_error);
                        hand->parseError = "trailing garbage";
                    }
                    goto around_again;
                }
            }
            return yajl_status_ok;
        case yajl_state_lexical_error:
        case yajl_state_parse_error:
            return yajl_status_error;
        case yajl_state_start:
        case yajl_state_got_value:
        case yajl_state_map_need_val:
        case yajl_state_array_need_val:
        case yajl_state_array_start:  {
            /* for arrays and maps, we advance the state for this
             * depth, then push the state of the next depth.
             * If an error occurs during the parsing of the nesting
             * enitity, the state at this level will not matter.
             * a state that needs pushing will be anything other
             * than state_start */

            yajl_state stateToPush = yajl_state_start;

            tok = yajl_lex_lex(hand->lexer, jsonText, jsonTextLen,
                               offset, &buf, &bufLen);

            switch (tok) {
                case yajl_tok_eof:
                    return yajl_status_ok;
                case yajl_tok_error:
                    yajl_bs_set(hand->stateStack, yajl_state_lexical_error);
                    goto around_again;
                case yajl_tok_string:
                    if (hand->callbacks && hand->callbacks->yajl_string) {
                        _CC_CHK(hand->callbacks->yajl_string(hand->ctx,
                                                             buf, bufLen));
                    }
                    break;
                case yajl_tok_string_with_escapes:
                    if (hand->callbacks && hand->callbacks->yajl_string) {
                        yajl_buf_clear(hand->decodeBuf);
                        yajl_string_decode(hand->decodeBuf, buf, bufLen);
                        _CC_CHK(hand->callbacks->yajl_string(
                                    hand->ctx, yajl_buf_data(hand->decodeBuf),
                                    yajl_buf_len(hand->decodeBuf)));
                    }
                    break;
                case yajl_tok_bool:
                    if (hand->callbacks && hand->callbacks->yajl_boolean) {
                        _CC_CHK(hand->callbacks->yajl_boolean(hand->ctx,
                                                              *buf == 't'));
                    }
                    break;
                case yajl_tok_null:
                    if (hand->callbacks && hand->callbacks->yajl_null) {
                        _CC_CHK(hand->callbacks->yajl_null(hand->ctx));
                    }
                    break;
                case yajl_tok_left_bracket:
                    if (hand->callbacks && hand->callbacks->yajl_start_map) {
                        _CC_CHK(hand->callbacks->yajl_start_map(hand->ctx));
                    }
                    stateToPush = yajl_state_map_start;
                    break;
                case yajl_tok_left_brace:
                    if (hand->callbacks && hand->callbacks->yajl_start_array) {
                        _CC_CHK(hand->callbacks->yajl_start_array(hand->ctx));
                    }
                    stateToPush = yajl_state_array_start;
                    break;
                case yajl_tok_integer:
                    if (hand->callbacks) {
                        if (hand->callbacks->yajl_number) {
                            _CC_CHK(hand->callbacks->yajl_number(
                                        hand->ctx, (const char *) buf, bufLen));
                        } else if (hand->callbacks->yajl_integer) {
                            long long int i;

                            i = yajl_verify_integer(hand, buf, bufLen, offset);
                            if (errno) {
                                goto around_again;
                            }
                            _CC_CHK(hand->callbacks->yajl_integer(hand->ctx, i));
                        }
                    } else {
                        (void) yajl_verify_integer(hand, buf, bufLen, offset);
                        if (errno) {
                            goto around_again;
                        }
                    }
                    break;
                case yajl_tok_double:
                    if (hand->callbacks) {
                        if (hand->callbacks->yajl_number) {
                            _CC_CHK(hand->callbacks->yajl_number(
                                        hand->ctx, (const char *) buf, bufLen));
                        } else if (hand->callbacks->yajl_double) {
                            double d;

                            d = yajl_verify_double(hand, buf, bufLen, offset);
                            if (errno) {
                                goto around_again;
                            }
                            _CC_CHK(hand->callbacks->yajl_double(hand->ctx, d));
                        }
                    } else {
                        (void) yajl_verify_double(hand, buf, bufLen, offset);
                        if (errno) {
                            goto around_again;
                        }
                    }
                    break;
                case yajl_tok_right_brace:
                    if (yajl_bs_current(hand->stateStack) ==
                        yajl_state_array_start)
                    {
                        if (hand->callbacks &&
                            hand->callbacks->yajl_end_array)
                        {
                            _CC_CHK(hand->callbacks->yajl_end_array(hand->ctx));
                        }
                        yajl_bs_pop(hand->stateStack);
                        goto around_again;
                    }
                    /* FALLTHROUGH */
                case yajl_tok_colon:
                case yajl_tok_comma:
                case yajl_tok_right_bracket:
                    yajl_bs_set(hand->stateStack, yajl_state_parse_error);
                    hand->parseError =
                        "unallowed token at this point in JSON text";
                    goto around_again;
                default:
                    yajl_bs_set(hand->stateStack, yajl_state_parse_error);
                    hand->parseError = "invalid token, internal error";
                    goto around_again;
            }
            /* got a value.  transition depends on the state we're in. */
            {
                yajl_state s = (yajl_state) yajl_bs_current(hand->stateStack);
                if (s == yajl_state_start || s == yajl_state_got_value) {
                    yajl_bs_set(hand->stateStack, yajl_state_parse_complete);
                } else if (s == yajl_state_map_need_val) {
                    yajl_bs_set(hand->stateStack, yajl_state_map_got_val);
                } else {
                    yajl_bs_set(hand->stateStack, yajl_state_array_got_val);
                }
            }
            if (stateToPush != yajl_state_start) {
                yajl_bs_push(hand->stateStack, stateToPush);
            }

            goto around_again;
        }
        case yajl_state_map_start:
        case yajl_state_map_need_key: {
            /* only difference between these two states is that in
             * start '}' is valid, whereas in need_key, we've parsed
             * a comma, and a string key _must_ follow */
            tok = yajl_lex_lex(hand->lexer, jsonText, jsonTextLen,
                               offset, &buf, &bufLen);
            switch (tok) {
                case yajl_tok_eof:
                    return yajl_status_ok;
                case yajl_tok_error:
                    yajl_bs_set(hand->stateStack, yajl_state_lexical_error);
                    goto around_again;
                case yajl_tok_string_with_escapes:
                    if (hand->callbacks && hand->callbacks->yajl_map_key) {
                        yajl_buf_clear(hand->decodeBuf);
                        yajl_string_decode(hand->decodeBuf, buf, bufLen);
                        buf = yajl_buf_data(hand->decodeBuf);
                        bufLen = yajl_buf_len(hand->decodeBuf);
                    }
                    /* FALLTHROUGH */
                case yajl_tok_string:
                    if (hand->callbacks && hand->callbacks->yajl_map_key) {
                        _CC_CHK(hand->callbacks->yajl_map_key(hand->ctx, buf,
                                                              bufLen));
                    }
                    yajl_bs_set(hand->stateStack, yajl_state_map_sep);
                    goto around_again;
                case yajl_tok_right_bracket:
                    if (yajl_bs_current(hand->stateStack) ==
                        yajl_state_map_start)
                    {
                        if (hand->callbacks && hand->callbacks->yajl_end_map) {
                            _CC_CHK(hand->callbacks->yajl_end_map(hand->ctx));
                        }
                        yajl_bs_pop(hand->stateStack);
                        goto around_again;
                    }
                    /* FALLTHROUGH */
                default:
                    yajl_bs_set(hand->stateStack, yajl_state_parse_error);
                    hand->parseError =
                        "invalid object key (must be a string)";
                    goto around_again;
            }
        }
        case yajl_state_map_sep: {
            tok = yajl_lex_lex(hand->lexer, jsonText, jsonTextLen,
                               offset, &buf, &bufLen);
            switch (tok) {
                case yajl_tok_colon:
                    yajl_bs_set(hand->stateStack, yajl_state_map_need_val);
                    goto around_again;
                case yajl_tok_eof:
                    return yajl_status_ok;
                case yajl_tok_error:
                    yajl_bs_set(hand->stateStack, yajl_state_lexical_error);
                    goto around_again;
                default:
                    yajl_bs_set(hand->stateStack, yajl_state_parse_error);
                    hand->parseError = "object key and value must "
                        "be separated by a colon (':')";
                    goto around_again;
            }
        }
        case yajl_state_map_got_val: {
            tok = yajl_lex_lex(hand->lexer, jsonText, jsonTextLen,
                               offset, &buf, &bufLen);
            switch (tok) {
                case yajl_tok_right_bracket:
                    if (hand->callbacks && hand->callbacks->yajl_end_map) {
                        _CC_CHK(hand->callbacks->yajl_end_map(hand->ctx));
                    }
                    yajl_bs_pop(hand->stateStack);
                    goto around_again;
                case yajl_tok_comma:
                    yajl_bs_set(hand->stateStack, yajl_state_map_need_key);
                    goto around_again;
                case yajl_tok_eof:
                    return yajl_status_ok;
                case yajl_tok_error:
                    yajl_bs_set(hand->stateStack, yajl_state_lexical_error);
                    goto around_again;
                default:
                    yajl_bs_set(hand->stateStack, yajl_state_parse_error);
                    hand->parseError = "after key and value, inside map, "
                                       "I expect ',' or '}'";

                    /* try to restore error offset */
                    if (*offset >= bufLen) {
                        *offset -= bufLen;
                        yajl_lex_adjust_charOff(hand->lexer, -((intmax_t) bufLen));
                    } else {
                        *offset = 0;
                    }

                    goto around_again;
            }
        }
        case yajl_state_array_got_val: {
            tok = yajl_lex_lex(hand->lexer, jsonText, jsonTextLen,
                               offset, &buf, &bufLen);
            switch (tok) {
                case yajl_tok_right_brace:
                    if (hand->callbacks && hand->callbacks->yajl_end_array) {
                        _CC_CHK(hand->callbacks->yajl_end_array(hand->ctx));
                    }
                    yajl_bs_pop(hand->stateStack);
                    goto around_again;
                case yajl_tok_comma:
                    yajl_bs_set(hand->stateStack, yajl_state_array_need_val);
                    goto around_again;
                case yajl_tok_eof:
                    return yajl_status_ok;
                case yajl_tok_error:
                    yajl_bs_set(hand->stateStack, yajl_state_lexical_error);
                    goto around_again;
                default:
                    yajl_bs_set(hand->stateStack, yajl_state_parse_error);
                    hand->parseError =
                        "after array element, I expect ',' or ']'";
                    goto around_again;
            }
        }
    }

    abort();
    /* NOTREACHED */
    return yajl_status_error;
}
