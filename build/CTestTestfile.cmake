# CMake generated Testfile for 
# Source directory: F:/C++/mySource
# Build directory: F:/C++/mySource/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(Reflex_Test "ReflexTest")
set_tests_properties(Reflex_Test PROPERTIES  _BACKTRACE_TRIPLES "F:/C++/mySource/CMakeLists.txt;43;add_test;F:/C++/mySource/CMakeLists.txt;0;")
add_test(Json_Test "JsonTest")
set_tests_properties(Json_Test PROPERTIES  _BACKTRACE_TRIPLES "F:/C++/mySource/CMakeLists.txt;44;add_test;F:/C++/mySource/CMakeLists.txt;0;")
add_test(Server_Test "ServerTest")
set_tests_properties(Server_Test PROPERTIES  _BACKTRACE_TRIPLES "F:/C++/mySource/CMakeLists.txt;45;add_test;F:/C++/mySource/CMakeLists.txt;0;")
add_test(Matrix_Test "MatrixTest")
set_tests_properties(Matrix_Test PROPERTIES  _BACKTRACE_TRIPLES "F:/C++/mySource/CMakeLists.txt;46;add_test;F:/C++/mySource/CMakeLists.txt;0;")
add_test(Image_Test "ImageTest")
set_tests_properties(Image_Test PROPERTIES  _BACKTRACE_TRIPLES "F:/C++/mySource/CMakeLists.txt;47;add_test;F:/C++/mySource/CMakeLists.txt;0;")
subdirs("utils/Matrix")
subdirs("utils/Log")
subdirs("utils/Reflex")
subdirs("utils/JsonSerialize")
subdirs("utils/Util")
subdirs("utils/Singleton")
subdirs("utils/Server")
subdirs("utils/Image")
