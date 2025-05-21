#ifndef MYGRAPHS_HH
#define MYGRAPHS_HH

#include <vector>
#include <string>
#include <stdexcept>
#include <cstdint>
#include "bliss/graph.hh"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <set>
#include <utility>




class Graph {
public:
    uint8_t num_vertices;
    std::vector<std::pair<uint8_t, uint8_t>> edges;

    Graph(uint8_t n) : num_vertices(n) {}

    Graph(uint8_t n, const std::vector<std::pair<uint8_t, uint8_t>>& e)
        : num_vertices(n), edges(e) {}

    void add_edge(uint8_t u, uint8_t v) {
        if (u < v)
            edges.emplace_back(u, v);
        else
            edges.emplace_back(v, u);
    }

    std::string to_g6() const {
        uint8_t n = num_vertices;
        if (n > 62) throw std::runtime_error("Only supports graphs with at most 62 vertices.");
        std::string result;
        result.push_back(static_cast<char>(n + 63));
        std::vector<uint8_t> bitvec;
        for (uint8_t j = 1; j < n; ++j) {
            for (uint8_t i = 0; i < j; ++i) {
                bool found = false;
                for (const auto& e : edges) {
                    if ((e.first == i && e.second == j) || (e.first == j && e.second == i)) {
                        found = true;
                        break;
                    }
                }
                bitvec.push_back(found ? 1 : 0);
            }
        }
        while (bitvec.size() % 6 != 0) bitvec.push_back(0);
        for (size_t k = 0; k < bitvec.size(); k += 6) {
            uint8_t value = 0;
            for (size_t i = 0; i < 6; ++i) {
                value |= (bitvec[k + i] << (5 - i));
            }
            result.push_back(static_cast<char>(value + 63));
        }
        return result;
    }

    Graph add_edge_across(size_t e1idx, size_t e2idx) const {
        if (e1idx == e2idx) throw std::invalid_argument("Edges must be distinct");
        uint8_t new_n = num_vertices + 2;
        uint8_t v1 = num_vertices;
        uint8_t v2 = num_vertices + 1;
        std::vector<std::pair<uint8_t, uint8_t>> new_edges;
        for (size_t i = 0; i < edges.size(); ++i) {
            auto [u, v] = edges[i];
            if (i == e1idx) {
                new_edges.emplace_back(u, v1);
                new_edges.emplace_back(v, v1);
                new_edges.emplace_back(v1, v2);
            } else if (i == e2idx) {
                new_edges.emplace_back(u, v2);
                new_edges.emplace_back(v, v2);
            } else {
                new_edges.emplace_back(u, v);
            }
        }
        std::sort(new_edges.begin(), new_edges.end());
        return Graph(new_n, new_edges);
    }

    Graph replace_edge_by_tetra(size_t eidx) const {
        uint8_t new_n = num_vertices + 4;
        auto [u, v] = edges[eidx];
        if (!(u < v)) throw std::invalid_argument("Edge must be (u < v)");
        std::vector<std::pair<uint8_t, uint8_t>> new_edges;
        for (size_t i = 0; i < edges.size(); ++i) {
            auto [a, b] = edges[i];
            if (i == eidx) {
                uint8_t v1 = new_n - 4;
                uint8_t v2 = new_n - 3;
                uint8_t v3 = new_n - 2;
                uint8_t v4 = new_n - 1;
                new_edges.emplace_back(u, v1);
                new_edges.emplace_back(v1, v2);
                new_edges.emplace_back(v1, v3);
                new_edges.emplace_back(v2, v4);
                new_edges.emplace_back(v3, v4);
                new_edges.emplace_back(v2, v3);
                new_edges.emplace_back(v, v4);
            } else {
                new_edges.emplace_back(a, b);
            }
        }
        std::sort(new_edges.begin(), new_edges.end());
        return Graph(new_n, new_edges);
    }

    Graph union_with(const Graph& other) const {
        uint8_t new_n = num_vertices + other.num_vertices;
        std::vector<std::pair<uint8_t, uint8_t>> new_edges = edges;
        for (const auto& e : other.edges) {
            new_edges.emplace_back(e.first + num_vertices, e.second + num_vertices);
        }
        return Graph(new_n, new_edges);
    }

