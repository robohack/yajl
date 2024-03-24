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

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <assert.h>

#include "yajl/yajl_tree.h"

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

    assert(sz != 0);
    TEST_CTX(ctx)->numMallocs++;
    rv = malloc(sz);
    assert(rv != NULL);
    if (TEST_CTX(ctx)->do_printfs) {
        fprintf(stderr, "yalloc:  %p of %ju\n", rv, sz);
    }
    return rv;
}

static void *
yajlTestRealloc(void *ctx,
                void *ptr,
                size_t sz)
{
    void *rv = NULL;

    if (ptr == NULL) {
        assert(sz != 0);
        TEST_CTX(ctx)->numMallocs++;
    } else if (sz == 0) {
        TEST_CTX(ctx)->numFrees++;
    }
    rv = realloc(ptr, sz);
    assert(rv != NULL);
    if (TEST_CTX(ctx)->do_printfs) {
        fprintf(stderr, "yrealloc:  %p -> %p of %ju\n", ptr, rv, sz);
    }
    return rv;
}


static unsigned char fileData[65536];   /* xxx: allocate to size of file, error if stdin can't be stat()ed? */

int
main(void)
{
    size_t rd;
    yajl_val node;
    char errbuf[1024];

    /* memory allocation debugging: allocate a structure which holds
     * allocation routines */
    yajl_alloc_funcs allocFuncs = {
        yajlTestMalloc,
        yajlTestRealloc,
        yajlTestFree,
        (void *) NULL
    };

    /* memory allocation debugging: allocate a structure which collects
     * statistics */
    yajlTestMemoryContext memCtx;

    memCtx.do_printfs = false;          /* xxx set from a command option */
    memCtx.numMallocs = 0;
    memCtx.numFrees = 0;

    allocFuncs.ctx = (void *) &memCtx;
    yajl_tree_parse_afs = &allocFuncs;

    /* read the entire config file */
    rd = fread((void *) fileData, (size_t) 1, sizeof(fileData) - 1, stdin);

    /* file read error handling */
    if ((rd == 0 && !feof(stdin)) || ferror(stdin)) {
        perror("error encountered on file read");
        exit(1);
    } else if (!feof(stdin)) {
        fprintf(stderr, "config file too big\n");
        exit(1);
    }
    fileData[rd] = '\0';

    /* we have the whole config file in memory.  let's parse it ... */
    node = yajl_tree_parse((const char *) fileData, errbuf, sizeof(errbuf));

    /* parse error handling */
    if (node == NULL) {
        assert(errbuf != NULL);
        fprintf(stderr, "tree_parse_error: %s\n", errbuf);
        fprintf(stderr, "memory leaks:\t%u\n", memCtx.numMallocs - memCtx.numFrees);

        exit(1);
    }

    /* ... and extract a nested value from the config file */
    {
        const char * path[] = { "Logging", "timeFormat", (const char *) 0 };

        yajl_val v = yajl_tree_get(node, path, yajl_t_string);

        if (v) {
            printf("%s/%s: %s\n", path[0], path[1], YAJL_GET_STRING(v));
        } else {
            printf("no such node: %s/%s\n", path[0], path[1]);
        }
    }
    fflush(stdout);				/* make sure stdout flushed before stderr! */

    yajl_tree_free(node);

    fprintf(stderr, "memory leaks:\t%u\n", memCtx.numMallocs - memCtx.numFrees);

    exit(memCtx.numMallocs - memCtx.numFrees ? 1 : 0);
}
