#define MAXN 128
#define MAXM ((MAXN + WORDSIZE - 1) / WORDSIZE)

#include <stdio.h>
#include <string.h>

#include "nauty.h"
#include "gtools.h"  // <-- Needed for stringtograph and writeline

int canonicalize_g6(const char *input_g6, char *output_g6) {
    graph g[MAXN * MAXM], cg[MAXN * MAXM];
    int lab[MAXN], ptn[MAXN], orbits[MAXN];
    static DEFAULTOPTIONS_GRAPH(options);
    statsblk stats;
    int n, m = MAXM;

    // stringtograph returns NULL on error
    if (stringtograph((char*)input_g6, g, m, &n) == NULL) {
        fprintf(stderr, "Invalid g6 input: %s\n", input_g6);
        return 1;
    }

    options.getcanon = TRUE;
    densenauty(g, lab, ptn, orbits, &options, &stats, m, n, cg);

    // write canonical graph to output_g6 (should be large enough!)
    writeline(cg, output_g6, m, n);
    return 0;
}
