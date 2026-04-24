cmake_minimum_required(VERSION 4.2)
project(tui)

# Specify the C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ╭────────────────────────────────────────────────────────────────────╮
# │ Functions                                                          │
# ╰────────────────────────────────────────────────────────────────────╯

function(set_warnings_as_errors target)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR
        CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_compile_options(${target} PRIVATE -Wall -Wextra -Werror)
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(${target} PRIVATE /W4 /WX)
    endif()
endfunction()

# ╭────────────────────────────────────────────────────────────────────╮
# │ TUI Library                                                        │
# ╰────────────────────────────────────────────────────────────────────╯

file(GLOB TUI_SRC "src/*.c")
file(GLOB TUI_HEADERS "include/*.h" "src/*.h")
add_library(tui STATIC ${TUI_SRC} ${TUI_HEADERS})
set_warnings_as_errors(tui)
target_include_directories(tui PUBLIC "include")
target_include_directories(tui PRIVATE "src")

# ╭────────────────────────────────────────────────────────────────────╮
# │ Examples                                                           │
# ╰────────────────────────────────────────────────────────────────────╯

file(GLOB EXAMPLES_SRC "examples/*.c")
add_executable(example ${EXAMPLES_SRC})
set_warnings_as_errors(example)
target_link_libraries(example tui)

