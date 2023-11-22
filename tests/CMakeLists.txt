cmake_minimum_required(VERSION 3.25)                                                                 
                                                                                                        
include(FetchContent)


FetchContent_Declare(snitch                                                                          
    GIT_REPOSITORY https://github.com/cschreib/snitch.git                           
    GIT_TAG        v1.2.0) # update version number as needed                        
FetchContent_MakeAvailable(snitch)                                                                   
                                                                                                     
if (NOT CMAKE_BUILD_TYPE)                                                                            
  set (CMAKE_BUILD_TYPE Release)                                                                     
endif (NOT CMAKE_BUILD_TYPE)                                                                         

set(TESTS 
  uuid
  resource-manager
  scene
  display_image
  lua_rotating_image
  lua_coloured_image
  msaa
)

foreach(TEST IN LISTS TESTS)
    project(test_${TEST})
    add_executable(${TEST} ${TEST}.cpp)                       
    set_target_properties(${TEST} PROPERTIES OUTPUT_NAME test_${TEST})
    target_include_directories(${TEST} PRIVATE "../include/")                                            
    target_compile_features(${TEST} PRIVATE cxx_std_20)                                               
    if (CMAKE_BUILD_TYPE MATCHES Debug)                                                                  
        target_compile_options(${TEST} PRIVATE -Wall -Wextra -pedantic)                                 
    else()                                                                                               
        target_compile_options(${TEST} PRIVATE -O2)                                                     
    endif (CMAKE_BUILD_TYPE MATCHES Debug)                                                               
    target_link_libraries(${TEST} PRIVATE snitch::snitch rosa)
endforeach()

# Copy reference images for gl tests
set(REFERENCE_IMAGES
${CMAKE_CURRENT_SOURCE_DIR}/references/images/display_image.png
)
add_custom_target(copy_refs ALL
  COMMAND cp -r ${CMAKE_CURRENT_SOURCE_DIR}/references/images/* ${CMAKE_CURRENT_BINARY_DIR}/references/
  DEPENDS ${REFERENCE_IMAGES}
)
ADD_DEPENDENCIES(copy_refs ${TESTS})

# copy pak files for resource tests
set(RESOURCE_PACKS
  ${CMAKE_CURRENT_SOURCE_DIR}/packs/emptydds.pak
  ${CMAKE_CURRENT_SOURCE_DIR}/packs/missingfile.pak
  ${CMAKE_CURRENT_SOURCE_DIR}/packs/nomanifest.pak
  ${CMAKE_CURRENT_SOURCE_DIR}/packs/notadds.pak
)
add_custom_target(copy_packs ALL
  COMMAND cp -r ${CMAKE_CURRENT_SOURCE_DIR}/packs ${CMAKE_CURRENT_BINARY_DIR}/
  DEPENDS ${RESOURCE_PACKS}
)
ADD_DEPENDENCIES(copy_packs ${TESTS})


file(GLOB_RECURSE ASSETS "references/assets/*")
ADD_CUSTOM_COMMAND(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/references/base.pak
    COMMAND ${CMAKE_COMMAND} -E tar cvf ${CMAKE_CURRENT_BINARY_DIR}/references/base.pak --format=zip -- .
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/references/assets/
    DEPENDS ${ASSETS}
)
ADD_CUSTOM_TARGET(
    pack_reference_assets ALL
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/references/base.pak
)
ADD_DEPENDENCIES(pack_reference_assets ${TESTS})