# CMakeLists head

target_sources(vsf INTERFACE
    retarget_io.c
    signal.c
)

add_subdirectory(RISCV)
