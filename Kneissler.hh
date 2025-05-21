#ifndef KNEISSLER_HH
#define KNEISSLER_HH

#include "mygraphs.hh"


#include <vector>
#include <algorithm>
#include <cstdint>
#include <set>
#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <map>

using namespace std;

Graph barrel_graph(uint8_t k, const vector<uint8_t>& p) {
    Graph g(2 * k);
    // generate rims of barrel
    for (uint8_t j = 0; j < k; ++j) {
        g.add_edge(j, (j + 1) % k);
        g.add_edge(k + j, k + (j + 1) % k);
    }
    // generate spokes
    g.add_edge(k - 1, 2 * k - 1);
    // cout << "k= " << (int)k << " -- " << p.size() << endl;
    for (size_t i = 0; i < p.size(); ++i) {
        g.add_edge(i, k + p[i]);
    }
    g.check_valid(0, "barrel_graph");
    return g;
}

vector<vector<uint8_t>> all_permutations(uint8_t n) {
    vector<uint8_t> v(n);
    for (uint8_t i = 0; i < n; ++i) v[i] = i;
    vector<vector<uint8_t>> perms;
    do {
        perms.push_back(v);
    } while (std::next_permutation(v.begin(), v.end()));
    return perms;
}

vector<Graph> all_barrel_graphs(uint8_t k) {
    vector<Graph> result;
    auto perms = all_permutations(k - 1);
    for (const auto& p : perms) {
        result.push_back(barrel_graph(k, p));
    }
    return result;
}

Graph tbarrel_graph(uint8_t k, const vector<uint8_t>& p) {
    Graph g(2 * k - 1);
    // one rim of length k
    for (uint8_t j = 0; j < k; ++j) {
        g.add_edge(j, (j + 1) % k);
    }
    // cout << "a " << g.to_g6() << endl;
    // other rim of length k-1
    for (uint8_t j = 0; j < k - 1; ++j) {
        g.add_edge(k + j, k + ((j + 1) % (k - 1)));
    }
    // cout << "b " << g.to_g6() << endl;
    g.add_edge(k - 1, 2 * k - 2);
    // cout << "c " << g.to_g6() << endl;
    for (size_t i = 0; i < p.size(); ++i) {
        g.add_edge(i, k + p[i]);
    }
    // cout << "d " << g.to_g6() << " " << g.edges.size() <<  endl;
    if (!g.check_valid(1, "tbarrel_graph")) {
        // print permutation
        cout << "tbarrel_graph: k= " << (int)k << " -- ";
        for (size_t i = 0; i < p.size(); ++i) {
            cout << (int)p[i] << " ";
        }
        cout << endl;
        // print edges 
        for (const auto& e : g.edges) {
            cout << (int)e.u << " " << (int)e.v << " " << (int)e.data << endl;
        }

        std::exit(-1);
    }
    return g;
}

vector<Graph> all_tbarrel_graphs(uint8_t k) {
    vector<Graph> result;
    auto perms = all_permutations(k - 1);
    for (const auto& p : perms) {
        result.push_back(tbarrel_graph(k, p));
    }
    return result;
}

Graph xtbarrel_graph(uint8_t k, const vector<uint8_t>& p) {
    Graph g(2 * k - 1);
    for (uint8_t j = 0; j < k - 1; ++j) {
        g.add_edge(j, (j + 1) % (k - 1));
        g.add_edge(k + j, k + (j + 1) % (k - 1));
    }
    // cout << "a " << g.to_g6() << endl;
    g.add_edge(k - 1, 2 * k - 2);
    g.add_edge(k - 1, k - 2);
    // cout << "b " << g.to_g6() << endl;
    for (size_t i = 0; i < p.size(); ++i) {
        if (p[i] + 2 < k) {
            g.add_edge(i, k + p[i]);
        } else {
            g.add_edge(i, k - 1);
        }
    }

    // cout << "c " << g.to_g6() << endl;
    if (!g.check_valid(1, "xtbarrel_graph")) {
        cout << "xtbarrel_graph: k= " << (int)k << " -- ";
        for (size_t i = 0; i < p.size(); ++i) {
            cout << (int)p[i] << " ";
        }
        cout << endl;
        // print edges 
        for (const auto& e : g.edges) {
            cout << (int)e.u << " " << (int)e.v << " " << (int)e.data << endl;
        }

        std::exit(-1);
    }
    return g;
}

