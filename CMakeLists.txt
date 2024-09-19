cmake_minimum_required (VERSION 3.1)

project (mph_ref VERSION 1.0)

set(TARGET_NAME mph_ref)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}") # puts libriries and bin at the same dir
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

set(BUILD_SHARED_LIBS OFF)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")

message(${CMAKE_CURRENT_SOURCE_DIR})

# main programm
add_executable( ${TARGET_NAME}  main.cpp )

FILE(GLOB APP_SRC 
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

target_sources( ${TARGET_NAME} PUBLIC ${APP_SRC})

target_include_directories( ${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )


string(TIMESTAMP _output "%Y-%m-%d %H:%M:%S")


target_compile_options(
		${TARGET_NAME} PUBLIC
		-Wall
		-Wextra
		-pedantic
		#-Werror
		-std=c++17
)


#install files
install(TARGETS  ${TARGET_NAME} DESTINATION .) # use cmake --install build_dir --prefix install_dir
# cmake --install prj/cmake-build-debug --prefix install
