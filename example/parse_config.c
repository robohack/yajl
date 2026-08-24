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

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <assert.h>

#ifndef EXIT_USAGE
# define EXIT_USAGE 2
#endif

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

static const char *
yajl_type_name(yajl_type t)
{
    switch (t) {
    case yajl_t_string:
        return "string";
        break;
    case yajl_t_number:
        return "number";
        break;
    case yajl_t_object:
        return "object";
        break;
    case yajl_t_array:
        return "array";
        break;
    case yajl_t_true:
        return "true";
    case yajl_t_false:
        return "false";
        break;
    case yajl_t_null:
        return "null";
        break;
    case yajl_t_any:
        return "any";
        break;
    }
    assert(false);
    return "OOPS!!!";
}

static const char *
yajl_tree_path(const char *path[],
               const char *p2)
{
    static char ps[65536];
    size_t i;

    ps[0] = '\0';
    for (i = 0; path[i]; i++) {
        strcat(ps, path[i]);
        if (path[i + 1] != NULL || p2 != NULL) {
            strcat(ps, "/");
        }
    }
    if (p2) {
        strcat(ps, p2);
    }

    return ps;
}

/*
 * XXX this is just a cheap hack that prints simpler structures that are not
 * nested too deeply.....
 */
static void
yajl_tree_print_v(const char *path[],
                  const char *p2,       /* xxx should be array */
                  yajl_val v)
{
    size_t i;
    const char *np[1000];              /* xxx allocate to len of path + 2 */

    switch (v->type) {
    case yajl_t_string:
        if (path != NULL) {
            printf("%s: ", yajl_tree_path(path, p2));
        }
        printf("\"%s\"\n", YAJL_GET_STRING(v));
        break;
    case yajl_t_number:
        if (path != NULL) {
            printf("%s: ", yajl_tree_path(path, p2));
        }
        if (YAJL_IS_DOUBLE(v)) {
            printf("%g\n", YAJL_GET_DOUBLE(v));
        } else if (YAJL_IS_INTEGER(v)) {
            printf("%lld\n", YAJL_GET_INTEGER(v));
        } else {
            printf("%s [INVALID RANGE]\n", YAJL_GET_NUMBER(v));
        }
        break;
    case yajl_t_object:
        i = 0;
        if (path != NULL) {
            for (i = 0; path[i]; i++) {
                np[i] = path[i];
            }
        }
        if (p2) {
            np[i] = p2;
            i++;
        }
        np[i] = NULL;
        for (i = 0; i < v->u.object.len; i++) {
            yajl_tree_print_v(np, v->u.object.keys[i], v->u.object.values[i]);
        }
        break;
    case yajl_t_array:
        if (path != NULL) {
            printf("%s:\n", yajl_tree_path(path, p2));
        }
        for (i = 0; i < v->u.array.len; i++) {
            printf("    [%ju]: (%s) ", (uintmax_t) i, yajl_type_name(v->u.array.values[i]->type));
            /* XXX p2 should be path + p2 */
            yajl_tree_print_v(path, p2, v->u.array.values[i]);
        }
        break;
    case yajl_t_true:
    case yajl_t_false:
        assert(YAJL_IS_TRUE(v) || YAJL_IS_FALSE(v));
        if (path != NULL) {
            printf("%s: ", yajl_tree_path(path, p2));
        }
        printf("%s\n", YAJL_IS_TRUE(v) ? "true" : "false");
        break;
    case yajl_t_null:
        if (path != NULL) {
            printf("%s: ", yajl_tree_path(path, p2));
        }
        printf("<NULL>\n");
        break;
    case yajl_t_any:
        assert(v->type != yajl_t_any); /* n.b.: invalid in data! */
        break;
    }
}

static unsigned char fileData[65536];   /* xxx: allocate and then realloc as reading (or provide getline(3)) */

int
main(int argc,
     const char *argv[])
{
    size_t rd;
    yajl_val node;
    char errbuf[1024];
    const char **path;

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

    if (argc == 1) {
        fprintf(stderr, "Usage: %s json path ...\n", argv[0]);
        exit(EXIT_USAGE);
    }

    path = &(argv[1]);

    /* read the entire config file (xxx or as much as we have room for) */
    rd = fread((void *) fileData, (size_t) 1, sizeof(fileData) - 1, stdin);

    /* file read error handling */
    if (rd == 0) {
        if (ferror(stdin)) {
            perror("error encountered reading stdin");
            exit(1);
        } else if (!feof(stdin)) {
            fprintf(stderr, "config file too big\n");
            exit(1);
        }
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
        yajl_val v = yajl_tree_get(node, path, yajl_t_any);

        if (v) {
            yajl_tree_print_v(path, NULL, v);
        } else {
            printf("no such node: %s\n", yajl_tree_path(path, NULL));
        }
    }
    /*
     * try to make sure stdout flushed before stderr!
     */
    if (fflush(stdout) != 0) {
        perror("fflush");
    }

    yajl_tree_free(node);

    fprintf(stderr, "memory leaks:\t%u\n", memCtx.numMallocs - memCtx.numFrees);

    exit(memCtx.numMallocs - memCtx.numFrees ? EXIT_FAILURE : EXIT_SUCCESS);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (concat "MAKEOBJDIRPREFIX=../build " (default-value 'compile-command))
 * End:
 */
