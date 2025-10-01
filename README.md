# 🔍 HashScanner

A multithreaded C++ utility to scan files and compute their SHA-256 hash, then compare them against a list of IOC (Indicators of Compromise).

Perfect for detecting suspicious or compromised files on Linux and Windows systems.

## Features

- ⚡ Fast scanning with multithreading
- 📂 Cross-platform: Linux & Windows
- 🛡 Verify files using SHA-256 (OpenSSL)
- ⚙️ Flexible configuration via config.ini
- 📝 Log results and errors to files
- 🚫 Exclude specific directories using PrunePaths

## 📦 Installation
### Dependencies

- C++20 or newer  
- [OpenSSL](https://www.openssl.org/) (`libssl-dev` on Linux, `mingw-w64-x86_64-openssl` on Windows)
- [pkg-config](https://www.freedesktop.org/wiki/Software/pkg-config/)

### Build on Linux

```sh
sudo apt install build-essential libssl-dev pkg-config
make
```

### Build on Windows (MSYS2/MinGW)

```sh
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-openssl pkg-config
mingw32-make
```


## ⚙️ Configuration

Create a `config.ini` file in the project root:

```
scan_dir=*
output_file=output.log
error_file=error.log
ioc_file=C:\Path\To\IOC_hash.csv
prune_paths=C:\Windows
debug=0
```

### Explanation of parameters
| Key           | Description                                                                   |
| ------------- | ----------------------------------------------------------------------------- |
| `scan_dir`    | Path to the directory to scan. Use `*` to scan all disks. |
| `output_file` | Path to the file where scan results (matches) are logged.                     |
| `error_file`  | Path to the file where any errors encountered during the scan are logged.     |
| `ioc_file`    | Path to a CSV file containing **SHA-256** hashes (one per line).                  |
| `prune_paths` | List of directory paths to skip, separated by semicolons (`;`).               |
| `debug`       | Set to `1` to enable verbose debug logs, `0` to disable.                      |


## 📄 IOC File (CSV)
The IOC file (hashes.csv) should contain one SHA-256 hash per line:
```
9a0364b9e99bb480dd25e1f0284c8555...
2c26b46b68ffc68ff99b453c1d304134...
```
- Each hash must be 64 hexadecimal characters
- Case-insensitive
- No header required

## ▶️ Run
Once compiled, you can run the scanner using one of two methods:

### 1.Default Mode (config.ini in same folder as executable)
If a config.ini file is located in the same directory as the compiled executable, simply run:
```bash
./HashScanner
```
The application will automatically read the configuration from the local config.ini.

### 2.Manual Mode (custom config path)
If you want to specify a custom path to the configuration file:
```bash
./HashScanner /path/to/your/config.ini
```
Make sure the file is readable and correctly formatted. 
If the config file is invalid or missing required fields, the application will exit with an error message.

## 🛠 Planned Improvements

- Support for additional hash algorithms (SHA-1, SHA-512, MD5)
- Add multithreading to the file collection process to improve scan performance

## License

Apache-2.0 license
