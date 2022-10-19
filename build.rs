use cxx_build::CFG;

use std::env;
use std::path::Path;

fn main() {
    if std::env::var("DOCS_RS").is_ok() {
        return;
    }

    println!("cargo:rerun-if-changed=src/lib.rs");
    println!("cargo:rerun-if-changed=src/native.rs");
    println!("cargo:rerun-if-changed=src/csrc");
    println!("cargo:rerun-if-changed=include/");

    let manifest_dir = env::var("CARGO_MANIFEST_DIR").unwrap();
    let include_path = Path::new(&manifest_dir).join("include");
    CFG.exported_header_dirs.push(&include_path);
    CFG.exported_header_dirs.push(&Path::new(&manifest_dir));

    cxx_build::bridge("src/native.rs")
        .file("src/csrc/delete.cpp")
        .flag_if_supported("-std=gnu++14")
        .flag_if_supported("/EHsc")
        .define("_GLIBCXX_USE_CXX11_ABI", "0")
        .warnings(false)
        .extra_warnings(false)
        .compile("filedelete");
}
