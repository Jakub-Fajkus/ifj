cmake_minimum_required(VERSION 3.6)
project(ifj)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c99")

set(SOURCE_FILES
        ifj16.c
        ial.c
        LexicalAnalyzer.c LexicalAnalyzer-test.c
        SyntacticalAnalyzer.c SyntacticalAnalyzer.h
        main.c
        DoubleLinkedList.h DoubleLinkedList.c
        Stack.c Stack.h
        SymbolTable.c SymbolTable.h
        BasicStructures.h LexicalAnalyzerStructures.h Debug.c Debug.h)

add_executable(ifj ${SOURCE_FILES})