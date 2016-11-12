cmake_minimum_required(VERSION 3.6)
project(ifj)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c99")

set(SOURCE_FILES
        IFJ16Lib/ifj16.c
        IAL/ial.c
        LexicalAnalyzer/LexicalAnalyzer.c LexicalAnalyzer/LexicalAnalyzer-test.c
        SyntacticalAnalyzer/SyntacticalAnalyzer.c SyntacticalAnalyzer/SyntacticalAnalyzer.h
        main.c
)

add_executable(ifj ${SOURCE_FILES})