set(LLVM_LINK_COMPONENTS
  ${LLVM_TARGETS_TO_BUILD}
  asmparser
  support
  mc
  )

add_clang_executable(auto-instrument
  AutoInstrumenter.cpp
  InsertBracesASTVisitor.cpp
  InsertBracesASTConsumer.cpp
  InstrumentASTVisitor.cpp
  InstrumentASTConsumer.cpp
  InsertBracesFrontendAction.cpp
  InstrumentFrontendAction.cpp
  InstrumentationMap.cpp
  )

target_link_libraries(auto-instrument
  clangTooling
  clangBasic
  )

install(TARGETS auto-instrument
  RUNTIME DESTINATION bin)
