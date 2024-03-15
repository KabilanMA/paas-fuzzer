cmake_minimum_required(VERSION 3.10)
project(pass-fuzzer)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the executable
add_executable(ConfigGenerator 
                driver.cpp
                ConfigGenerator.cpp
                JsonFileHandler.cpp
                Patch.cpp
                Evaluator.cpp
                ../scheduler/ClusterAccess.cpp

                ../QueryGeneration/QueryExecutor.cpp
                ../QueryGeneration/DatabaseHandler.cpp
                ../QueryGeneration/LogFileHandler.cpp
                ../QueryGeneration/QueryFileHandler.cpp
                ../QueryGeneration/QueryGenerator.cpp
                )

# Specify include directories
target_include_directories(ConfigGenerator 
    PRIVATE 
    "/home/tharsha/Desktop/FYP/c/kubernetes" 
    "/home/tharsha/Desktop/FYP/paas-fuzzer/include/driver"
    "/home/tharsha/Desktop/FYP/paas-fuzzer/include/scheduler"
    "/home/tharsha/Desktop/FYP/paas-fuzzer/vcpkg/installed/x64-linux/include"
    "/home/tharsha/Desktop/FYP/paas-fuzzer/include/QueryGeneration"
)

# Specify library directories
link_directories(
    "/home/tharsha/Desktop/FYP/c/kubernetes/build/**"
    "/home/tharsha/Desktop/FYP/paas-fuzzer/vcpkg/installed/x64-linux/lib"
)

# Link
target_link_libraries(ConfigGenerator 
    PRIVATE
    yaml
    websockets
    kubernetes
    /home/tharsha/Desktop/FYP/paas-fuzzer/vcpkg/installed/x64-linux/lib/libjsoncpp.a
    /usr/lib/x86_64-linux-gnu/libmysqlcppconn.so
)

# Set the directory containing your text files
set(TEXT_FILES_DIR ${CMAKE_SOURCE_DIR}/../QueryGeneration/output)

configure_file(${CMAKE_SOURCE_DIR}/../QueryGeneration/credentials_local.txt ${CMAKE_CURRENT_BINARY_DIR}/credentials_local.txt COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/../QueryGeneration/credentials_cluster.txt ${CMAKE_CURRENT_BINARY_DIR}/credentials_cluster.txt COPYONLY)

# Add a custom command to copy files during the build step
# add_custom_command(
#     TARGET ConfigGenerator POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_directory
#         ${TEXT_FILES_DIR}
#         $<TARGET_FILE_DIR:ConfigGenerator>/output
# )

file(COPY ${TEXT_FILES_DIR} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

