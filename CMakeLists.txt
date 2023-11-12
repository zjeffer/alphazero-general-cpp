cmake_minimum_required(VERSION 3.23 FATAL_ERROR)


######### MAIN PROJECT #########
project(alphazero VERSION 1.0)

###### LibTorch ######
set(CMAKE_PREFIX_PATH /usr/include/libtorch/)
find_package(Torch REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${OpenCV_LIBS})
include_directories(${TORCH_INCLUDE_DIRS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

###### g3log ######
set(CMAKE_PREFIX_PATH /usr/local/lib/cmake/g3log)
find_package(g3log CONFIG REQUIRED)
include_directories(${G3LOG_INCLUDE_DIRS})

###### main files ######
file (GLOB PROJECT_SOURCES 
    ${PROJECT_SOURCE_DIR}/src/*.cpp
    ${PROJECT_SOURCE_DIR}/lib/*.cpp
)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

# link libraries (torch, g3log)
target_link_libraries(${PROJECT_NAME} ${TORCH_LIBRARIES} g3log)

# python
find_package(Python REQUIRED COMPONENTS Development)
target_include_directories(${PROJECT_NAME} PRIVATE ${Python_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${Python_LIBRARIES})

# set warnings
target_compile_options(${PROJECT_NAME} PRIVATE 
    -Wall -pedantic 
    -pedantic-errors -Wextra -Wcast-align
    -Wchar-subscripts  -Wcomment
    -Wdisabled-optimization
    -Werror -Wfloat-equal  -Wformat  -Wformat=2
    -Wformat-nonliteral -Wformat-security 
    -Wformat-y2k
    -Wimport  -Winit-self  -Winline
    -Winvalid-pch -Wlong-long
    -Wmissing-field-initializers -Wmissing-format-attribute  
    -Wmissing-noreturn
    -Wpacked -Wparentheses  -Wpointer-arith
    -Wredundant-decls -Wreturn-type
    -Wsequence-point -Wsign-compare  -Wstack-protector
    -Wstrict-aliasing -Wstrict-aliasing=2 -Wswitch -Wtrigraphs  -Wuninitialized
    -Wunknown-pragmas  -Wunreachable-code -Wunused
    -Wunused-function  -Wunused-label  -Wunused-parameter
    -Wunused-value  -Wunused-variable  -Wvariadic-macros
    -Wshadow
    -Wvolatile-register-var  -Wwrite-strings)

# set c++ standard
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 23)