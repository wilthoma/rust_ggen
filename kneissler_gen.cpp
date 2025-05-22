#include "mygraphs.hh"
#include "Kneissler.hh"
#include <chrono>
#include <iostream>
#include "CLI11.hpp"

std::chrono::high_resolution_clock::time_point tic_time;

void tic() {
    tic_time = std::chrono::high_resolution_clock::now();
}

void toc() {
    auto toc_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(toc_time - tic_time).count();
    std::cout << "Elapsed time: " << duration << " ms" << std::endl;
}

struct Range {
    int start = 0;
    int end = 0;

    // This tells CLI11 how to convert a string like "1:20" to a Range
    friend std::istream& operator>>(std::istream& in, Range& r) {
        std::string s;
        in >> s;
        auto pos = s.find(':');
        if (pos == std::string::npos)
            throw CLI::ConversionError("Range format must be start:end");

        r.start = std::stoi(s.substr(0, pos));
        r.end = std::stoi(s.substr(pos + 1));
        return in;
    }
};

int main(int argc, char** argv) {
    CLI::App app{"Kneissler graph and matrix generator"};

    Range r_loops;
    Range r_types;
    bool compute_matrices = false;
    bool compute_bases = false;
    bool even_edges = false;
    bool overwrite = false;

    app.add_option("range_loops", r_loops, "Range in format start:end")->required();
    app.add_option("range_types", r_types, "Range in format start:end")->required();
    app.add_flag("-m,--compute-matrices", compute_matrices, "Compute matrices");
    app.add_flag("-b,--compute-bases", compute_bases, "Compute bases");
    app.add_flag("-e,--even-edges", even_edges, "Use even edges");
    app.add_flag("-o,--overwrite", overwrite, "Overwrite existing files");


    CLI11_PARSE(app, argc, argv);

    // Check if the ranges are valid
    if (r_loops.start < 0 || r_loops.end < r_loops.start) {
        std::cerr << "Invalid range for loops: " << r_loops.start << ":" << r_loops.end << std::endl;
        return 1;
    }
    if (r_types.start < 0 || r_types.end < r_types.start || r_types.end > 3) {
        std::cerr << "Invalid range for types: " << r_types.start << ":" << r_types.end << std::endl;
        return 1;
    }

    for (int l =r_loops.start; l <= r_loops.end; ++l) {
        for (int k = r_types.start; k <= r_types.end; ++k) {
            // for (bool even_edges : {true}) {
            KneisslerGVS gvs(l, k, even_edges);
            
            if (compute_bases) {
                tic();
                gvs.build_basis(overwrite);
                toc();
            }
            // test_basis_vs_ref(gvs);

            if (compute_matrices && k>=2) {
                KneisslerContract D(l,k, even_edges);
                tic();
                D.build_matrix(overwrite);
                toc();
            //     test_matrix_vs_ref(D);
            }
            
        }
    }

    return 0;
}