vector<Graph> all_xtbarrel_graphs(uint8_t k) {
    vector<Graph> result;
    auto perms = all_permutations(k - 1);
    for (const auto& p : perms) {
        result.push_back(xtbarrel_graph(k, p));
    }
    return result;
}

Graph triangle_graph(uint8_t k, const vector<uint8_t>& p) {
    Graph g(2 * k);
    for (uint8_t j = 0; j < k; ++j) {
        g.add_edge(j, (j + 1) % k);
    }
    for (uint8_t j = 0; j < k - 1; ++j) {
        g.add_edge(k + 1 + j, k + 1 + (j + 1) % (k - 1));
    }
    g.add_edge(k - 1, k);
    g.add_edge(k, 2 * k - 1);
    for (size_t i = 0; i < p.size(); ++i) {
        if (p[i] + 2 < k) {
            g.add_edge(i, k + 1 + p[i]);
        } else {
            g.add_edge(i, k);
        }
    }
    g.check_valid(0, "triangle_graph");
    return g;
}

vector<Graph> all_triangle_graphs(uint8_t k) {
    vector<Graph> result;
    auto perms = all_permutations(k - 1);
    for (const auto& p : perms) {
        result.push_back(triangle_graph(k, p));
    }
    return result;
}

Graph hgraph(uint8_t k, const vector<uint8_t>& p) {
    Graph g(2 * k);
    for (uint8_t j = 0; j < k - 1; ++j) {
        g.add_edge(j, (j + 1) % (k - 1));
        g.add_edge(k + 1 + j, k + 1 + (j + 1) % (k - 1));
    }
    g.add_edge(k - 2, k - 1);
    g.add_edge(k - 1, 2 * k - 1);
    g.add_edge(k - 1, k);
    for (size_t i = 0; i < p.size(); ++i) {
        if (i + 2 < k) {
            g.add_edge(i, k + p[i]);
        } else if (p[i] > 0) {
            g.add_edge(k, k + p[i]);
        }
    }
    g.check_valid(0, "hgraph");
    return g;
}

vector<Graph> all_hgraph_graphs(uint8_t k) {
    vector<Graph> result;
    auto perms = all_permutations(k - 1);
    for (const auto& p : perms) {
        if (p[k - 2] > 0) {
            result.push_back(hgraph(k, p));
        }
    }
    return result;
}

void ensure_folder_of_filename_exists(const string& filename) {
    size_t pos = filename.find_last_of("/\\");
    if (pos != string::npos) {
        string folder = filename.substr(0, pos);
        if (!std::filesystem::exists(folder)) {
            std::filesystem::create_directories(folder);
        }
    }
}
    

void save_matrix_to_sms_file(const map<pair<size_t, size_t>, int>& matrix, int nrows, int ncols, const string& filename) {
    ensure_folder_of_filename_exists(filename);
    ofstream file(filename);
    if (!file) throw std::runtime_error("Failed to open file for writing");
    // first line is rows cols M
    file << nrows << " " << ncols << " " << matrix.size() << "\n";
    for (const auto& [key, value] : matrix) {
        // sms file uses 1-based indexing
        file << key.first+1 << " " << key.second+1 << " " << value << "\n";
    }
    // last line is 0 0 0
    file << "0 0 0\n";
    // close file
    file.close();
}

string get_type_string(bool even_edges) {
    return even_edges ? "even_edges" : "odd_edges";
}

class KneisslerGVS  {
    public:
        uint8_t num_loops;
        uint8_t kn_type; // 0=trivalent generators, 1=relation generators, 2=all trivalent, 3=trivalent complement
        bool even_edges;
        uint8_t num_vertices;
        size_t num_edges;
        uint8_t k;

        KneisslerGVS(uint8_t loops, uint8_t kntype_, bool even_edges_)
            : num_loops(loops), kn_type(kntype_), even_edges(even_edges_) {
                k = num_loops - 1;
                num_vertices = 2 * k;
                num_edges = 3 * k;
                if (kn_type == 1) {
                    num_edges -= 1;
                    num_vertices -= 1;
                }
            }

        string get_basis_file_path(){
            return "data/kneissler/" + get_type_string(even_edges) +
                   "/gra" + std::to_string(num_loops) +
                   "_" + std::to_string(kn_type) + ".g6";
        }

