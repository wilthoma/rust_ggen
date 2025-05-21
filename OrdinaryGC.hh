#ifndef ORDINARYGC_HH
#define ORDINARYGC_HH

#include "mygraphs.hh"

string get_type_string(bool even_edges) {
    return even_edges ? "even_edges" : "odd_edges";
}

class OrdinaryGVS {
    public:
        uint8_t num_vertices;
        uint8_t num_loops;
        bool even_edges;

        OrdinaryGVS(uint8_t n, uint8_t loops, bool even)
            : num_vertices(n), num_loops(loops), even_edges(even) {}
        
        string get_basis_file_path(){
            return "data/ordinary/" + get_type_string(even_edges) +
                   "/gra" + std::to_string(num_vertices) +
                   "_" + std::to_string(num_loops) + ".g6";
        }
        string get_input_file_path(){
            return "data/ordinary/" + get_type_string(even_edges) +
                   "/gra" + std::to_string(num_vertices) +
                   "_" + std::to_string(num_loops) + ".txt";
        }

        void build_basis(bool ignore_existing_files = false) {
            string infile = get_input_file_path();
            // check if 

        }
}





#endif // ORDINARYGC_HH