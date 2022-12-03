cmake_minimum_required(VERSION 3.10)

# Comment out this part if you want to use the GNU toolchain instead
#set(CMAKE_C_COMPILER clang.exe)
#set(CMAKE_CXX_COMPILER clang++.exe)

# ======================== Some basic definitions ==============================
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_compile_options (-fdiagnostics-color=always)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_compile_options (-fcolor-diagnostics)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # for clang-tidy
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "Setting the build type to 'Release'")
    set(CMAKE_BUILD_TYPE "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
elseif((CMAKE_BUILD_TYPE STREQUAL Debug) OR 
       (CMAKE_BUILD_TYPE STREQUAL RelWithDebInfo))
    message(STATUS "Setting the build type to '${CMAKE_BUILD_TYPE}'")
    set(CMAKE_BUILD_TYPE "Debug")
endif()

project(
    "Basic Game"
    VERSION "1.0.5"
    DESCRIPTION "This project implements a simple command line game as a showcase"
)

include_directories(${PROJECT_SOURCE_DIR}/include)

# Prohibit in-source builds
if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
    message(FATAL_ERROR "In-source build is not allowed. Make sure to create \
    a build directory and call cmake from there")
endif()


# ============================== Processor cores ===============================
include(ProcessorCount)
ProcessorCount(N)
if(NOT N EQUAL 0)
    set(NUMBER_PROCS ${N})
    message(STATUS "Found number of physical cores: ${NUMBER_PROCS}")
else()
    set(NUMBER_PROCS 1)
    message(STATUS "Calling 'ProcessorCount()' failed: Using only 1 core")
endif()


#=========================== SOURCE CODE FILES =================================
file(GLOB ALL_HEADER_FILES ${PROJECT_SOURCE_DIR}/include/*.h)
file(GLOB ALL_CPP_FILES 
    ${PROJECT_SOURCE_DIR}/src/*.cpp 
    ${PROJECT_SOURCE_DIR}/test/*.cpp
)
set(ALL_SOURCES ${ALL_HEADER_FILES} ${ALL_CPP_FILES})


#=============================== OpenMP ========================================
find_package(OpenMP REQUIRED)
if(OpenMP_CXX_FOUND)
    add_compile_options(${OpenMP_CXX_FLAGS})
    add_link_options(${OpenMP_CXX_FLAGS})
endif()


# ========================= Basic compile options ==============================
set(DEBUG_OPTIONS -O0 -g)
set(RELEASE_OPTIONS -O3)


# ==================== Option CLANG_TIDY_ANALYSIS ==============================
option(
    CLANG_TIDY_ANALYSIS 
    "Boolean option whether to use clang-tidy analysis or not. If this option \
    is set to ON, then the compiler builtin analysis tool is deactivated. \
    Otherwise the compiler builtin analysis tool is activated."
    ON
)


# ================ Static analysis : Compiler Warnings =========================
# COMPILER_WARNING_BASIC: a reasonable set of warnings: should be alway used
# COMPILER_WARNING_EXTENDED: an extended set of warnings, 
#     This can be too much, carefully check if all required
# COMPILER_WARNING_STRICT: only contains '-Werror' which treat warning as error

if((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") OR 
   (CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
    set(COMPILER_WARNING_BASIC -Wall -Wextra -Wshadow 
       -Wnon-virtual-dtor -pedantic
    )
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


# ================= Static analysis : builtin analyzer =========================
# The builtin analyzers also perform a kind of static analysis, but they
# much more expensive than the compiler warnings, since they use compiler
# instrumentation and code injection to perform their task. Those are therefore
# recommended only for debug builds and test purpose.
if(CMAKE_BUILD_TYPE STREQUAL Debug AND NOT CLANG_TIDY_ANALYSIS)
    if((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") AND 
       (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 7.0))
        set(BUILTIN_ANALYZER_OPTION -fanalyzer)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        set(BUILTIN_ANALYZER_OPTION /analyze)
    endif()
    add_compile_options(${BUILTIN_ANALYZER_OPTION})
endif()


# ====================== Static analysis : CPPCHECK ============================
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

    set(CPPCHECK_THREADS_ARG -j${NUMBER_PROCS} 
        CACHE STRING "The number of threads to use for cppcheck"
    )
    set(CPPCHECK_PROJECT_ARG 
        --project=${PROJECT_BINARY_DIR}/compile_commands.json
        CACHE STRING 
        "The project file to use for cppcheck, e.g clang compilation database"
    )
    set(CPPCHECK_BUILD_DIR_ARG 
        --cppcheck-build-dir=${PROJECT_BINARY_DIR}/analysis/cppcheck
        CACHE STRING 
        "The cppcheck working folder for check results and cppcheck caching"
    )

    if(EXISTS "${CMAKE_SOURCE_DIR}/cppcheck-suppressions")
        set(CPPCHECK_SUPPRESSIONS_ARG 
            --suppressions-list=${CMAKE_SOURCE_DIR}/cppcheck-suppressions
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
            --exitcode-suppressions=${CMAKE_SOURCE_DIR}/cppcheck-exitcode-suppressions
            CACHE STRING 
            "The file defining exit code suppressions. For these errors, \
            cppcheck will display a message but will cause an error exit"
        )
    else()
        set(CPPCHECK_EXITCODE_SUPPRESSIONS ""
            CACHE STRING 
            "The file defining exit code suppressions. For these errors, \
            cppcheck will display a message but will cause an error exit"
        )
    endif()

    set(CPPCHECK_ERROR_EXITCODE_ARG --error-exitcode=0
        CACHE STRING 
        "The exit code to use by cppcheck if warning/errors are found. \
        By using a number different from zero(0), warnings are treated as errors"
    )
    set(CPPCHECK_LANGUAGE_ARG --language=c++
        CACHE STRING 
        "Forces cppcheck to analyze all files as the given programming language, \
        Possible values are c, c++"
    )
    set(CPPCHECK_CPP_STANDARD_ARG --std=c++17
        CACHE STRING 
        "The C/C++ standard to use. Possible values: c89, c99, c11, c++03, \
        c++11, c++14, c++17, c++20(default)"
    )
    set(CPPCHECK_ENABLE_ARGS --enable=all
        CACHE STRING "Additional checks to be enabled by cppcheck"
    )
    set(CPPCHECK_INCLUDE_PATHS -I${CMAKE_SOURCE_DIR}/include
        CACHE STRING 
        "List of include paths cppcheck uses. Each path is prepended by '-I'"
    )
    set(CPPCHECK_OTHER_ARGS 
        --force --suppress=missingIncludeSystem --suppress=unmatchedSuppression
        CACHE STRING "List of other useful cppcheck arguments"
    )
    
    set(CPPCHECK_ALL_ARGS 
        #${CPPCHECK_THREADS_ARG}        
        ${CPPCHECK_BUILD_DIR_ARG} 
        ${CPPCHECK_SUPPRESSIONS_ARG}   ${CPPCHECK_EXITCODE_SUPPRESSIONS}
        ${CPPCHECK_ERROR_EXITCODE_ARG} ${CPPCHECK_ENABLE_ARGS}
        ${CPPCHECK_LANGUAGE_ARG}       ${CPPCHECK_CPP_STANDARD_ARG}
        ${CPPCHECK_INCLUDE_PATHS}      ${CPPCHECK_OTHER_ARGS}
    )

    file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/analysis/cppcheck)

    foreach(arg IN LISTS CPPCHECK_BIN CPPCHECK_ALL_ARGS CMAKE_CXX_FLAGS ALL_SOURCES)
        list(APPEND CPPCHECK_COMMAND ${arg})
    endforeach()

    add_custom_target(Cppcheck_analysis ALL
        COMMAND ${CPPCHECK_COMMAND}
        COMMENT "Running Cppcheck static analysis: ${CPPCHECK_COMMAND}"
    )
endif()


# ======================== Static analysis : CLANG-TIDY ========================
find_program(CLANG_TIDY_BIN 
    NAMES clang-tidy 
    DOC "Path to the clang-tidy executable"
)

if(CLANG_TIDY_BIN AND CLANG_TIDY_ANALYSIS)
    message(STATUS "Found Clang-tidy: ${CLANG_TIDY_BIN}")

    set(CLANG_TIDY_EXTRA_ARGS 
        --extra-arg=-I${CMAKE_SOURCE_DIR}/include --extra-arg=-std=c++17
        --extra-arg=-Wno-unknown-warning-option
        #--extra-arg=-Wunknown-argument
        #--extra-arg=-Qunused-arguments
        CACHE STRING 
        "Additional argument to append to the compiler command line"
    )
    set(CLANG_TIDY_BUILD_PATH_ARGS "-p" "${PROJECT_BINARY_DIR}/"
        CACHE STRING 
        "The folder in which clang-tidy looks for a \
        compile command database, e.g compile_commands.json"
    )
    set(CLANG_TIDY_COLOR_ARGS --use-color
        CACHE STRING "Use colors in clang-tidy diagnostics results"
    )
    set(CLANG_TIDY_HEADER_FILTER_ARGS 
        --header-filter=${CMAKE_SOURCE_DIR}/include/*
        CACHE STRING 
        "Header files matching this regular expression will be checked too"
    )
    string(CONCAT CHECKS_ARG "--checks=*," 
        "-llvmlibc-restrict-system-libc-headers,"
        "-llvm-include-order,"
        "-llvmlibc-callee-namespace,"
        "-modernize-use-trailing-return-type,"
        "-llvmlibc-implementation-in-namespace"
    )
    set(CLANG_TIDY_CHECKS_ARG ${CHECKS_ARG}
        CACHE STRING "Comma separated list of checks(reg. expressions) that \
        are enabled. Checks prefixed with '-' are deactivated"
    )
    
    set(CLANG_TIDY_CMD_LINE 
        ${CLANG_TIDY_BIN} ${CLANG_TIDY_EXTRA_ARGS} ${CLANG_TIDY_BUILD_PATH_ARGS}
        ${CLANG_TIDY_HEADER_FILTER_ARGS} ${CLANG_TIDY_CHECKS_ARG}
    )

    foreach(arg IN LISTS CLANG_TIDY_CMD_LINE CMAKE_CXX_FLAGS ALL_SOURCES)
        list(APPEND CLANG_TIDY_COMMAND ${arg})
    endforeach()

    add_custom_target(Clang_tidy_analysis ALL
        COMMAND ${CLANG_TIDY_COMMAND}
        COMMENT "Running Clang-tidy static analysis: ${CLANG_TIDY_COMMAND}"
    )
endif()


# ================== Static analysis : include-what-you-use ====================
find_program(iwyu_path NAMES include-what-you-use iwyu iwyu_tool.py)
if(iwyu_path)
    message(STATUS "Found include-what-you-use: ${iwyu_path}")
    set(iwyu_arguments
        -std=c++17
        -I${CMAKE_SOURCE_DIR}/include
        ${OpenMP_CXX_FLAGS}
        -Qunused-arguments
        #-Wunknown-warning-option
        #-Wunknown-argument
        -Wno-unknown-warning-option
        -fno-common
        -pipe 
        -fstack-protector-strong 
        -fdiagnostics-color=always 
        -fvisibility-inlines-hidden
        -Xiwyu --transitive_includes_only 
        -Xiwyu --no_fwd_decls
        -Xiwyu --cxx17ns
    )

    foreach(src_file IN LISTS ALL_CPP_FILES)
        list(
            APPEND iwyu_commands 
            COMMAND ${iwyu_path} ${iwyu_arguments} ${src_file}
        )
    endforeach(src_file in ALL_CPP_FILES)

    add_custom_target(
        iwyu_check ALL
        ${iwyu_commands}
        COMMENT "Running include-what-you-use check on CPP files with command: \
        ${iwyu_path} ${iwyu_arguments}"
    )
else()
    message(WARNING "Program 'include-what-you-use' not found")
endif()


# ====================== Runtime analysis: SANITIZER ===========================
set(SANITIZER_BASIC_OPTIONS 
    -fno-omit-frame-pointer -fno-optimize-sibling-calls 
    -fsanitize-address-use-after-scope
)
set(ADDRESS_SANITIZER_OPTIONS -fsanitize=address)
set(MEMORY_SANITIZER_OPTIONS -fsanitize=memory)
set(THREAD_SANITIZER_OPTIONS -fsanitize=thread)
set(LEAK_SANITIZER_OPTIONS   -fsanitize=leak)
set(UNDEFINED_BEHAVIOR_SANITIZER_OPTIONS -fsanitize=undefined)

# Sanitizers aren't supported under MINGW
if(MINGW AND 
(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
    set(ALL_SANITIZERS "")
else()
    set(ALL_SANITIZERS 
        ${SANITIZER_BASIC_OPTIONS}  ${ADDRESS_SANITIZER_OPTIONS} 
        ${UNDEFINED_BEHAVIOR_SANITIZER_OPTIONS}
    )
endif()

if((CMAKE_BUILD_TYPE STREQUAL Debug) OR 
   (CMAKE_BUILD_TYPE STREQUAL RelWithDebInfo))
    add_compile_options(${DEBUG_OPTIONS} ${ALL_SANITIZERS})
    add_link_options(${ALL_SANITIZERS})
else()
    add_compile_options(${RELEASE_OPTIONS})
endif()


# ======================== TARGET: basic_game ==================================
add_executable(basic_game src/main.cpp include/fighter.h src/fighter.cpp)
target_include_directories(
    basic_game 
    PRIVATE "${PROJECT_SOURCE_DIR}/include"
)
#if(IWYU_COMMAND)
#    set_property(TARGET basic_game 
#        PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${IWYU_COMMAND}
#    )
#endif()


# ===================== TARGET: Python extension with SWIG =====================
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

    SET_SOURCE_FILES_PROPERTIES(include/fighter.i PROPERTIES CPLUSPLUS ON)
    SWIG_ADD_LIBRARY(basic_game_swig 
        LANGUAGE python 
        SOURCES include/fighter.i src/fighter.cpp
    )
    SWIG_LINK_LIBRARIES(basic_game_swig ${PYTHON_LIBRARIES})

    # deactivate warnings in compilation of python extensions
    get_property(compile_flags TARGET basic_game_swig PROPERTY COMPILE_OPTIONS)
    list(REMOVE_ITEM compile_flags 
        ${COMPILER_WARNING_BASIC} 
        ${COMPILER_WARNING_EXTENDED}
        ${BUILTIN_ANALYZER_OPTION}
        ${ALL_SANITIZERS}
    )
    set_target_properties( 
        basic_game_swig PROPERTIES COMPILE_OPTIONS "${compile_flags}"
    )
else()
    message(WARNING 
        "SWIG and/or Python not found, unable to generate python interface")
endif()


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
target_include_directories(runTests PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include ${GTEST_INCLUDE_DIRS}
)
target_link_libraries(runTests 
    ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} 
    ${THREADS_LINKER_FLAG}
)
add_test(NAME "Complete_tests" COMMAND runTests ARGS --gtest_color=yes)

#if(IWYU_COMMAND)
#    set_property(TARGET runTests 
#        PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${IWYU_COMMAND}
#    )
#endif()


# =========================== TARGET: Coverage test ============================
LIST(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake_modules")

if ((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") AND (CMAKE_BUILD_TYPE STREQUAL Debug))
    if(NOT CMAKE_RC_COMPILER_ID)
        set(CMAKE_RC_COMPILER_ID "GNU")
    endif()

    set(COVERAGE_FOLDER  ${PROJECT_BINARY_DIR}/analysis/coverage)
    file(MAKE_DIRECTORY ${COVERAGE_FOLDER})

    set(COVERAGE_EXTERNAL_HEADERS
        # google test headers:
        *gtest*
        # system headers:
        *mingw64*
        *c++*
    )
    set(COVERAGE_EXCLUDES
        # project files:
        ${PROJECT_SOURCE_DIR}/include/fighter.i
        ${PROJECT_SOURCE_DIR}/src/main.cpp
        # external stuff
        ${COVERAGE_EXTERNAL_HEADERS}
    )
    #set(CODE_COVERAGE_VERBOSE TRUE CACHE STRING "Do we want verbose coverage check?")
    include(CodeCoverage)
    append_coverage_compiler_flags_to_target(runTests)
    target_link_options(runTests PRIVATE --coverage -lgcov)

    #if(NOT MINGW) # gcovr(frontend for gcov) is not available under MINGW
    #    setup_target_for_coverage_gcovr_html(
    #        NAME runTestsCoverageHtml
    #        EXECUTABLE runTests
    #        DEPENDENCIES runTests
    #        BASE_DIRECTORY ${PROJECT_SOURCE_DIR}
    #    )
    #    setup_target_for_coverage_gcovr_xml(
    #        NAME runTestsCoverageXml
    #        EXECUTABLE runTests
    #        DEPENDENCIES runTests
    #        BASE_DIRECTORY ${PROJECT_SOURCE_DIR}
    #    )
    #endif()

    setup_target_for_coverage_lcov(
        NAME runTestsCoverageLcov
        EXECUTABLE runTests
        DEPENDENCIES runTests
        BASE_DIRECTORY ${PROJECT_SOURCE_DIR}
        #EXCLUDE # files to be excluded by lcov
    )
endif()


# ============================= TARGET: docs ===================================
find_package(Doxygen)

if(DOXYGEN_FOUND)
    set(DOXYGEN_GENERATE_HTML  YES)
    set(DOXYGEN_GENERATE_LATEX NO)
    set(DOXYGEN_GENERATE_MAN   NO)
    set(DOXYGEN_PROJECT_NAME  ${CMAKE_PROJECT_NAME})
    set(DOXYGEN_EXCLUDE_PATTERNS 
        ${PROJECT_BINARY_DIR} 
        ${PROJECT_SOURCE_DIR}/cmake_modules
    )
    doxygen_add_docs(
        docs
        ${PROJECT_SOURCE_DIR}
        COMMENT "Generating Doxygen documentation"
    )
else()
    message(WARNING "Doxygen not found, unable to generate documentation")
endif()


# ========================== TARGET: distclean =================================
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
