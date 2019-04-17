cmake_minimum_required(VERSION 3.12)
project(SyntaxAnalysis)

set(CMAKE_CXX_STANDARD 11)
#
add_executable(SyntaxAnalysis main.cpp  source/LL.cpp head/LL.h head/LR.h source/LR.cpp head/base.h source/base.cpp)