fn main() {
    println!("cargo:rustc-link-lib=static=blissshim");
    println!("cargo:rustc-link-search=native=.");
    println!("cargo:rerun-if-changed=bliss_shim.cpp");
    println!("cargo:rustc-link-lib=dylib=c++"); // On macOS
    println!("cargo:rustc-link-search=native=.");
    println!("cargo:rustc-link-lib=static=bliss"); // or "dylib=bliss"
}
