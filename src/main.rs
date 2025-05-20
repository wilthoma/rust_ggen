mod graphs;
mod kneissler;
use graphs::*;
use kneissler::*;

fn main() {
    // println!("Hello, world!");
    // Graph::tetrahedron_graph().print();
    // Graph::tetrastring_graph(1).print();

    // create_geng_ref(3,0);
    // create_geng_ref(4,0);
    // create_geng_ref(5,0);
    // create_geng_ref(5,0);
    // create_geng_ref(6,0);
    // create_geng_ref(7,0);
    // create_geng_ref(8,0);
    // create_geng_ref(9,0);
    // create_geng_ref(10,0);
    // create_geng_ref(11,0);



    // for l in 3..10 {
    //     for k in 1..l {
    //         if !is_satisfiable(l, k) {
    //             continue;
    //         }
    //         println!("{} {}", l, k);
    //         generate_graphs(l, k).unwrap();
    //         create_geng_ref(l,k);
    //         compare_file_to_ref(l,k);
    //     }
    // }

    let start = std::time::Instant::now();
    //generate_graphs(12,1).unwrap();

    for l in 12..13 {
        compute_all_kneissler_graphs(l, 0);
        // compute_all_kneissler_graphs(l, 1);
        // compute_all_kneissler_graphs(l, 2);
        // compute_all_kneissler_graphs(l, 3);
    }
    

    let duration = start.elapsed();
    println!("Time elapsed: {:?}", duration);


    // generate_graphs(3,0).unwrap();
    // generate_graphs(4,0).unwrap();
    // generate_graphs(5,0).unwrap();
    // generate_graphs(6,0).unwrap();
    // generate_graphs(7,0).unwrap();
    // generate_graphs(8,0).unwrap();
    // generate_graphs(9,0).unwrap();
    // let start = std::time::Instant::now();
    // generate_graphs(12,0).unwrap();
    // let duration = start.elapsed();
    // println!("Time elapsed: {:?}", duration);
    // // check correctness
    // compare_file_to_ref(12,0);
    // generate_graphs(11,0).unwrap();

}
