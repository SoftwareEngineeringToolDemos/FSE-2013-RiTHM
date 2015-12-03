link_directories("${PATH_TO_LLVM_BUILD}/lib" "-L../../../../../../libconfig-1.4.8/lib/.libs")

add_clang_executable(globalize
  Globalize.cpp
  )

target_link_libraries(globalize
  clangEdit clangTooling clangBasic clangAST clangASTMatchers config++)

if (LLVM_COMPILER_IS_GCC_COMPATIBLE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I../../../../../../libconfig-1.4.8/lib -fexceptions")
endif()
