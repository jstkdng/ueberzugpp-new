# ueberzugpp-new

[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=jstkdng_ueberzugpp-new&metric=alert_status)](https://sonarcloud.io/summary/new_code?id=jstkdng_ueberzugpp-new)

This project uses C++23, supports GCC (libstdc++), Clang and Apple Clang (libc++)

Available in the [AUR](https://aur.archlinux.org/packages/ueberzugpp-new-git)

## Be sure to clone with submodules

`git clone https://github.com/jstkdng/ueberzugpp-new --recurse-submodules`

## Libraries needed

- openssl
- libxcb
- xcb-util-errors
- xcb-util-image
- wayland (libwayland)
- wayland-protocols
- extra-cmake-modules
- libvips

## To build

```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```
