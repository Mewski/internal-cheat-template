# Internal Cheat Template

Minimal Windows-focused template for developing and testing a DLL and a simple injector. Intended for education and experimentation only, do not use for malicious purposes.

## Overview

This repository contains a minimal DLL project plus a small injector program so you can prototype DLL-based tooling and learn Windows process injection techniques in a controlled environment.

## Requirements

- Git
- CMake >= 3.21
- Ninja (optional, recommended for fast builds)
- Visual Studio 2022 (Desktop development with C++) for MSVC toolchain

## Building from source

Clone the repository and initialize submodules:

```bash
git clone --recursive https://github.com/Mewski/internal-cheat-template.git
cd internal-cheat-template
git submodule update --init --recursive
```

Using Developer PowerShell for VS 2022:

```bash
patch -p1 -i ./patches/polyhook-2.patch
cmake -B build -G "Ninja" -DCMAKE_BUILD_TYPE=Release .
cmake --build build --config Release
```

Build artifacts (executables and DLLs) are written to `build/bin/`.

## Usage

From `build/bin/` run the injector to load a DLL into a target process:

```bash
./dll-injector.exe <pid> <path/to/template-dll.dll>
```

Example:

```bash
./dll-injector.exe 1337 ./template-dll.dll
```

Notes:
- The injector is a simple example and performs synchronous injection for demonstration.
- The `template-dll` project contains a basic `DllMain` and an exported helper so you can extend and experiment.

## Project layout

- `template-dll/` — DLL source and CMake targets
- `dll-injector/` — injector source and CMake targets
- `stub-executable/` — optional test host executable
- `dependencies/` — vendored dependencies and submodules

## Security & ethics

This repository is for learning and defensive purposes. Do not use it to harm others or violate laws. If you're unsure whether a use is permitted, seek guidance.

## License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.
