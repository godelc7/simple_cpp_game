# simple_cpp_game

This repository implements a basic game where an hero controlled by a monster fight against two monsters: an orc and a dragon. It's implemented in C++. The Hero and both monsters are executed inside two differents threads. This is enabled by the OpenMP library contained in GCC. The source code containts some unittests using the google test framework and a python interface generated with SWIG.


## Getting started

1. Dependencies: GCC 4 or more, python 3, SWIG 4 or more, CMake, Doxygen
2. Configure:
    ```bash
    $ cmake .
    ```
3. Build: the compiled code along with the test suite, a documentation(html and latex) and the python interface are generated automaticaly
    ```bash
    $ make
    ```
4. Run the game (excecutable):
    ```bash
    $ ./basic_game
    ```
5. Run the test suite:
    ```bash
    $ ./runTests
    ```
6. Run the game using the python interface (not fully implemented yet):
    ```bash
    $ python basic_game.py
    ```
7. Clean and distclean:
    ```bash
    $ make clean distclean
    ```

## TODO
Refer to the comments inside the source code
