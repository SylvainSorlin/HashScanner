# 🔍 HashScanner

A multithreaded C++ utility to scan files and compute their SHA-256 hash, then compare them against a list of IOC (Indicators of Compromise).

Perfect for detecting suspicious or compromised files on Linux and Windows systems.

## Features

- ⚡ Fast scanning with multithreading
- 📂 Cross-platform: Linux & Windows
- 🛡 Verify files using SHA-256 (OpenSSL)
- ⚙️ Flexible configuration:
    - config.ini file
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
scan_dir=C:\Path\To\Scan
output_file=output.log
error_file=error.log
ioc_file=C:\Path\To\IOC_hash.csv
prune_paths=C:\Windows
```

- `scan_dir`: Directory to scan  
- `output_file`: File to log scan results  
- `error_file`: File to log errors  
- `ioc_file`: CSV file containing IOC hashes (**ONLY SHA256**)
- `prune_paths`: Semicolon-separated list of folder names to skip  



## 📄 IOC File (CSV)
The IOC file (hashes.csv) should contain one SHA-256 hash per line:
```
9a0364b9e99bb480dd25e1f0284c8555...
2c26b46b68ffc68ff99b453c1d304134...
```
- Each hash must be 64 hexadecimal characters
- Case-insensitive

## 🛠 Planned Improvements

- Support for additional hash algorithms (SHA-1, SHA-512, MD5)
- Add multithreading to the file collection process to improve scan performance

## License

MIT License
