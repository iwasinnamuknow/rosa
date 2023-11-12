cmake_minimum_required(VERSION 3.25)                                                                 
                                                                                                        
include(FetchContent)                                                                                   
project(rosa_tests)                                                                                  
                                                                                                     
FetchContent_Declare(snitch                                                                          
                     GIT_REPOSITORY https://github.com/cschreib/snitch.git                           
                     GIT_TAG        v1.2.0) # update version number as needed                        
FetchContent_MakeAvailable(snitch)                                                                   
                                                                                                     
if (NOT CMAKE_BUILD_TYPE)                                                                            
  set (CMAKE_BUILD_TYPE Release)                                                                     
endif (NOT CMAKE_BUILD_TYPE)                                                                         
                                                                                                     
file(GLOB SOURCES "*.cpp")                                                                                                                                               
                                                                                                     
add_executable(rosa_tests ${SOURCES})                                                                
target_include_directories(rosa_tests PRIVATE "../include/")                                            
target_compile_features(rosa_tests PRIVATE cxx_std_20)                                               
                                                                                                     
if (CMAKE_BUILD_TYPE MATCHES Debug)                                                                  
  target_compile_options(rosa_tests PRIVATE -Wall -Wextra -pedantic)                                 
else()                                                                                               
  target_compile_options(rosa_tests PRIVATE -O2)                                                     
endif (CMAKE_BUILD_TYPE MATCHES Debug)                                                               
                                                                                                     
target_link_libraries(rosa_tests PRIVATE snitch::snitch rosa)