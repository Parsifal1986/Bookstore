CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

project(bookstore)

set(CMAKE_CXX_STANDARD 17)

aux_source_directory(./main DIR_SRC)
add_executable(code ${DIR_SRC})

