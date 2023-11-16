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


file(GLOB PACKS "*.pak")
add_custom_target(copy_packs ALL
  COMMAND cp ${PACKS} ${CMAKE_CURRENT_BINARY_DIR}/
)
ADD_DEPENDENCIES(copy_packs ${TESTS})