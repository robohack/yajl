/*
 * Copyright (c) 2007-2014, Lloyd Hilaiel <me@lloyd.io>
 * Copyright (c) 2016-2024, Greg A. Woods <woods@robohack.ca>
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

#include <yajl/yajl_parse.h>
#include <yajl/yajl_gen.h>

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* true when we're reformatting a stream */
static bool s_streamReformat = false;

#define GEN_AND_RETURN(func)                                          \
    {                                                                 \
        yajl_gen_status __stat = func;                                \
        if (__stat == yajl_gen_generation_complete && s_streamReformat) { \
            yajl_gen_reset(g, "\n");                                    \
            __stat = func;                                              \
        }                                                               \
        return __stat == yajl_gen_status_ok;                            \
    }

static int reformat_null(void * ctx)
{
    yajl_gen g = (yajl_gen) ctx;
    GEN_AND_RETURN(yajl_gen_null(g));
}

static int reformat_boolean(void * ctx, int boolean)
{
    yajl_gen g = (yajl_gen) ctx;
    GEN_AND_RETURN(yajl_gen_bool(g, boolean));
}

static int reformat_number(void * ctx, const char * s, size_t l)
{
    yajl_gen g = (yajl_gen) ctx;
    GEN_AND_RETURN(yajl_gen_number(g, s, l));
}

static int reformat_string(void * ctx, const unsigned char * stringVal,
                           size_t stringLen)
{
    yajl_gen g = (yajl_gen) ctx;
    GEN_AND_RETURN(yajl_gen_string(g, stringVal, stringLen));
}

static int reformat_map_key(void * ctx, const unsigned char * stringVal,
                            size_t stringLen)
{
    yajl_gen g = (yajl_gen) ctx;
    GEN_AND_RETURN(yajl_gen_string(g, stringVal, stringLen));
}

static int reformat_start_map(void * ctx)
{
    yajl_gen g = (yajl_gen) ctx;
    GEN_AND_RETURN(yajl_gen_map_open(g));
}


static int reformat_end_map(void * ctx)
{
    yajl_gen g = (yajl_gen) ctx;
    GEN_AND_RETURN(yajl_gen_map_close(g));
}

static int reformat_start_array(void * ctx)
{
    yajl_gen g = (yajl_gen) ctx;
    GEN_AND_RETURN(yajl_gen_array_open(g));
}

static int reformat_end_array(void * ctx)
{
    yajl_gen g = (yajl_gen) ctx;
    GEN_AND_RETURN(yajl_gen_array_close(g));
}

static yajl_callbacks callbacks = {
    reformat_null,
    reformat_boolean,
    NULL,
    NULL,
    reformat_number,
    reformat_string,
    reformat_start_map,
    reformat_map_key,
    reformat_end_map,
    reformat_start_array,
    reformat_end_array
};

/* context storage for memory debugging routines */
typedef struct
{
    bool do_printfs;
    unsigned int numFrees;
    unsigned int numMallocs;
    /* XXX: we really need a hash table here with per-allocation
     *      information to find any missing free() calls */
} yajlTestMemoryContext;

/* cast void * into context */
#define TEST_CTX(vptr) ((yajlTestMemoryContext *) (vptr))

static void
yajlTestFree(void *ctx,
             void *ptr)
{
    /* note: yajl should never try to free a NULL pointer */
    assert(ptr != NULL);
    TEST_CTX(ctx)->numFrees++;
    if (TEST_CTX(ctx)->do_printfs) {
        fprintf(stderr, "yfree:  %p\n", ptr);
    }
    free(ptr);
}

static void *
yajlTestMalloc(void *ctx,
               size_t sz)
{
    void *rv = NULL;

    /* note: yajl should never ask for zero bytes */
    assert(sz != 0);
    TEST_CTX(ctx)->numMallocs++;
    rv = malloc(sz);
    assert(rv != NULL);
    if (TEST_CTX(ctx)->do_printfs) {
        fprintf(stderr, "yalloc:  %p of %ju\n", rv, (uintmax_t) sz);
    }
    return rv;
}

static void *
yajlTestRealloc(void *ctx,
                void *ptr,
                size_t sz)
{
    void *rv = NULL;

    /* note: yajl should never ask for zero bytes, nor use realloc() to free */
    assert(sz != 0);
    if (ptr == NULL) {
        TEST_CTX(ctx)->numMallocs++;
    }
    rv = realloc(ptr, sz);
    assert(rv != NULL);
    if (TEST_CTX(ctx)->do_printfs) {
        fprintf(stderr, "yrealloc:  %p -> %p of %ju\n", ptr, rv, (uintmax_t) sz);
    }
    return rv;
}

#ifndef EXIT_USAGE
# define EXIT_USAGE	2
#endif

