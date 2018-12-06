enable_language(ASM_NASM)
if(CMAKE_ASM_NASM_COMPILER_LOADED)
    set(CAN_USE_ASSEMBLER TRUE)
    file(GLOB_RECURSE ASM_FILES "*.asm")
endif(CMAKE_ASM_NASM_COMPILER_LOADED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_executable(register-rename register-rename.cpp ${ASM_FILES})
target_link_libraries(register-rename benchmark)
add_dependencies(register-rename gbenchmark)
