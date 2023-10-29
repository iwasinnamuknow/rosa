project(rosa_testbed)

# Per-example targets
add_executable(simple_image simple_image.cpp) 
target_compile_features(simple_image PRIVATE cxx_std_20)
target_include_directories(simple_image PRIVATE ../include/)
target_link_libraries(simple_image PRIVATE rosa)


# Asset packaging
file(GLOB_RECURSE ASSETS "assets/*")

ADD_CUSTOM_COMMAND(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/base.pak
    COMMAND ${CMAKE_COMMAND} -E tar cvf ${CMAKE_CURRENT_BINARY_DIR}/base.pak --format=zip -- .
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/assets/
    DEPENDS ${ASSETS}
)
ADD_CUSTOM_TARGET(
    pack_assets ALL
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/base.pak
)
ADD_DEPENDENCIES(pack_assets simple_image)

