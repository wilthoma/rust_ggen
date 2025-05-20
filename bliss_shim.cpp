#include "bliss/graph.hh"
#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <cstring>

extern "C" {

// Canonicalize a graph and return the permutation as well as canonical edges.
// `n` = number of vertices.
// `edges` = edge list (flat array of 2*u32, u32[]).
// `num_edges` = number of edges (not the size of the array).
// `out_permutation` = array to write the permutation (size n).
void canonicalize_graph(
    unsigned int n,
    const unsigned int* edges,
    unsigned int num_edges,
    unsigned int* out_permutation
) {
    bliss::Graph g(n);

    for (unsigned int i = 0; i < num_edges; ++i) {
        unsigned int u = edges[2 * i];
        unsigned int v = edges[2 * i + 1];
        g.add_edge(u, v);
    }

    // Get canonical labeling as a std::vector
    bliss::Stats stats;
    const unsigned int* perm = g.canonical_form(stats);

    // Copy to output buffer
    std::memcpy(out_permutation, perm, n * sizeof(unsigned int));
}

}
