# HashScanner

HashScanner is a cross-platform (Windows & Linux) C++ application for scanning directories, computing SHA256 hashes of files, and checking them against a list of Indicators of Compromise (IOCs). It supports configuration via a simple `config.ini` file.

## Features

- Recursively scans directories for files  
- Skips empty files, symlinks, and pruned paths  
- Computes SHA256 hashes using OpenSSL  
- Compares file hashes against a CSV list of IOCs  
- Logs results and errors to configurable files  
- Multi-threaded scanning for performance  
- Configurable via `config.ini` or command-line argument  

## Dependencies

- C++17 or newer  
- [OpenSSL](https://www.openssl.org/) (`libssl-dev` on Linux, `vcpkg install openssl` on Windows)  
- A C++17-compatible compiler (g++, clang++, MSVC)  
- [pkg-config](https://www.freedesktop.org/wiki/Software/pkg-config/) (recommended for Linux builds)  

## Configuration

Create a `config.ini` file in the project root:

```
scan_dir=C:\Path\To\Scan
output_file=output.log
error_file=error.log
ioc_file=C:\Path\To\IOC_hash.csv
prune_paths=SharePoint;Temp;Windows
```

- `scan_dir`: Directory to scan  
- `output_file`: File to log scan results  
- `error_file`: File to log errors  
- `ioc_file`: CSV file containing IOC hashes (**ONLY SHA256**)
- `prune_paths`: Semicolon-separated list of folder names to skip  

## Build

### Linux

```sh
sudo apt install build-essential libssl-dev pkg-config
make
```

### Windows (MSYS2/MinGW)

```sh
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-openssl pkg-config
mingw32-make
```

## Run

By default, the program looks for config.ini in the current directory:
```sh
./HashScanner
```

You can also provide a custom configuration file as an argument:
```sh
./HashScanner custom_config.ini
```

On Windows (PowerShell or CMD):
```sh
HashScanner.exe
HashScanner.exe custom_config.ini
```

## Output

- Results are written to the file specified by `output_file`
- Errors are written to the file specified by `error_file`

## License

MIT License

## Author

SylvainSorlin