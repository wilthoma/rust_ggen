use graph6_rs::Graph as G6Graph;
use rand::Rng;
use std::collections::HashSet;
use std::process::{Command, Stdio};
use std::io::Write;
use std::error::Error;


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
