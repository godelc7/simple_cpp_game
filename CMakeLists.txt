cmake_minimum_required(VERSION 3.10)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_compile_options (-fdiagnostics-color=always)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_compile_options (-fcolor-diagnostics)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "Setting the build type to 'Release'")
    set(CMAKE_BUILD_TYPE "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
elseif((CMAKE_BUILD_TYPE STREQUAL Debug) OR (CMAKE_BUILD_TYPE STREQUAL RelWithDebInfo))
    message(STATUS "Setting the build type to '${CMAKE_BUILD_TYPE}'")
    set(CMAKE_BUILD_TYPE "Debug")
endif()


project(basic_game)

#------------ COMPILER WARNINGS DEFINITIONS ---------- 
# COMPILER_WARNING_BASIC: a reasonable set of warnings: should be alway used
# COMPILER_WARNING_EXTENDED: an extended set of warnings, 
#     This can be too much, carefully check if all required
# COMPILER_WARNING_STRICT: only contains '-Werror' which treat warning as error
#-----------------------------------------------------

if((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") OR  (CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
    set(COMPILER_WARNING_BASIC -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic)
    set(COMPILER_WARNING_EXTENDED 
        -Wold-style-cast -Wcast-align -Wunused -Woverloaded-virtual -Wpedantic 
        -Wconversion -Wsign-conversion -Wmisleading-indentation 
        -Wduplicated-cond -Wduplicated-branches -Wlogical-op -Wnull-dereference
        -Wuseless-cast -Wdouble-promotion -Wformat=2
    )
    set(COMPILER_WARNING_STRICT -Werror)

    add_compile_options(${COMPILER_WARNING_BASIC} ${COMPILER_WARNING_EXTENDED})

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel") # Intel C/C++ compiler: ICC
    # TODO
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")  # Visual Studio C++
    set(COMPILER_WARNING_BASIC /W4 /w14640)
    set(COMPILER_WARNING_EXTENDED /w14242 /w14254 /w14263 /w14265 /w14287
        /we4289 /w14296 /w14311 /w14545 /w14546 /w14547 /w14549 /w14555
        /w14619 /w14640 /w14826 /w14905 /w14906 /w14928
    )
    set(COMPILER_WARNING_STRICT /WX)
    add_compile_options(${COMPILER_WARNING_BASIC} ${COMPILER_WARNING_EXTENDED})
endif()

#------------ BASIC COMPILE OPTIONS ------------
set(DEBUG_OPTIONS -O0 -g)
set(REALESE_OPTIONS -O3)

#------------ SANITIZER DEFINITIONS ---------- 
set(SANITIZER_BASIC_OPTIONS 
    -fno-omit-frame-pointer -fno-optimize-sibling-calls 
    -fsanitize-address-use-after-scope
)
set(ADRESS_SANITIZER_OPTIONS -fsanitize=address)
set(MEMORY_SANITIZER_OPTIONS -fsanitize=memory)
set(THREAD_SANITIZER_OPTIONS -fsanitize=thread)
set(LEAK_SANITIZER_OPTIONS   -fsanitize=leak)
set(UNDEFINED_BEHAVIOR_SANITIZER_OPTIONS -fsanitize=undefined)


# Sanitizers aren't supported under MINGW GCC
if(MINGW AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(ALL_SANITIZERS "")
else()
    set(ALL_SANITIZERS 
        ${SANITIZER_BASIC_OPTIONS}  ${ADRESS_SANITIZER_OPTIONS} 
        ${LEAK_SANITIZER_OPTIONS} ${UNDEFINED_BEHAVIOR_SANITIZER_OPTIONS}
    )
endif()


if((CMAKE_BUILD_TYPE STREQUAL Debug) OR (CMAKE_BUILD_TYPE STREQUAL RelWithDebInfo))
    add_compile_options(${DEBUG_OPTIONS} ${ALL_SANITIZERS})
    add_link_options(${ALL_SANITIZERS})
else()
    add_compile_options(${REALESE_OPTIONS})
endif()

include_directories(${CMAKE_SOURCE_DIR}/include)

# ===================== Processor cores ===========================
include(ProcessorCount)
ProcessorCount(N)
if(NOT N EQUAL 0)
    set(NUMBER_PROCS ${N})
    message(STATUS "Found number of physical cores: ${NUMBER_PROCS}")
else()
    set(NUMBER_PROCS 1)
    message(STATUS "Calling 'ProcessorCount()' failed: Using only 1 core")
endif()

#====================== OpenMP ====================================
find_package(OpenMP REQUIRED)
if(OpenMP_CXX_FOUND)
    add_compile_options(${OpenMP_CXX_FLAGS})
    add_link_options(${OpenMP_CXX_FLAGS})
endif()


# ====================== CPPCHECK ============================================
find_program(CPPCHECK_BIN NAMES cppcheck DOC "Path to the cppcheck executable")

if(CPPCHECK_BIN)
    execute_process(
        COMMAND ${CPPCHECK_BIN} --version
        OUTPUT_VARIABLE CPPCHECK_VERSION
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    message(STATUS "Found CppCheck: ${CPPCHECK_BIN} \
    (found version '${CPPCHECK_VERSION}')")

    set(CPPCHECK_THREADS_ARG "-j${NUMBER_PROCS}" 
        CACHE STRING "The number of threads to use for cppcheck"
    )
    set(CPPCHECK_PROJECT_ARG 
        "--project=${PROJECT_BINARY_DIR}/compile_commands.json"
        CACHE STRING 
        "The project file to use for cppcheck, e.g clang compilation database"
    )
    set(CPPCHECK_BUILD_DIR_ARG 
        "--cppcheck-build-dir=${PROJECT_BINARY_DIR}/analysis/cppcheck"
        CACHE STRING 
        "The cppcheck working folder for check results and cppcheck caching"
    )
    
    if(EXISTS "${CMAKE_SOURCE_DIR}/cppcheck-suppressions")
        set(CPPCHECK_SUPPRESSIONS_ARG 
            "--suppressions-list=${CMAKE_SOURCE_DIR}/cppcheck-suppressions"
            CACHE STRING 
            "The file defining warnings to be removed from cppcheck results"
        )
    else()
        set(CPPCHECK_SUPPRESSIONS_ARG ""
            CACHE STRING 
            "The file defining warnings to be suppressed from cppcheck results"
        )
    endif()

    if(EXISTS "${CMAKE_SOURCE_DIR}/cppcheck-exitcode-suppressions")
        set(CPPCHECK_EXITCODE_SUPPRESSIONS 
            "--exitcode-suppressions=${CMAKE_SOURCE_DIR}/cppcheck-exitcode-suppressions"
            CACHE STRING 
            "The file defining exitcode suppressions. For these errors, \
            cppcheck will display a message but will cause an error exit"
        )
    else()
        set(CPPCHECK_EXITCODE_SUPPRESSIONS ""
            CACHE STRING 
            "The file defining exitcode suppressions. For these errors, \
            cppcheck will display a message but will cause an error exit"
        )
    endif()

    set(CPPCHECK_ERROR_EXITCODE_ARG "--error-exitcode=0"
        CACHE STRING 
        "The exitcode to use by cppcheck if warning/errors are found. \
        By using a number different from zero(0), warnings are treated as errors"
    )
    set(CPPCHECK_CPP_STANDARD_ARG "--std=c++17"
        CACHE STRING 
        "The C/C++ standard to use. Possible values: c89, c99, c11, c++03, \
        c++11, c++14, c++17, c++20(default)"
    )
    set(CPPCHECK_ENABLE_ARGS "--enable=all"
        CACHE STRING "Additional checks to be enabled by cppcheck"
    )
    set(CPPCHECK_INCLUDE_PATHS "-I${CMAKE_SOURCE_DIR}/include"
        CACHE STRING 
        "List of include paths cppcheck uses. Each path is prepended by '-I'"
    )
    set(CPPCHECK_OTHER_ARGS 
        "--force" "--suppress=missingIncludeSystem" "--suppress=unmatchedSuppression"
        CACHE STRING "List of other usefull cppcheck arguments"
    )
    
    set(CPPCHECK_ALL_ARGS 
        ${CPPCHECK_THREADS_ARG}        ${CPPCHECK_BUILD_DIR_ARG} 
        ${CPPCHECK_SUPPRESSIONS_ARG}   ${CPPCHECK_EXITCODE_SUPPRESSIONS}
        ${CPPCHECK_ERROR_EXITCODE_ARG} ${CPPCHECK_ENABLE_ARGS}
        ${CPPCHECK_CPP_STANDARD_ARG}   ${CPPCHECK_INCLUDE_PATHS} 
        ${CPPCHECK_OTHER_ARGS}
    )

    file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/analysis/cppcheck)

    foreach(arg IN LISTS CPPCHECK_BIN CPPCHECK_ALL_ARGS)
        list(APPEND CMAKE_CXX_CPPCHECK ${arg})
    endforeach()
endif()


# ====================== CLANG-TIDY ============================================
find_program(CLANG_TIDY_BIN NAMES clang-tidy DOC "Path to the clang-tidy executable")

if(CLANG_TIDY_BIN)
    message(STATUS "Found Clang-tidy: ${CLANG_TIDY_BIN}")

    set(CLANG_TIDY_EXTRA_ARGS 
        "--extra-arg=-I${CMAKE_SOURCE_DIR}/include" "--extra-arg=-std=c++17"
        CACHE STRING 
        "Additional argument to append to the compiler command line"
    )
    set(CLANG_TIDY_BUILD_PATH_ARGS "-p" "${PROJECT_BINARY_DIR}/"
        CACHE STRING 
        "The folder in which clang-tidy searches for a \
        compile commmand database (compile_commands.json)"
    )
    set(CLANG_TIDY_COLOR_ARGS "--use-color"
        CACHE STRING "Use colors in clang-tidy diagnostics results"
    )
    set(CLANG_TIDY_HEADER_FILTER_ARGS 
        "--header-filter=${CMAKE_SOURCE_DIR}/include/*"
        CACHE STRING 
        "Header files matching this regular expression will be checked too"
    )
    set(CLANG_TIDY_CHECKS_ARG 
        "--checks=*,-llvmlibc-restrict-system-libc-headers,-llvm-include-order\
        ,-llvmlibc-callee-namespace,-modernize-use-trailing-return-type\
        ,-llvmlibc-implementation-in-namespace"
        CACHE STRING 
        "Comma separated list of checks(reg. expressions) that \
        are enabled. Checks prefixed with '-' are deactivated"
    )
    
    set(CLANG_TIDY_CMD_LINE 
        ${CLANG_TIDY_BIN} ${CLANG_TIDY_EXTRA_ARGS} ${CLANG_TIDY_BUILD_PATH_ARGS}
        ${CLANG_TIDY_HEADER_FILTER_ARGS} ${CLANG_TIDY_CHECKS_ARG}
    )

    foreach(arg IN LISTS CLANG_TIDY_CMD_LINE)
        #list(APPEND CMAKE_CXX_CLANG_TIDY ${arg})
    endforeach()
endif()


# ======================== TARGET: basic_game =================================
add_executable(basic_game src/main.cpp include/fighter.h src/fighter.cpp)
target_include_directories(basic_game PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")


# ======================== DOXYGEN ============================================
find_package(Doxygen)

if(DOXYGEN_FOUND)
    add_custom_target(
      doc
      COMMAND ${DOXYGEN_EXECUTABLE} >> doxygen_output && cd latex && make >> doxygen_output
      WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
      COMMENT "Generating API documentation with Doxygen"
      VERBATIM
    )
else()
    message(WARNING "Doxygen not found, unable to generate documentation")
endif()


# ======================== TARGET: distclean =================================
ADD_CUSTOM_TARGET (distclean)
SET(DISTCLEANED
    CTestTestfile.cmake *basic_game* runTests
    CMakeFiles html latex CMakeCache.txt CMakeDoxyfile.in
    CMakeDoxygenDefaults.cmake cmake_install.cmake  doxygen_output Makefile
)

ADD_CUSTOM_COMMAND(
    DEPENDS clean
    COMMENT "cleaning distribution"
    COMMAND rm
    ARGS    -Rf CMakeTmp ${DISTCLEANED}
    TARGET  distclean
)


# ======================== TARGET: Testing =================================
enable_testing()

find_package(GTest REQUIRED)

find_package(Threads)
if(Threads_FOUND)
    set(THREADS_LINKER_FLAG Threads::Threads)
else()
    set(THREADS_LINKER_FLAG pthread)
endif()


add_executable(runTests test/test_fighter.cpp include/fighter.h src/fighter.cpp)
#set(CMAKE_CXX_FLAGS)
target_include_directories(runTests PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include ${GTEST_INCLUDE_DIRS}
)
target_link_libraries(runTests 
    ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} 
    ${THREADS_LINKER_FLAG}
)
add_test(NAME "Complete_tests" COMMAND runTests ARGS --gtest_color=yes)




# ================ TARGET: Python extension with SWIG ===================
cmake_policy(SET CMP0078 NEW)
cmake_policy(SET CMP0086 NEW)
find_package(SWIG)
FIND_PACKAGE(PythonLibs)
find_package(Python COMPONENTS Interpreter Development)

if(SWIG_FOUND AND Python_FOUND)
    INCLUDE(${SWIG_USE_FILE})
    
    INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})
    link_directories(${Python_LIBRARY_DIRS})

    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)

    SET(CMAKE_SWIG_FLAGS "")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra -std=c++17")

    SET_SOURCE_FILES_PROPERTIES(include/fighter.i PROPERTIES CPLUSPLUS ON)
    SWIG_ADD_LIBRARY(basic_game_swig 
        LANGUAGE python 
        SOURCES include/fighter.i src/fighter.cpp
    )
    SWIG_LINK_LIBRARIES(basic_game_swig ${PYTHON_LIBRARIES})
else()
    message(WARNING 
        "SWIG and/or Python not found, unable to generate python interface")
endif()


