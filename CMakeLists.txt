cmake_minimum_required(VERSION 3.13)

project(LocationAwareFuzzer)


add_executable(main main.cpp)
target_compile_options(main PUBLIC
  -Werror
  -Wall
  -Wextra
  -Wconversion
  -Wswitch-default
  -Wdisabled-optimization
  -Wformat
  -Winit-self
  -Woverloaded-virtual
  -Wfloat-equal
  -Wreturn-type
)
target_compile_features(main PUBLIC cxx_std_20)
