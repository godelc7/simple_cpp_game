#!/bin/swig

%module basic_game
%{
// generic includes required to compile the wrapper code
#include "fighter.h"
%}

// includes containing the C/C++ to be interfaced to python
%include "fighter.h"

%pythoncode%{
    print("\nBASIC GAME PYTHON INTERFACE:  Main function for playing not implemented yet\n");

    if __name__ == "__main__":
        hero = Hero(); hero.SetRole(ROLE_HERO); hero.SetHealth(HEALTH_HERO)
        orc = Monster(); orc.SetRole(ROLE_ORC); orc.SetHealth(HEALTH_ORC)
        dragon = Monster(); dragon.SetRole(ROLE_DRAGON); dragon.SetHealth(HEALTH_DRAGON)
        hero.Print()
        orc.Print()
        dragon.Print()
%};

/*
 * TODO @Kamdoum: Complete the python interface
 *                - Main function to play the game using python module threading: separate file
 *                - Python tests using Pytest: separate file
 */
