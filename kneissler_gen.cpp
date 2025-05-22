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
    app.add_option("range_loops", r_loops, "Range in format start:end")->required();
    app.add_option("range_types", r_types, "Range in format start:end")->required();

    CLI11_PARSE(app, argc, argv);

    // Example usage
    for (int l =10; l < 15; ++l) {
        for (int k = 0; k < 4; ++k) {
            // for (bool even_edges : {true}) {
            for (bool even_edges : {true, false}) {
                KneisslerGVS gvs(l, k, even_edges);
                
                tic();
                gvs.build_basis(true);
                toc();
                // test_basis_vs_ref(gvs);

                if (k>=2) {
                    KneisslerContract D(l,k, even_edges);
                    tic();
                    D.build_matrix(true);
                    toc();
                //     test_matrix_vs_ref(D);
                }
            }
        }
    }

    return 0;
}