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

find_program(CLANG_TIDY "clang-tidy")
# Adding clang-tidy target if executable is found
if(CLANG_TIDY)
    add_custom_target(
        clang-tidy
        COMMAND ${CMAKE_SOURCE_DIR}/scripts/run-clang-tidy.py
        -checks='*,-clang-analyzer-alpha*'
        -header-filter='OpenS3D/src/.*'
        -p='${CMAKE_BINARY_DIR}'
        -j='4'
    )
endif()
