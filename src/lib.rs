//! Force-remove files or folders on Windows
//!
//! This crate provides a simple function that allows to force-delete a folder or file, even if it is being
//! used by other processes. This can be used in race condition situations where a process has opened a folder
//! and it tries to delete it just before closing the original handle.
//!
//! This function will close all the handles of all the processes that have
//! opened the requested file or directory, thus it may cause unexpected
//! behaviour on other programs or could leave your file system on an
//! inconsistent state. USE THIS UNDER YOUR OWN RISK.

mod native;

use std::ffi::OsString;
use std::os::windows::prelude::OsStrExt;

use windows::core::PCWSTR;
use windows::Win32::Storage::FileSystem::{GetFileAttributesW, INVALID_FILE_ATTRIBUTES};

use native::force_delete::force_delete_file;


/// Force-delete a file or folder that is being held by other processes.
///
/// # Arguments
/// * `fname` - Full path to the file or folder to force-delete.
///
/// # Returns
/// `true` if the call was successful, else an error will be returned.
///
/// # Notes
/// This function will close all the handles of all the processes that have
/// opened the requested file or directory, thus it may cause unexpected
/// behaviour on other programs or could leave your file system on an
/// inconsistent state. USE THIS UNDER YOUR OWN RISK.
pub fn force_delete_file_folder(fname: OsString) -> bool {
    let mut file_buf: Vec<u16> = fname.encode_wide().collect();
    file_buf.push(0);
    force_delete_file(file_buf);

    let file_vec: Vec<u16> = fname.encode_wide().collect();
    let file_pwstr = PCWSTR(file_vec.as_ptr());
    unsafe {
        let file_attrs = GetFileAttributesW(file_pwstr);
        return file_attrs == INVALID_FILE_ATTRIBUTES;
    }
}


#[cfg(test)]
mod tests {
    use super::*;
    use windows::Win32::Storage::FileSystem::{CreateFileW, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, FILE_ACCESS_FLAGS, FILE_SHARE_MODE};
    use windows::Win32::System::SystemServices::{GENERIC_WRITE, GENERIC_READ};
    use windows::Win32::UI::Shell::ShellExecuteW;
    use windows::Win32::UI::WindowsAndMessaging::SW_SHOWNORMAL;
    use tempfile::TempDir;

    use std::{thread, time};

    #[test]
    fn it_works() {
        let tmp_dir = TempDir::new().unwrap();
        let tmp_path = OsString::from(tmp_dir.path().as_os_str());
        let path_vec: Vec<u16> = tmp_path.encode_wide().collect();
        let path_pwstr = PCWSTR(path_vec.as_ptr());

        unsafe {
            ShellExecuteW(None, None, path_pwstr, None, None, SW_SHOWNORMAL);
        }

        let ten_millis = time::Duration::from_millis(1000);
        thread::sleep(ten_millis);

        unsafe {
            let file_attrs = GetFileAttributesW(path_pwstr);
            assert!(file_attrs != INVALID_FILE_ATTRIBUTES);
        }

        unsafe {
            let err = CreateFileW(path_pwstr, FILE_ACCESS_FLAGS(GENERIC_WRITE | GENERIC_READ),
                FILE_SHARE_MODE(0), None, OPEN_EXISTING,
                 FILE_FLAG_BACKUP_SEMANTICS, None).unwrap_err();
            assert!(err.code() != windows::core::HRESULT(0))
        }

        let res = force_delete_file_folder(OsString::from(tmp_path));
        assert!(res);
    }
}