    Graph contract_edge(size_t eidx) const {
        if (num_vertices < 2) throw std::invalid_argument("Not enough vertices to contract");
        uint8_t new_n = num_vertices - 1;
        auto [u, v] = edges[eidx];
        if (!(u < v)) throw std::invalid_argument("Edge must be (u < v)");
        std::set<std::pair<uint8_t, uint8_t>> edge_set;
        for (size_t i = 0; i < edges.size(); ++i) {
            if (i == eidx) continue;
            auto [a, b] = edges[i];
            uint8_t aa = (a < v) ? a : (a == v ? u : a - 1);
            uint8_t bb = (b < v) ? b : (b == v ? u : b - 1);
            if (aa < bb)
                edge_set.emplace(aa, bb);
            else if (bb < aa)
                edge_set.emplace(bb, aa);
        }
        std::vector<std::pair<uint8_t, uint8_t>> new_edges(edge_set.begin(), edge_set.end());
        return Graph(new_n, new_edges);
    }


    static Graph from_g6(const std::string& g6) {
        if (g6.empty()) throw std::invalid_argument("Empty g6 string");
        uint8_t first = static_cast<uint8_t>(g6[0]);
        if (first < 63) throw std::invalid_argument("Invalid graph6 string");
        uint8_t n = (first >= 63 && first <= 126) ? (first - 63) : throw std::invalid_argument("Only supports n ≤ 62");
        size_t num_bits = n * (n - 1) / 2;
        size_t num_bytes = (num_bits + 5) / 6;
        if (g6.size() < 1 + num_bytes) throw std::invalid_argument("g6 string too short");
        std::vector<uint8_t> bits;
        for (size_t i = 0; i < num_bytes; ++i) {
            uint8_t val = static_cast<uint8_t>(g6[1 + i]);
            if (val < 63) throw std::invalid_argument("Invalid graph6 data byte");
            val -= 63;
            for (int j = 5; j >= 0; --j) {
                bits.push_back((val >> j) & 1);
            }
        }
        bits.resize(num_bits);
        std::vector<std::pair<uint8_t, uint8_t>> edges;
        size_t k = 0;
        for (uint8_t j = 1; j < n; ++j) {
            for (uint8_t i = 0; i < j; ++i) {
                if (bits[k++] == 1) {
                    edges.emplace_back(i, j);
                }
            }
        }
        return Graph(n, edges);
    }

    static void save_to_file(const std::vector<std::string>& g6_list, const std::string& filename) {
        std::ofstream file(filename);
        if (!file) throw std::runtime_error("Failed to open file for writing");
        file << g6_list.size() << "\n";
        for (const auto& g6 : g6_list) {
            file << g6 << "\n";
        }
    }

    static std::vector<std::string> load_from_file(const std::string& filename) {
        std::ifstream file(filename);
        if (!file) throw std::runtime_error("Failed to open file for reading");
        size_t num_graphs;
        file >> num_graphs;
        std::string line;
        std::getline(file, line); // consume rest of first line
        std::vector<std::string> g6_list;
        while (std::getline(file, line)) {
            if (!line.empty())
                g6_list.push_back(line);
        }
        if (g6_list.size() != num_graphs)
            throw std::runtime_error("Number of graphs in file does not match the first line");
        return g6_list;
    }

    static std::vector<std::string> load_from_file_nohdr(const std::string& filename) {
        std::ifstream file(filename);
        if (!file) throw std::runtime_error("Failed to open file for reading");
        std::vector<std::string> g6_list;
        std::string line;
        while (std::getline(file, line)) {
            if (!line.empty())
                g6_list.push_back(line);
        }
        return g6_list;
    }

    static Graph tetrahedron_graph() {
        return Graph(4, {{0,1},{0,2},{0,3},{1,2},{1,3},{2,3}});
    }

    static Graph tetrastring_graph(uint8_t n_blocks) {
        uint8_t n = 4 * n_blocks;
        std::vector<std::pair<uint8_t, uint8_t>> edges;
        for (uint8_t i = 0; i < n_blocks; ++i) {
            edges.emplace_back(4 * i, 4 * i + 1);
            edges.emplace_back(4 * i, 4 * i + 2);
            edges.emplace_back(4 * i + 1, 4 * i + 2);
            edges.emplace_back(4 * i + 1, 4 * i + 3);
            edges.emplace_back(4 * i + 2, 4 * i + 3);
            if (i == n_blocks - 1) {
                edges.emplace_back(0, 4 * i + 3);
            } else {
                edges.emplace_back(4 * i + 3, 4 * i + 4);
            }
        }
        return Graph(n, edges);
    }

    void print() const {
        std::cout << "Graph with " << (int)num_vertices << " vertices and " << edges.size()
                  << " edges. G6 code: " << to_g6() << ".\n";
        for (const auto& [u, v] : edges) {
            std::cout << (int)u << " " << (int)v << "\n";
        }
    }
};





#endif // MYGRAPHS_HH