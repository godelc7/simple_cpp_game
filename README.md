# Simple C++ Game

This repository implements a basic game where an hero controlled by a monster fight against two monsters: an orc and a dragon. It's implemented in C++. The Hero and both monsters are executed inside two different threads. This is enabled by the OpenMP library contained in GCC. The source code contains some unittest using the google test framework and a python interface generated with SWIG.

## Getting started

1. Dependencies: GCC 4 or more, python 3, SWIG 4 or more, CMake, Doxygen

2. Configure: Only out of source builds allowed, create a build folder first

    ```bash
    mkdir build
    cd build
    cmake ..
    ```

3. Build: the compiled code along with the test suite, a documentation(html and latex) and the python interface are generated automatically

    ```bash
    make
    ```

4. Run the game (executable):

    ```bash
    ./basic_game
    ```

5. Run the test suite:

    ```bash
    ./runTests
    ```

6. Run the game using the python interface (not fully implemented yet):

    ```bash
    python basic_game.py
    ```

7. One can also run coverage test, which requires `gcov`, `lcov` and `genhtml` installed:

    ```bash
    make runTestsCoverageLcov
    ```

8. Generate Doxygen documentation:

    ```bash
    make docs
    ```

9. Clean and distclean:

    ```bash
    make clean distclean
    ```

## TODO

Refer to the comments inside the source code
