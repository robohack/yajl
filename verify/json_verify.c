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

#include <yajl/yajl_parse.h>

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
usage(const char *progname)
{
    fprintf(stderr, "%s: validate json from stdin\n"
                    "usage: json_verify [options]\n"
                    "    -c allow comments\n"
                    "    -q quiet mode\n"
                    "    -s verify a stream of multiple json entities\n"
                    "    -u allow invalid utf8 inside strings\n",
            progname);
    exit(EXIT_USAGE);
}

int
main(int argc, char **argv)
{
    yajl_status stat;
    size_t rd;
    yajl_handle hand;
    static unsigned char fileData[BUFSIZ + 1];
    int quiet = 0;
    int retval = EXIT_SUCCESS;
    int a = 1;
    bool set_allow_comments = false;
    bool set_dont_validate = false;
    bool set_allow_multi = false;

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

    /* check arguments.*/
    while ((a < argc) && (argv[a][0] == '-') && (strlen(argv[a]) > 1)) {
        unsigned int i;

        for (i = 1; i < strlen(argv[a]); i++) {
            switch (argv[a][i]) {
            case 'D':
                memCtx.do_printfs = true;
                break;
            case 'q':
                quiet = 1;
                break;
            case 'c':
                set_allow_comments = true;
                break;
            case 'u':
                set_dont_validate = true;
                break;
            case 's':
                set_allow_multi = true;
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

    /* allocate a parser */
    hand = yajl_alloc(NULL, &allocFuncs, NULL);

    /* configure the parser */
    if (set_allow_comments) {
        yajl_config(hand, yajl_allow_comments, 1);
    }
    if (set_dont_validate) {
        yajl_config(hand, yajl_dont_validate_strings, 1);
    }
    if (set_allow_multi) {
        yajl_config(hand, yajl_allow_multiple_values, 1);
    }

    for (;;) {
        rd = fread((void *) fileData, (size_t) 1, sizeof(fileData) - 1, stdin);
        if (rd == 0) {
            if (ferror(stdin)) {
                fprintf(stderr, "stdin: error encountered: %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            } else if (!feof(stdin)) {
                fprintf(stderr, "stdin: error before EOF: %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
            break;
        }
        fileData[rd] = '\0';

        /* read file data, pass to parser */
        stat = yajl_parse(hand, fileData, rd);

        if (stat != yajl_status_ok) {
            break;
        }
    }
    if (stat == yajl_status_ok) {
        /* parse any remaining data in the buffer */
        stat = yajl_complete_parse(hand);
    }
    if (stat != yajl_status_ok) {
        if (!quiet) {
            unsigned char *str = yajl_get_error(hand, 1, fileData, rd);

            fprintf(stderr, "stdin: %s", str);
            yajl_free_error(hand, str);
        }
        retval = EXIT_FAILURE;
    }

    yajl_free(hand);

    if (!quiet) {
        printf("JSON is %s\n", (retval == EXIT_SUCCESS) ? "valid" : "invalid");
    }

    if (!quiet) {
        fprintf(stderr, "memory leaks:\t%u\n", memCtx.numMallocs - memCtx.numFrees);
    }
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
