# Building Serdes

## Prerequisites

- C++20 compiler (g++, clang++)
- Meson build system
- Ninja build tool

## Install build tools

```bash
# Homebrew (macOS/Linux)
brew install meson ninja

# Ubuntu/Debian
sudo apt install meson ninja-build

# Fedora
sudo dnf install meson ninja-build

# Arch
sudo pacman -S meson ninja
```

## Dependencies

All dependencies are managed automatically by Meson:

- **nlohmann/json** (required by JsonArray demo) - If not installed on your system, Meson will automatically download it via wraps

No manual dependency installation needed!

## Build

```bash
# Setup (first time only)
meson setup builddir

# Compile
meson compile -C builddir
```

## Run demos

```bash
./builddir/Demo/helloworld
./builddir/Demo/basic
./builddir/Demo/constexpr
./builddir/Demo/jsonarray
```

## Clean build

```bash
rm -rf builddir
meson setup builddir
```
