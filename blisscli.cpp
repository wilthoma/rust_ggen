#include <iostream>
#include <vector>
#include <utility>
#include <functional>
#include "bliss/graph.hh"
#include "mygraphs.hh"

int main() {
    int n;
    std::cin >> n;

    std::vector<std::pair<int, int>> edges;
    int u, v;
    while (std::cin >> u >> v) {
        edges.emplace_back(u, v);
    }

    bliss::Graph g(n);
    for (auto& [u, v] : edges) {
        g.add_edge(u, v);
    }

    std::vector<std::vector<unsigned>> generators;
    auto callback = [&](unsigned n, const unsigned* perm) {
        generators.emplace_back(perm, perm + n);
    };

    bliss::Stats stats;
    g.find_automorphisms(stats, callback);  // Modern Bliss: expects std::function

    for (const auto& perm : generators) {
        for (auto x : perm) std::cout << x << " ";
        std::cout << "\n";
    }

    return 0;
}
