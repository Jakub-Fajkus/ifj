cmake_minimum_required(VERSION 3.6)
project(ifj)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c99 -g -DDEBUG")

file(GLOB ifj_SRC "*.h" debug.c doublelinkedlist.c expressionanalizer.c ifj16.c instruction.c interpret.c lexicalanalyzer.c main.c semanticalanalyzer.c stack.c symboltable.c syntacticalanalyzer.c)

add_executable(ifj ${ifj_SRC})