        vector<string> get_basis_g6() {
            // if (!is_valid()) {
            //     // Return empty list if graph vector space is not valid.
            //     cerr << "Empty basis: not valid" << endl;
            //     return {};
            // }
            return Graph::load_from_file(get_basis_file_path());
        }

        void build_basis(bool ignore_existing_files = false) {
            string fname = get_basis_file_path();
            cout << "Building basis for " << fname << endl;
            // exit if file exists and ignore_existing_files is false
            if (!ignore_existing_files && std::ifstream(fname)) {
                return;
            }
            ensure_folder_of_filename_exists(fname);
            auto perms = all_permutations(k - 1);
            std::set<std::string> g6s;

            if (kn_type == 0) {
                for (const auto& p : perms) {
                    Graph g = barrel_graph(k, p);
                    if (!g.has_odd_automorphism(even_edges)) {
                        g6s.insert(g.to_canon_g6());
                    }
                }
            } else if (kn_type == 1) {
                for (const auto& p : perms) {
                    Graph g = tbarrel_graph(k, p);
                    if (!g.has_odd_automorphism(even_edges)) {
                        string g6 = g.to_canon_g6();
                        Graph::check_g6_valid(g6, 1, "can_tbarrel_graph");
                        g6s.insert(g6);
                    }
                    if (p[k - 2] != k-2) {
                        Graph gg = xtbarrel_graph(k, p);
                        if (!gg.has_odd_automorphism(even_edges)) {
                            gg.check_valid(1, "can_xtbarrel_graph0 ");
                            // cout << "gg " << gg.to_g6() << endl;
                            string g6 = gg.to_canon_g6();
                            if (!Graph::check_g6_valid(g6, 1, "can_xtbarrel_graph ")) {
                                cout << "gg " << gg.num_vertices  << endl;
                                // print edge list 
                                // for (const auto& e : gg.edges) {
                                //     cout << (int)e.u << " " << (int)e.v << " " << (int)e.data << endl;
                                // }
                                cout << "g6 " << g6 << endl;
                                std::exit(-1);
                            }
                            g6s.insert(g6);
                        }
                    }
                }
            } else if (kn_type == 2) {
                for (const auto& p : perms) {
                    Graph g = barrel_graph(k, p);
                    if (!g.has_odd_automorphism(even_edges)) {
                        g6s.insert(g.to_canon_g6());
                    }
                    Graph gg = triangle_graph(k, p);
                    if (!gg.has_odd_automorphism(even_edges)) {
                        g6s.insert(gg.to_canon_g6());
                    }
                    if (p[k - 2] > 0) {
                        Graph ggg = hgraph(k, p);
                        if (!ggg.has_odd_automorphism(even_edges)) {
                            g6s.insert(ggg.to_canon_g6());
                        }
                    }
                }
            } else if (kn_type == 3) {
                // we assume the type 0 and 2 basis files exist
                KneisslerGVS gvs0(num_loops, 0, even_edges);
                KneisslerGVS gvs2(num_loops, 2, even_edges);

                string fname0 = gvs0.get_basis_file_path();
                string fname2 = gvs2.get_basis_file_path();

                vector<string> gs0_ = Graph::load_from_file(fname0);
                vector<string> gs2_ = Graph::load_from_file(fname2);
                for (const auto& g : gs2_) {
                    if (std::find(gs0_.begin(), gs0_.end(), g) == gs0_.end()) {
                        g6s.insert(g);
                    }
                }
            } else {
                throw std::runtime_error("Unknown graph type");
            }
            vector<string> gs2(g6s.begin(), g6s.end());
            std::sort(gs2.begin(), gs2.end());
            Graph::save_to_file(gs2, fname);
        }

        string to_string() const {
            return "KneisslerGVS(" + std::to_string(num_loops) + ", " +
                   std::to_string(kn_type) + ", " + get_type_string(even_edges) + ")";
        }
};

class KneisslerContract {
    public:

    uint8_t num_loops;
    uint8_t kn_type; // 0=trivalent generators, 1=invalid, 2=all trivalent, 3=trivalent complement
    bool even_edges;

    KneisslerGVS domain;
    KneisslerGVS target;


    KneisslerContract(uint8_t loops, uint8_t kntype_, bool even_edges_)
        : num_loops(loops), kn_type(kntype_), even_edges(even_edges_), 
          domain(loops, kntype_, even_edges_),
          target(loops, 1, even_edges_) {
            if (kn_type == 1 || kn_type > 3) {
                throw std::invalid_argument("Invalid kn_type for contraction");
            }
        }

