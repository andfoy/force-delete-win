# Force Delete Win

Force delete a file or folder held by other Windows processes, even if it is being used by other processes.

This can be used in race condition situations where a process has opened a folder and it tries to delete it just before closing the original handle.

This function will close all the handles of all the processes that have opened the requested file or directory, thus it may cause unexpected behavior on other programs or could leave your file system on an inconsistent state. **USE THIS UNDER YOUR OWN RISK**.

## Installation

### Rust

In order to use `force-delete-win` in your [Rust](rust/README.md) library/program, you need to add
it as a dependency in your Cargo.toml:

```toml
[dependencies]
force-delete-win = 0.1

# To install from source
# force-delete-win = { git = "https://github.com/andfoy/force-delete-win" }
```

### Python

You can install the [Python](python/README.md) bindings by using either `pip` or `conda`:

```bash
# PyPi installation
pip install force-delete-win

# Conda installation
conda install force-delete-win -c conda-forge
```

## Usage

For using this package please do,

### Rust


```rust
use std::ffi::OsString;
use force_delete_win::force_delete_file_folder;

fn my_func() -> Result<bool, String> {
    let path = OsString::from(r"C:/my_path");
    match force_delete_file_folder(path){
        true => Ok(true),  // The folder was deleted properly
        false => Err("The folder wasn't deleted properly")
    }
}
```

### Python

```python
from force_delete_win import force_delete_file_folder

deleted = force_delete_file_folder(path_to_folder_or_file)
```

## Test

To run our tests, just open a terminal and run,

### Rust
```bash
cargo test
```

### Python
```bash
cd python
python runtests.py
```

For more information regarding local development, please check the README pages
for each programming language binding.

## Changelog
Visit our [CHANGELOG](CHANGELOG.md) file to learn more about our new features and improvements.

## Contribution guidelines
We use `cargo clippy` to lint this project and `cargo test` to test the Rust components. Python bindings support PEP257 and PEP8. Feel free to send a PR or create an issue if you have any problem/question.
