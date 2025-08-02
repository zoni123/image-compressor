# image-compressor

## Overview

An image compressor which works with P3 and P6 .ppm formats and with 24-bit .bmp format, with 4 levels of compression available. It uses the SVD provided by LAPACK to compress images.

## Installation

### Prerequisites

- GCC
- LAPACK + LAPACKE
- BLAS

### Windows (MSYS2)

1. **Install MSYS2**:
	- Download and install MSYS2 from [MSYS2](https://www.msys2.org/).

2. **Update MSYS2**:
	- Open the MSYS2 MSYS terminal:
	```sh
	pacman -Syu
	```

3. **Install GCC, LAPACK, BLAS, and LAPACKE**:
	- Open the MSYS2 MSYS terminal:
	```sh
	pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-lapack mingw-w64-x86_64-lapacke mingw-w64-x86_64-openblas
	```

4. **Set Environment Variables**:
	- Ensure that the `bin` directory of MSYS2 MinGW-w64 is added to your system's `PATH`.

### Unix-like Systems (Linux, macOS)

1. **Install GCC**:
	- On Debian-based systems:
    ```sh
    sudo apt-get update
    sudo apt-get install build-essential
    ```
	- On Red Hat-based systems:
    ```sh
    sudo dnf install gcc gcc-c++
    ```

2. **Install LAPACK, BLAS, and LAPACKE**:
	- On Debian-based systems:
    ```sh
    sudo apt-get install liblapack-dev libblas-dev liblapacke-dev
    ```
	- On Red Hat-based systems:
    ```sh
    sudo dnf install lapack-devel blas-devel lapacke-devel
    ```

## Build/Clean

```sh
make build
```

```sh
make clean
```

## Usage

```sh
./image_compressor <input_image1.ppm/.bmp> <input_image2.ppm/.bmp> ... <input_image32.ppm/.bmp>
```
