cmake_minimum_required(VERSION 3.21)

project(My_Language LANGUAGES CXX)

option(MY_LANGUAGE_ENABLE_WARNINGS "Enable compiler warnings" ON)

add_executable(read
    Read.cpp
    Syntax_Analis.cpp
    DOT.cpp
    Main.cpp
    Simply.cpp
    Back_end.cpp
    Build_Table_Name_Func.cpp
)

set(PROCESSOR_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Processor-reference")

add_executable(processor_assembler
    "${PROCESSOR_SOURCE_DIR}/Compilator.cpp"
)
target_include_directories(processor_assembler PRIVATE "${PROCESSOR_SOURCE_DIR}")
target_compile_features(processor_assembler PRIVATE cxx_std_11)

add_executable(processor_emulator
    "${PROCESSOR_SOURCE_DIR}/Processor.cpp"
)
target_include_directories(processor_emulator PRIVATE "${PROCESSOR_SOURCE_DIR}")
target_compile_features(processor_emulator PRIVATE cxx_std_11)

set(LANGUAGE_PROGRAM "${CMAKE_CURRENT_SOURCE_DIR}/recursion_example.txt" CACHE FILEPATH
    "Source program in My_Language to compile and execute on Processor")

target_compile_features(read PRIVATE cxx_std_11)
set_target_properties(read PROPERTIES CXX_EXTENSIONS OFF)

if(MY_LANGUAGE_ENABLE_WARNINGS)
    if(MSVC)
        target_compile_options(read PRIVATE /W4)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(read PRIVATE
            -Wall
            -Wextra
            -Wshadow
            -Wconversion
            -Wformat=2
            -Werror=vla
        )
    endif()
endif()

find_program(GRAPHVIZ_DOT_EXECUTABLE NAMES dot)
if(NOT GRAPHVIZ_DOT_EXECUTABLE)
    # Leave the command name as a fallback so installing Graphviz later works
    # without changing the target definition.
    set(GRAPHVIZ_DOT_EXECUTABLE dot)
    message(STATUS "Graphviz not found; install it before building the example target")
endif()

add_custom_target(example
    COMMAND $<TARGET_FILE:read> "${CMAKE_CURRENT_SOURCE_DIR}/example.txt"
    COMMAND "${GRAPHVIZ_DOT_EXECUTABLE}" -Tpng tree.dot -o tree.png
    DEPENDS read
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    COMMENT "Compiling example and rendering build/tree.png with Graphviz"
    VERBATIM
)

add_custom_target(run_program
    COMMAND $<TARGET_FILE:read> "${LANGUAGE_PROGRAM}"
    COMMAND "${CMAKE_COMMAND}" -E copy
            "${CMAKE_CURRENT_BINARY_DIR}/output.asm"
            "${CMAKE_CURRENT_BINARY_DIR}/processor_input.asm"
    COMMAND $<TARGET_FILE:processor_assembler>
            "${CMAKE_CURRENT_BINARY_DIR}/processor_input.asm"
    COMMAND $<TARGET_FILE:processor_emulator>
    DEPENDS read processor_assembler processor_emulator
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    COMMENT "Compiling My_Language program, assembling it, and running it on Processor"
    VERBATIM
)
