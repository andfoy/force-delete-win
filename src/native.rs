extern crate cxx;

#[cxx::bridge]
pub mod force_delete {
    unsafe extern "C++" {
        include!("wrapper.h");

        fn force_delete_file(fname: Vec<u16>);
    }
}
