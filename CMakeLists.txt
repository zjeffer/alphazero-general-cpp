cmake_minimum_required(VERSION 3.27 FATAL_ERROR)


######### MAIN PROJECT #########
project(alphazero VERSION 1.0)
enable_testing()

###### LibTorch ######
set(CMAKE_PREFIX_PATH /usr/include/libtorch/)
find_package(Torch REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${OpenCV_LIBS})
include_directories(${TORCH_INCLUDE_DIRS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")
set(CMAKE_CXX_STANDARD 23)

###### GoogleTest ######
add_subdirectory(vendor/googletest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

###### g3log ######
set(CMAKE_PREFIX_PATH /usr/local/lib/cmake/g3log)
find_package(g3log CONFIG REQUIRED)
include_directories(${G3LOG_INCLUDE_DIRS})

###### nlohmann_json ######
find_package(nlohmann_json REQUIRED)
# workaround for https://github.com/nlohmann/json/issues/4051
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DJSON_HAS_RANGES=0")

###### main files ######
file (GLOB PROJECT_SOURCES
    ${PROJECT_SOURCE_DIR}/src/Game.cpp
    ${PROJECT_SOURCE_DIR}/src/lib/Agent/*.cpp
    ${PROJECT_SOURCE_DIR}/src/lib/DataManager/*.cpp
    ${PROJECT_SOURCE_DIR}/src/lib/Configuration/*.cpp
    ${PROJECT_SOURCE_DIR}/src/lib/Environment/*.cpp
    ${PROJECT_SOURCE_DIR}/src/lib/Logging/*.cpp
    ${PROJECT_SOURCE_DIR}/src/lib/MCTS/*.cpp
    ${PROJECT_SOURCE_DIR}/src/lib/NeuralNetwork/*.cpp
    ${PROJECT_SOURCE_DIR}/src/lib/Utilities/*.cpp
)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/src/main.cpp ${PROJECT_SOURCES})

# link libraries (torch, g3log)
target_link_libraries(${PROJECT_NAME} ${TORCH_LIBRARIES} g3log)

# set up testing
file (GLOB TEST_SOURCES
    ${PROJECT_SOURCE_DIR}/test/main.cpp
    ${PROJECT_SOURCE_DIR}/test/Tests/*.cpp
)
add_executable(${PROJECT_NAME}_test ${PROJECT_SOURCES} ${TEST_SOURCES})
target_link_libraries(${PROJECT_NAME}_test gtest_main gmock ${TORCH_LIBRARIES} g3log)

# python
find_package(Python REQUIRED COMPONENTS Development)
target_include_directories(${PROJECT_NAME} PRIVATE ${Python_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${Python_LIBRARIES})

# set c++ standard
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 23)
set_target_properties(${PROJECT_NAME}_test PROPERTIES CXX_STANDARD 23)