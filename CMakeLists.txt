cmake_minimum_required(VERSION 3.10)

project(basic_game)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "Setting the build type to 'Release'")
    set(CMAKE_BUILD_TYPE "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -O3 -Wall -Wextra -std=c++17")
else()
    message(STATUS "Setting the build type to 'Debug'")
    set(CMAKE_BUILD_TYPE "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -O0 -g -rdynamic -Wall -Wextra -std=c++17 -lasan -fsanitize=address")
    set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fopenmp -fno-omit-frame-pointer -lasan -std=c++17 -fsanitize=address")
    set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -lasan -fsanitize=address")
endif()


add_executable(basic_game src/main.cpp include/fighter.h src/fighter.cpp)
target_include_directories(basic_game PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

#---------
find_package(Doxygen)

if(DOXYGEN_FOUND)
    add_custom_target(
      doc ALL
      COMMAND ${DOXYGEN_EXECUTABLE} >> doxygen_output && cd latex && make >> doxygen_output
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      COMMENT "Generating API documentation with Doxygen"
      VERBATIM
    )
else()
    message(WARNING "Doxygen not found, unable to generate documentation")
endif()

ADD_CUSTOM_TARGET (distclean)
SET(DISTCLEANED
        CTestTestfile.cmake basic_game.cbp CMakeFiles html latex CMakeCache.txt CMakeDoxyfile.in CMakeDoxygenDefaults.cmake cmake_install.cmake  doxygen_output Makefile
        )

ADD_CUSTOM_COMMAND(
        DEPENDS clean
        COMMENT "cleaning distribution"
        COMMAND rm
        ARGS    -Rf CMakeTmp ${DISTCLEANED}
        TARGET  distclean
)


#----------------- Testing --------------------------------
enable_testing()

find_package(GTest REQUIRED)

add_executable(runTests test/test_fighter.cpp include/fighter.h src/fighter.cpp)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -O3 -Wall -Wextra -std=c++17")
target_include_directories(runTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include" "${GTEST_INCLUDE_DIRS}")
target_link_libraries(runTests ${GTEST_LIBRARIES} pthread)




#------------------- Python interface with swig ---------------
find_package(SWIG)

if(SWIG_FOUND)
    INCLUDE(${SWIG_USE_FILE})

    FIND_PACKAGE(PythonLibs)
    INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})

    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)

    SET(CMAKE_SWIG_FLAGS "")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra -std=c++17")

    SET_SOURCE_FILES_PROPERTIES(include/fighter.i PROPERTIES CPLUSPLUS ON)
    SWIG_ADD_MODULE(basic_game python include/fighter.i src/fighter.cpp)
    SWIG_LINK_LIBRARIES(basic_game ${PYTHON_LIBRARIES})
else()
    message(WARNING "SWIG not found, unable to generate python interface")
endif()