    string get_matrix_file_path(){
        return "data/kneissler/" + get_type_string(even_edges) +
               "/contractD" + std::to_string(num_loops) +
               "_" + std::to_string(kn_type) + ".txt";
    }

    void build_matrix(bool ignore_existing_files = false) {
        string fname = get_matrix_file_path();
        ensure_folder_of_filename_exists(fname);
        // exit if file exists and ignore_existing_files is false
        if (!ignore_existing_files && std::ifstream(fname)) {
            return;
        }
        auto perms = all_permutations(num_loops - 1);
        std::set<std::string> g6s;

        cout << "Building matrix for contraction" << endl;

        vector<string> in_basis = domain.get_basis_g6();
        vector<string> out_basis = target.get_basis_g6();
        map<string, size_t> in_basis_map;
        map<string, size_t> out_basis_map;
        map<pair<size_t, size_t>, int> matrix;
        int num_rows = in_basis.size();
        int num_cols = out_basis.size();

        for (size_t i = 0; i < in_basis.size(); ++i) {
            in_basis_map[in_basis[i]] = i;
        }
        for (size_t i = 0; i < out_basis.size(); ++i) {
            out_basis_map[out_basis[i]] = i;
        }

        for (const string s : in_basis) {
            Graph g = Graph::from_g6(s);
            auto v = g.get_contractions_with_sign(even_edges);
            for (const auto& [g1, sign] : v) {
                auto [g1s, sign2] = g1.to_canon_g6_sgn(even_edges);
                int val = sign * sign2;
                if (out_basis_map.find(g1s) != out_basis_map.end()) {
                    size_t row = in_basis_map[s];
                    size_t col = out_basis_map[g1s];
                    if (matrix.find({row, col}) != matrix.end()) {
                        matrix[{row, col}] += val;
                    } else {
                        matrix[{row, col}] = val;
                    }
                }
            }
        }
        // save matrix to file
        save_matrix_to_sms_file(matrix, num_rows, num_cols, fname);
        cout << "Matrix saved to " << fname << endl;
    }


};


void test_basis_vs_ref(KneisslerGVS V) {
    // test if the basis is correct
    cout << "Checking basis correctness "<< V.to_string() << "..." << endl;  
    vector<string> g6s = V.get_basis_g6();
    string ref_fname = "data/kneissler/ref/" + get_type_string(V.even_edges) +
                   "/gra" + std::to_string(V.num_loops) +
                   "_" + std::to_string(V.kn_type) + ".g6";
    cout << "Reference file: " << ref_fname << endl;
    vector<string> ref_g6s = Graph::load_from_file(ref_fname);

    // need to re-canonize reference basis
    for (size_t i = 0; i < ref_g6s.size(); ++i) {
        Graph g = Graph::from_g6(ref_g6s[i]);
        auto g1s = g.to_canon_g6();
        ref_g6s[i] = g1s;
        // sanity checks
        if (g.has_odd_automorphism(V.even_edges)) {
            cout << "Reference graph has odd automorphism: " << g1s << endl;
        }

    }

    // check whether the entries are the same
    //std::sort(g6s.begin(), g6s.end());
    std::sort(ref_g6s.begin(), ref_g6s.end());
    std::set<string> g6s_set(g6s.begin(), g6s.end());
    std::set<string> ref_g6s_set(ref_g6s.begin(), ref_g6s.end());
    std::set<string> diff;
    std::set_difference(g6s_set.begin(), g6s_set.end(),
                        ref_g6s_set.begin(), ref_g6s_set.end(),
                        std::inserter(diff, diff.begin()));
    if (diff.size() > 0) {
        cout << "The following graphs are in the basis but not in the reference:" << endl;
        for (const auto& g6 : diff) {
            cout << g6 << endl;
        }
    } else {
        cout << "All graphs in the basis are in the reference" << endl;
    }
    // check whether the entries are the same
    diff.clear();
    std::set_difference(ref_g6s_set.begin(), ref_g6s_set.end(),
                        g6s_set.begin(), g6s_set.end(),
                        std::inserter(diff, diff.begin()));
    if (diff.size() > 0) {
        cout << "The following graphs are in the reference but not in the basis:" << endl;
        for (const auto& g6 : diff) {
            cout << g6 << endl;
        }
    } else {
        cout << "All graphs in the reference are in the basis" << endl;
    }
}


#endif // KNEISSLER_HH