# About

**Poloiouns GUI** uses the [Slint](https://slint.dev) to render the gui for the **Polonious text reader**

# Requirements

You will need a development environment that can compile `C++20`, [CMake 3.21](https://cmake.org/download/), and [Ninja](https://ninja-build.org/) for `-GNinja`. Slint don't provide binaries yet, so we will use the CMake integration that will automatically build the tools and library from source. Since it's implemented in the Rust programming language, this means that you also need to install a Rust compiler (1.70 or newer). You can easily install a Rust compiler following the instruction from the [Rust website](https://www.rust-lang.org/learn/get-started). We're going to use cmake's builtin FetchContent module to fetch the source code of Slint.

# Compile and Run

to compile the code into and executable:

```bash
cmake -GNinja .

cmake --build .
```

and then you can run the app

```
./polonious-gui
```

or for (UNIX/MAC) an easier build and run command

```
./run.sh
```

if you need to give permission to run the shell file, type `sudo chmod +x ./run.sh`
