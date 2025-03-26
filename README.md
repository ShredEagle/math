# Math

Generic mathematical library, featuring strongly-typed support for linear algebra and other essential topics for graphics programming.

The library aims to provide type-safe implementations for common mathematical operations in computer graphics,
including vector and matrix algebra, transformations, pose representations,
and interpolation.

## Build (with Conan 2)

* Ensure [the pre-requisites](https://github.com/ShredEagle/public_build_info?tab=readme-ov-file#requirements) are met.
* Clone:
    ```bash
    git clone --recurse-submodule https://github.com/ShredEagle/math.git
    cd math
    ```
* Build:
    ```bash
    conan build ./conan/
    ```

## Build System and Dependencies

The project uses CMake for its build scripts,
which is sufficient for building the project.
However, CMake does not manage upstream dependencies.

To address this, a Conan recipe is provided on top of the CMake scripts.
It handles dependency management and facilitates the integration
inside a dependency graph.
