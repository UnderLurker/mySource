# CMake generated Testfile for 
# Source directory: F:/Visual-Studio-practice/vscode/mySource
# Build directory: F:/Visual-Studio-practice/vscode/mySource/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(MAIN_TEST "F:/Visual-Studio-practice/vscode/mySource/build/MAIN.exe")
set_tests_properties(MAIN_TEST PROPERTIES  _BACKTRACE_TRIPLES "F:/Visual-Studio-practice/vscode/mySource/CMakeLists.txt;30;add_test;F:/Visual-Studio-practice/vscode/mySource/CMakeLists.txt;0;")
subdirs("utils/Matrix")
subdirs("utils/Log")
subdirs("utils/Reflex")
subdirs("utils/Util")
subdirs("utils/Singleton")
