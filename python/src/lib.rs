use std::ffi::OsString;

use pyo3::prelude::*;
use force_delete_win::force_delete_file_folder;

/// Force-delete a file or folder that is being held by other processes.
///
/// Parameters
/// ----------
/// fname: str
///     Full path to the file or folder to force-delete.
///
/// Returns
/// -------
/// `True` if the call was successful, else an error will be returned.
///
/// Notes
/// -----
/// This function will close all the handles of all the processes that have
/// opened the requested file or directory, thus it may cause unexpected
/// behaviour on other programs or could leave your file system on an
/// inconsistent state. USE THIS UNDER YOUR OWN RISK.
#[pyfunction]
#[pyo3(name = "force_delete_file_folder")]
fn force_delete_file_py(path: OsString) -> PyResult<bool> {
    Ok(force_delete_file_folder(path))
}

/// A Python module implemented in Rust.
#[pymodule]
fn force_delete_win(_py: Python, m: &PyModule) -> PyResult<()> {
    m.add_function(wrap_pyfunction!(force_delete_file_py, m)?)?;
    Ok(())
}