static void
usage(const char * progname)
{
    fprintf(stderr, "%s: reformat json from stdin\n"
            "usage:  json_reformat [options]\n"
            "    -D enable memory allocation debugging printfs\n"
            "    -e escape any forward slashes (for embedding in HTML)\n"
            "    -m minimize json rather than beautify (default)\n"
            "    -s reformat a stream of multiple json entites\n"
            "    -u allow invalid UTF8 inside strings during parsing\n",
            progname);
    exit(EXIT_USAGE);
}

int
main(int argc, char **argv)
{
    yajl_handle hand;
    static unsigned char fileData[BUFSIZ + 1];
    /* generator config */
    yajl_gen g;
    yajl_status stat;
    size_t rd;
    int retval = EXIT_SUCCESS;
    int a = 1;
    bool disable_beautify = false;
    bool set_allow_multi = false;
    bool set_dont_validate = false;
    bool set_escape_solidus = false;

    /* memory allocation debugging: allocate a structure which assigns
     * allocation routines */
    yajl_alloc_funcs allocFuncs = {
        yajlTestMalloc,
        yajlTestRealloc,
        yajlTestFree,
        (void *) NULL
    };

    /* memory allocation debugging: allocate a structure which collects
     * statistics and controls debugging features */
    yajlTestMemoryContext memCtx = {
        .do_printfs = false,
        .numMallocs = 0,
        .numFrees = 0,
    };

    allocFuncs.ctx = (void *) &memCtx;

    /* check arguments XXX convert to getopt()! */
    while ((a < argc) && (argv[a][0] == '-') && (strlen(argv[a]) > 1)) {
        unsigned int i;

        for (i = 1; i < strlen(argv[a]); i++) {
            switch (argv[a][i]) {
            case 'D':
                memCtx.do_printfs = true;
                break;
            case 'm':
                disable_beautify = true;
                break;
            case 's':
                set_allow_multi = true;
                s_streamReformat = true;
                break;
            case 'u':
                set_dont_validate = true;
                break;
            case 'e':
                set_escape_solidus = true;
                break;
            default:
                fprintf(stderr, "%s: unrecognized option: '%c'\n\n",
                        argv[0], argv[a][i]);
                usage(argv[0]);
            }
        }
        ++a;
    }
    if (a < argc) {
        usage(argv[0]);
    }

    g = yajl_gen_alloc(&allocFuncs);
    assert(g != NULL);                  /* XXX internal error with bad yajl_alloc_funcs! */
    yajl_gen_config(g, yajl_gen_beautify, 1);
    yajl_gen_config(g, yajl_gen_validate_utf8, 1);

    /* ok.  open file.  let's read and parse */
    hand = yajl_alloc(&callbacks, &allocFuncs, (void *) g);

    /* and let's allow comments by default */
    yajl_config(hand, yajl_allow_comments, 1);

    if (disable_beautify) {
        yajl_gen_config(g, yajl_gen_beautify, 0);
    }
    if (set_allow_multi) {
        yajl_config(hand, yajl_allow_multiple_values, 1);
    }
    if (set_dont_validate) {
        yajl_config(hand, yajl_dont_validate_strings, 1);
    }
    if (set_escape_solidus) {
        yajl_gen_config(g, yajl_gen_escape_solidus, 1);
    }

    for (;;) {
        rd = fread((void *) fileData, (size_t) 1, sizeof(fileData) - 1, stdin);
        if (rd == 0) {
            if (ferror(stdin)) {
                fprintf(stderr, "stdin: error encountered: %s\n",
                        strerror(errno));
                exit(EXIT_FAILURE);
            } else if (!feof(stdin)) {
                fprintf(stderr, "stdin: error before EOF: %s\n",
                        errno ? strerror(errno) : "Not an error");
                exit(EXIT_FAILURE);
            }
            break;
        }
        fileData[rd] = '\0';

        stat = yajl_parse(hand, fileData, rd);

        if (stat != yajl_status_ok) {
            break;
        } else {
            const unsigned char *buf;
            size_t len;

            yajl_gen_get_buf(g, &buf, &len);
            fwrite(buf, (size_t) 1, len, stdout);
            yajl_gen_clear(g);
        }
    }
    if (stat == yajl_status_ok) {
        /* parse any remaining data in the buffer */
        stat = yajl_complete_parse(hand);
    }
    if (stat != yajl_status_ok) {
        unsigned char *str = yajl_get_error(hand, 1, fileData, rd);

        fprintf(stderr, "stdin: %s", str);
        yajl_free_error(hand, str);
        retval = EXIT_FAILURE;
    }

    yajl_gen_free(g);
    yajl_free(hand);

    fprintf(stderr, "memory leaks:\t%u\n", memCtx.numMallocs - memCtx.numFrees);
    if ((memCtx.numMallocs - memCtx.numFrees) > 0) {
        retval = EXIT_FAILURE;
    }

    exit(retval);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (concat "MAKEOBJDIRPREFIX=../build " (default-value 'compile-command))
 * End:
 */
