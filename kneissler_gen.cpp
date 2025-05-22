#include "mygraphs.hh"
#include "Kneissler.hh"
#include <chrono>
#include <iostream>

std::chrono::high_resolution_clock::time_point tic_time;

void tic() {
    tic_time = std::chrono::high_resolution_clock::now();
}

void toc() {
    auto toc_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(toc_time - tic_time).count();
    std::cout << "Elapsed time: " << duration << " ms" << std::endl;
}


int main() {
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