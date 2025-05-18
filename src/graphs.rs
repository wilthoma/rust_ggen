use graph6_rs::Graph as G6Graph;
use rand::Rng;
use std::collections::HashSet;
use std::process::{Command, Stdio};
use std::io::Write;
use std::error::Error;

#[derive(Clone)]
pub struct Graph {
    num_vertices: u8,
    edges: Vec<(u8, u8)>,
}

impl Graph {
    pub fn new(num_vertices: u8) -> Self {
        Graph {
            num_vertices,
            edges: Vec::new(),
        }
    }

    pub fn add_edge(&mut self, u: u8, v: u8) {
        if u<v {
            self.edges.push((u, v));
        } else {
            self.edges.push((v, u));
        }
    }

    // pub fn to_g6(&self) -> String {
    //     // Create an adjacency matrix
    //     let n = self.num_vertices as usize;
    //     let mut adj = vec![vec![false; n]; n];
    //     for (u, v) in &self.edges {
    //         adj[*u as usize][*v as usize] = true;
    //         adj[*v as usize][*u as usize] = true;
    //     }
    //     // Manually build the bit vector for G6Graph
    //     let mut bit_vec = Vec::new();
    //     for i in 0..n {
    //         for j in (i + 1)..n {
    //             bit_vec.push(adj[i][j] as u8);
    //         }
    //     }
    //     let g6 = G6Graph {
    //         n: n,
    //         bit_vec,
    //     };
    //     g6.to_g6()
    // }

    pub fn from_g6_ref(g6_str: &str) -> Self {
        let g6 = G6Graph::from_g6(g6_str).unwrap();
        let num_vertices = g6.n as u8;
        let bit_vec = g6.bit_vec;
        let mut edges = Vec::new();
        for i in 0..num_vertices{
            for j in 0..num_vertices{
                if i<j && bit_vec[  (i as usize)*g6.n+(j as usize)]>0 {
                    edges.push((i, j));
                }
            }
        }
        Graph { num_vertices, edges }
    }

    pub fn to_g6(&self) -> String {
        let graph = self;
        let n = graph.num_vertices;
        assert!(n <= 62, "This encoder only supports graphs with at most 62 vertices.");

        // Write N(n)
        let mut result = String::new();
        result.push((n + 63) as char);

        // Create adjacency bit vector in the correct order
        let mut bitvec = Vec::new();
        for j in 1..n {
            for i in 0..j {
                let bit = graph.edges.contains(&(i, j)) || graph.edges.contains(&(j, i));
                bitvec.push(bit as u8);
            }
        }

        // Pad bitvec with zeros to make length multiple of 6
        while bitvec.len() % 6 != 0 {
            bitvec.push(0);
        }

        // Encode into 6-bit chunks
        for chunk in bitvec.chunks(6) {
            let mut value = 0u8;
            for (i, &bit) in chunk.iter().enumerate() {
                value |= bit << (5 - i);
            }
            result.push((value + 63) as char);
        }

        result
    }

    pub fn add_edge_across(&self, e1idx : usize, e2idx: usize) -> Graph
    {
        // creates new vertices on the edges with indices e1idx and e1idx, and joins them by an edge
        assert_ne!(e1idx, e2idx, "Edges must be distinct");
        let new_n = self.num_vertices + 2;
        let n = self.num_vertices;
        let mut new_edges = vec![];
        let v1 = n;
        let v2 = n+1;
        for i in 0..n {
            let (u,v) = self.edges[i];
            if i==e1idx {
                new_edges.push((u,v1));
                new_edges.push((v,v1));
                new_edges.push((v1,v2));
            } else if i == e2idx {
                new_edges.push((u,v2));
                new_edges.push((v,v2));
            } else {
                new_edges.push( (u,v) );
            }
        }
        new_edges.sort();
        Graph {
            num_vertices: new_n,
            edges: new_edges,
        }
    }

    pub fn union(&self, other : &Graph) -> Graph {
        let new_n = self.num_vertices + other.num_vertices;
        // join the two edges vectors
        let mut edges = self.edges.clone();
        edges.extend(
            other
                .edges
                .iter()
                .map(|&(u, v)| (u + self.num_vertices, v + self.num_vertices)),
        );
        Graph {
            num_vertices: new_n,
            edges,
        }
    }


    pub fn contract_edge(&self, eidx : usize) -> Graph {
        let new_n = self.num_vertices-1;
        let (u,v) = self.edges[eidx];
        assert!(u<v);

        let new_edges = self.edges.iter()
                .enumerate()
                .filter_map(|(i, &(a, b))| {
                    if i == eidx {
                        return None;
                    }
                    let aa = if a < v { a } else if a == v { u } else { a - 1 };
                    let bb = if b < v { b } else if b == v { u } else { b - 1 };
                    if aa < bb {
                        Some((aa, bb))
                    } else if bb < aa {
                        Some((bb, aa))
                    } else {
                        None
                    }
                })
                .collect::<HashSet<_>>() // remove duplicates
                .into_iter()
                .collect::<Vec<_>>(); // convert to Vec
        
        
        Graph {
            num_vertices : new_n,
            edges : new_edges,
        }
    }

