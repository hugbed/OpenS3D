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
function(target_add_clang_tidy target-name include-dirs source-files)
    message(INFO ${source-files})

    string(REPLACE ";" ";-I" include-dirs "${include-dirs}")
    set(include-dirs-str "${include-dirs}")
    message(${include-dirs-str})

    # Adding clang-tidy target if executable is found
    if(CLANG_TIDY)
        add_custom_target(
            ${target-name}-clang-tidy
            COMMAND /usr/bin/clang-tidy
#            COMMAND /usr/bin/clang-tidy-4.0
            -checks='*,-misc-unused-parameters'
            -p=${CMAKE_BINARY_DIR}/
            ${source-files}
            --
            -std=c++14
            -I${include-dirs-str}
        )
    endif()
endfunction()
