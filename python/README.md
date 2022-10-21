# Force Delete Win

Force delete a file or folder held by other Windows processes, even if it is being used by other processes.

This can be used in race condition situations where a process has opened a folder and it tries to delete it just before closing the original handle.

This function will close all the handles of all the processes that have opened the requested file or directory, thus it may cause unexpected behavior on other programs or could leave your file system on an inconsistent state. **USE THIS UNDER YOUR OWN RISK**.

## Installation

For now, our package is only installable directly from source,

```toml
[dependencies]
force_delete_win = { git = "https://github.com/andfoy/force-delete-win" }
```

## Usage

For using this package please do,

```rust
use std::ffi::OsString;
use force_delete_win::force_delete_file_folder;

fn my_func() -> Result<bool, String>{
    let path = OsString::from(r"C:/my_path");
    match force_delete_file_folder(path){
        true => Ok(true),  // The folder was deleted properly
        false => Err("The folder wasn't deleted properly")
    }
}
```

## Test

To run our tests, just open a terminal and run,

```
cargo test
```
