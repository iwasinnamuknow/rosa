project(rosa_examples)

# Per-example targets
if (WIN32)
    add_executable(simple_image WIN32 simple_image.cpp) 
else (WIN32)
    add_executable(simple_image simple_image.cpp) 
endif (WIN32)
target_compile_features(simple_image PRIVATE cxx_std_20)
target_include_directories(simple_image PRIVATE ../include/)
target_link_libraries(simple_image PRIVATE rosa)

if (WIN32)
    add_executable(countdown WIN32 countdown.cpp) 
else (WIN32)
    add_executable(countdown countdown.cpp) 
endif (WIN32)
target_compile_features(countdown PRIVATE cxx_std_20)
target_include_directories(countdown PRIVATE ../include/)
target_link_libraries(countdown PRIVATE rosa)

if (WIN32)
    add_executable(serialise WIN32 serialise.cpp) 
else (WIN32)
    add_executable(serialise serialise.cpp) 
endif (WIN32)
target_compile_features(serialise PRIVATE cxx_std_20)
target_include_directories(serialise PRIVATE ../include/)
target_link_libraries(serialise PRIVATE rosa)

if (WIN32)
    add_executable(deserialise WIN32 deserialise.cpp) 
else (WIN32)
    add_executable(deserialise deserialise.cpp) 
endif (WIN32)
target_compile_features(deserialise PRIVATE cxx_std_20)
target_include_directories(deserialise PRIVATE ../include/)
target_link_libraries(deserialise PRIVATE rosa)

if (WIN32)
    add_executable(generate_uuids WIN32 generate_uuids.cpp) 
else (WIN32)
    add_executable(generate_uuids generate_uuids.cpp) 
endif (WIN32)
target_compile_features(generate_uuids PRIVATE cxx_std_20)
target_include_directories(generate_uuids PRIVATE ../include/)
target_link_libraries(generate_uuids PRIVATE rosa)

if (WIN32)
    add_executable(camera_movement WIN32 camera_movement.cpp) 
else (WIN32)
    add_executable(camera_movement camera_movement.cpp) 
endif (WIN32)
target_compile_features(camera_movement PRIVATE cxx_std_20)
target_include_directories(camera_movement PRIVATE ../include/)
target_link_libraries(camera_movement PRIVATE rosa)

if (WIN32)
    add_executable(text_rendering WIN32 text_rendering.cpp)
else (WIN32)
    add_executable(text_rendering text_rendering.cpp)
endif (WIN32)
target_compile_features(text_rendering PRIVATE cxx_std_20)
target_include_directories(text_rendering PRIVATE ../include/)
target_link_libraries(text_rendering PRIVATE rosa)

if (WIN32)
    add_executable(reaction_tester WIN32 reaction_tester/reaction_tester.cpp reaction_tester/RosaTalker.cpp app.rc)
else (WIN32)
    add_executable(reaction_tester reaction_tester/reaction_tester.cpp reaction_tester/RosaTalker.cpp)
endif (WIN32)
add_dependencies(reaction_tester pack_assets)
target_compile_features(reaction_tester PRIVATE cxx_std_20)
target_include_directories(reaction_tester PRIVATE ../include/)
target_link_libraries(reaction_tester PRIVATE rosa)

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

