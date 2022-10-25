# Force Delete Win

Force delete a file or folder held by other Windows processes, even if it is being used by other processes.

This can be used in race condition situations where a process has opened a folder and it tries to delete it just before closing the original handle.

This function will close all the handles of all the processes that have opened the requested file or directory, thus it may cause unexpected behavior on other programs or could leave your file system on an inconsistent state. **USE THIS UNDER YOUR OWN RISK**.


## Dependencies
To compile force-delete-win sources, you must have [Rust](https://rustup.rs/) installed.

## Installation

You can install this library by using either `pip` or `conda`:

```bash
# PyPi installation
pip install force-delete-win

# Conda installation
conda install force-delete-win -c conda-forge
```

## Building from source

To build from sources, you will require both a working stable or nightly Rust toolchain with
target `x86_64-pc-windows-msvc`, which can be installed using [rustup](https://rustup.rs/).

Additionally, force-delete-win uses [Maturin](https://github.com/PyO3/maturin) as the
build backend, which can be installed using `pip`:

```batch
pip install maturin
```

To test your compilation environment settings, you can build force-delete-win
sources locally, by executing:

```bash
maturin develop
```

This package depends on the following Rust crates:

* [PyO3](https://github.com/PyO3/pyo3): Library used to produce Python bindings from Rust code.
* [force-delete-win](https://github.com/andfoy/force-delete-win): Force file/folder deletion on Windows from Rust.
* [Maturin](https://github.com/PyO3/maturin): Build system to build and publish Rust-based Python packages.


## Usage

For using this package please do,

```python
from force_delete_win import force_delete_file_folder

deleted = force_delete_file_folder(path_to_folder_or_file)
```

## Test

We use `pytest` run our tests, just open a terminal and run,
### Python
```bash
python runtests.py
```

## Changelog
Visit our [CHANGELOG](CHANGELOG.md) file to learn more about our new features and improvements.

## Contribution guidelines
We follow PEP8 and PEP257 for pure python packages and Rust to compile extensions. We use MyPy type annotations for all functions and classes declared on this package. Feel free to send a PR or create an issue if you have any problem/question.
