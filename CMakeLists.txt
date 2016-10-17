cmake_minimum_required(VERSION 3.6)
project(ifj)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c99 -pedantic -Wextra -Wall ")

set(SOURCE_FILES IFJ16Lib/ifj16.c IFJ16Lib/ifj16.h IFJ16Lib/ifj16-test.c LexicalAnalyzer/LexicalAnalyzer.c LexicalAnalyzer/LexicalAnalyzer-test.c LexicalAnalyzer/LexicalAnalyzer.h )
add_executable(ifj ${SOURCE_FILES})