    pub fn contract_edge_opt(&self, eidx : usize) -> Option<Graph> {
        let g = self.contract_edge(eidx);
        if g.edges.len() +1  == self.edges.len() {
            Some(g)
        } else {
            None
        }
    }

    pub fn from_g6(g6: &str) -> Graph {
        let bytes = g6.as_bytes();
        assert!(!bytes.is_empty(), "Empty g6 string");

        // Decode N(n)
        let first = bytes[0];
        assert!(first >= 63, "Invalid graph6 string");

        let n = match first {
            63..=126 => (first - 63) as u8,
            _ => panic!("This decoder only supports n ≤ 62 (1-byte N(n))"),
        };

        // Compute number of bits in the upper triangle: n(n-1)/2
        let num_bits = (n as usize * (n as usize - 1)) / 2;
        let num_bytes = (num_bits + 5) / 6;

        let bit_data = &bytes[1..=num_bytes];
        let mut bits = Vec::with_capacity(num_bits);

        for &byte in bit_data {
            assert!(byte >= 63, "Invalid graph6 data byte");
            let val = byte - 63;
            for i in (0..6).rev() {
                bits.push((val >> i) & 1);
            }
        }

        // Trim any extra bits (if padding was added)
        bits.truncate(num_bits);

        // Reconstruct edge list in order: (0,1), (0,2), (1,2), (0,3), (1,3), (2,3), ...
        let mut edges = Vec::new();
        let mut k = 0;
        for j in 1..n {
            for i in 0..j {
                if bits[k] == 1 {
                    edges.push((i, j));
                }
                k += 1;
            }
        }

        Graph {
            num_vertices: n,
            edges,
        }
    }

    pub fn save_to_file(g6_list: &[String], filename: &str) -> std::io::Result<()> {
        let mut file = std::fs::File::create(filename)?;
        // first line is number of graphs
        writeln!(file, "{}", g6_list.len())?;
        // write each graph6 string
        for g6 in g6_list {
            writeln!(file, "{}", g6)?;
        }
        Ok(())
    }

    pub fn load_from_file(filename: &str) -> std::io::Result<Vec<String>> {
        let file = std::fs::File::open(filename)?;
        let reader = std::io::BufReader::new(file);
        // read first line and trsnform to int
        let mut lines = reader.lines();
        let first_line = lines.next().unwrap()?;
        let num_graphs: usize = first_line.trim().parse().unwrap();
        let mut g6_list = Vec::new();
        for line in lines.take(num_graphs) {
            let g6 = line?;
            g6_list.push(g6);
        }
        if g6_list.len() != num_graphs {
            return Err(std::io::Error::new(
                std::io::ErrorKind::InvalidData,
                "Number of graphs in file does not match the first line",
            ));
        }
        Ok(g6_list)
    }

    pub fn tetrahedron_graph() -> Graph {
        Graph {
            num_vertices : 4,
            edges : vec![(0,1),(0,2),(0,3),(1,2),(1,3),(2,3)]
        }
    }

    pub fn tetrastring_graph(n_blocks : u8) -> Graph {
        let n = 4*n_blocks;
        let mut edges = vec![];
        for i in 0..n_blocks {
            edges.push((4*i, 4*i+1));
            edges.push((4*i, 4*i+2));
            edges.push((4*i+1, 4*i+2));
            edges.push((4*i+1, 4*i+3));
            edges.push((4*i+2, 4*i+3));
            edges.push((4*i, 4*((i+1)%n_blocks)));
        }
        Graph {
            num_vertices : n,
            edges
        }

    }
}


fn canonicalize_and_dedup_g6<I>(
    g6_iter: I,
) -> Result<HashSet<String>, Box<dyn Error>>
where
    I: IntoIterator<Item = String>,
{
    let labelg_path = "labelg";

    // Launch `labelg -g` subprocess
    let mut child = Command::new(labelg_path)
        .arg("-g") // output graph6 format
        .stdin(Stdio::piped())
        .stdout(Stdio::piped())
        .spawn()?;

    // Write all input g6 strings to stdin
    {
        let stdin = child.stdin.as_mut().ok_or("Failed to open stdin")?;
        for g6 in g6_iter {
            writeln!(stdin, "{}", g6)?;
        }
    }

    // Collect and parse canonical outputs
    let output = child.wait_with_output()?;
    if !output.status.success() {
        return Err("labelg failed".into());
    }

    let canonical_g6s = String::from_utf8(output.stdout)?
        .lines()
        .map(str::to_string)
        .collect::<HashSet<_>>();

    Ok(canonical_g6s)
}



