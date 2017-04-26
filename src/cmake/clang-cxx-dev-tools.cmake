# Additional targets to perform clang-format/clang-tidy
# Get all project files

set(SRC_DIRS "apps;core")
set(ALL_CXX_SOURCE_FILES "")
foreach("dir" ${SRC_DIRS})
    file(GLOB_RECURSE
        ALL_CXX_SOURCE_FILES
        ${ALL_CXX_SOURCE_FILES} ${dir}/*.[chi]pp ${dir}/*.[chi]xx ${dir}/*.cc ${dir}/*.hh ${dir}/*.ii ${dir}/*.[CHI] ${dir}/*.[chi]
        )
endforeach()

# Adding clang-format target if executable is found
find_program(CLANG_FORMAT "clang-format")
if(CLANG_FORMAT)
    add_custom_target(
        clang-format
        COMMAND /usr/bin/clang-format
        -i
        -style=file
        ${ALL_CXX_SOURCE_FILES}
    )
endif()

## Why some checks are removed
#
# -clang-analyzer-alpha*        : until more stable
# -google-runtime-references*   : there are a lot of output by reference that will remain this way for now
# -modernize-pass-by-value*     : false positives for trivially copiable types.
#                                 Can be re-enabled to look for true positives

find_program(CLANG_TIDY "clang-tidy")
# Adding clang-tidy target if executable is found
if(CLANG_TIDY)
    add_custom_target(
        clang-tidy
        COMMAND ${CMAKE_SOURCE_DIR}/scripts/run-clang-tidy.py
#        -clang-tidy-binary='/home/jon/Downloads/clang-4.0/bin/clang-tidy'
        -checks='*,-clang-analyzer-alpha*,-google-runtime-references*,-modernize-pass-by-value*'
        -header-filter='\(${CMAKE_SOURCE_DIR}/core/.*|${CMAKE_SOURCE_DIR}/apps/.*\)'
        -file-filter='\(libs|tests_*\)'
        -p='${CMAKE_BINARY_DIR}'
        -j='4'
    )
endif()
