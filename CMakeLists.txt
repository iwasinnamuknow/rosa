cmake_minimum_required(VERSION 3.16)

project(rosa)

file(GLOB_RECURSE SOURCES "src/*.cpp")

if (_WIN32)                                                                                          
  add_executable(rosa_exe WIN32 ${SOURCES})                                                          
else (_WIN32)                                                                                        
  add_executable(rosa_exe ${SOURCES})                                                                
endif (_WIN32)

set_property(TARGET rosa_exe PROPERTY OUTPUT_NAME rosa)

target_compile_features(rosa_exe PRIVATE cxx_std_20)
target_include_directories(rosa_exe PRIVATE
    include/
    vendor/
)