pub fn generate_graphs(g : usize, d : usize) -> Result<(), Box<dyn std::error::Error>> {
    // d is the defect

    if g<3 || d < 0 || d +8 > 2*g {
        println!("Warning: generate_graphs called with non-satisfiable paramters.");
        return Ok(());
    }
    let n = 3*g-3 -d;
    let e = 2*g-2-d; 

    assert!(n*(n-1)/2 >= e);

    let filename = format!("data/graphs{}_{}.g6", g, d);
    if d>0 {
        // contract an edge
        let otherfilename = format!("data/graphs{}_{}.g6", g, d-1);
        let g6s = Graph::load_from_file(&otherfilename)?;
        let g6_iter = g6s.into_iter().map(|s| Graph::from_g6(&s))
                    .flat_map(move |g| (0..=e).filter_map(move |idx| g.contract_edge_opt(idx) ))
                    .map(|g| g.to_g6());
        let g6_canon = canonicalize_and_dedup_g6(g6_iter)?;
        let g6_vec: Vec<String> = g6_canon.into_iter().collect();
        Graph::save_to_file(&g6_vec, &filename)?;

    } else if d==0 {
        // start with the tetrastring
        let mut g6list = vec![];
        if g%2 == 1 
        {
            g6list.push(Graph::tetrastring_graph(((g-1)/2) as u8).to_g6());
        }

        for l1 in 3..g-3 {
            let l2 = g-l1;
            if l1<l2 {
                continue;
            } 
            let fname1 = format!("data/graphs{}_{}.g6", l1, 0);
            let fname2 = format!("data/graphs{}_{}.g6", l2, 0);
            let g6s1 = Graph::load_from_file(&fname1)?;
            let g6s2 = Graph::load_from_file(&fname2)?;
            let g6s1 = g6s1.into_iter().map(|s| Graph::from_g6(&s)).collect::<Vec<_>>();
            let g6s2 = g6s2.into_iter().map(|s| Graph::from_g6(&s)).collect::<Vec<_>>();
            for g1 in g6s1.iter() {
                let e1 = g1.edges.len();
                for g2 in g6s2.iter() {
                    let e2 = g2.edges.len();
                    let gg = g1.union(g2);

                    for i in 0..e1 {
                        for j in 0..e2 {
                            let ggg = gg.add_edge_across(i, j+e1);
                            g6list.push(ggg.to_g6());
                        }
                    }
                }
            }
        }

        
        // add graphs obtained by connecting two edges
        if g>3 {
            let otherfname = format!("data/graphs{}_0.g6", g-1);
            let g6s = Graph::load_from_file(&otherfname)?;
            let ee = e-3;

            let g6s = g6s.into_iter().map(|s| Graph::from_g6(&s))
                        .flat_map(move |g| (0..ee).flat_map(
                            |j| (j+1..ee).map(|k| 
                                g.add_edge_across(j, k).to_g6()
                            ) 
                        )).collect();
            g6list.extend(g6s);

        }
        



        let g6_canon = canonicalize_and_dedup_g6(g6list)?;
        let g6_vec: Vec<String> = g6_canon.into_iter().collect();
        Graph::save_to_file(&g6_vec, &filename)?;
    }

    Ok(())
}


#[cfg(test)]
mod tests {
    use super::*;

    fn graphs_equal(g1: &Graph, g2: &Graph) -> bool {
        if g1.num_vertices != g2.num_vertices {
            return false;
        }
        let mut e1 = g1.edges.clone();
        let mut e2 = g2.edges.clone();
        e1.sort();
        e2.sort();
        e1 == e2
    }

    #[test]
    fn test_random_graphs_g6_roundtrip() {
        let mut rng = rand::rng();
        for _ in 0..10 {
            let n = rng.random_range(1..=20); // keep n small for test speed
            let mut g = Graph::new(n);
            // Randomly add edges
            for u in 0..n {
                for v in (u + 1)..n {
                    if rng.random_bool(0.5) {
                        g.add_edge(u, v);
                    }
                }
            }
            let g6 = g.to_g6();
            let g2 = Graph::from_g6(&g6);
            let g6_2 = g2.to_g6();
            assert!(
                g6 == g6_2,
                "G6 roundtrip failed: g6={}, back={:?}",
                g6,
                g6_2
            );
            assert!(
                graphs_equal(&g, &g2),
                "Graph roundtrip failed: orig={:?}, g6={}, back={:?}",
                g.edges,
                g6,
                g2.edges
            );
        }
    }

    #[test]
    fn test_canonicalize_and_dedup_g6() {
        // These three are isomorphic, the last is not
        let g6_graphs = vec![
            "D??".to_string(),
            "D_@".to_string(),
            "D`?".to_string(), // all isomorphic
            "D?@".to_string(), // triangle + pendant (non-isomorphic)
        ];


        let result = canonicalize_and_dedup_g6(g6_graphs.clone());
        match result {
            Ok(canon_set) => {
                // Should deduplicate the first three, so only 2 canonical forms
                println!("Canonical forms: {:?}", canon_set);
                assert_eq!(
                    canon_set.len(),
                    3,
                    "Expected 3 canonical forms, got {:?}",
                    canon_set
                );
            }
            Err(e) => {
                panic!("canonicalize_and_dedup_g6 failed: {}", e);
            }
        }
    }
}
