# CMake generated Testfile for 
# Source directory: C:/Users/ASUS/Desktop/Augemnted-ARBAB/40231027/AUG_ARBAB/kir/ARBAB/include/cJSON
# Build directory: C:/Users/ASUS/Desktop/Augemnted-ARBAB/40231027/AUG_ARBAB/kir/ARBAB/build/include/cJSON
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if(CTEST_CONFIGURATION_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(cJSON_test "C:/Users/ASUS/Desktop/Augemnted-ARBAB/40231027/AUG_ARBAB/kir/ARBAB/build/include/cJSON/cJSON_test")
  set_tests_properties(cJSON_test PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/ASUS/Desktop/Augemnted-ARBAB/40231027/AUG_ARBAB/kir/ARBAB/include/cJSON/CMakeLists.txt;248;add_test;C:/Users/ASUS/Desktop/Augemnted-ARBAB/40231027/AUG_ARBAB/kir/ARBAB/include/cJSON/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(cJSON_test "C:/Users/ASUS/Desktop/Augemnted-ARBAB/40231027/AUG_ARBAB/kir/ARBAB/build/include/cJSON/cJSON_test")
  set_tests_properties(cJSON_test PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/ASUS/Desktop/Augemnted-ARBAB/40231027/AUG_ARBAB/kir/ARBAB/include/cJSON/CMakeLists.txt;248;add_test;C:/Users/ASUS/Desktop/Augemnted-ARBAB/40231027/AUG_ARBAB/kir/ARBAB/include/cJSON/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test(cJSON_test "C:/Users/ASUS/Desktop/Augemnted-ARBAB/40231027/AUG_ARBAB/kir/ARBAB/build/include/cJSON/cJSON_test")
  set_tests_properties(cJSON_test PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/ASUS/Desktop/Augemnted-ARBAB/40231027/AUG_ARBAB/kir/ARBAB/include/cJSON/CMakeLists.txt;248;add_test;C:/Users/ASUS/Desktop/Augemnted-ARBAB/40231027/AUG_ARBAB/kir/ARBAB/include/cJSON/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test(cJSON_test "C:/Users/ASUS/Desktop/Augemnted-ARBAB/40231027/AUG_ARBAB/kir/ARBAB/build/include/cJSON/cJSON_test")
  set_tests_properties(cJSON_test PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/ASUS/Desktop/Augemnted-ARBAB/40231027/AUG_ARBAB/kir/ARBAB/include/cJSON/CMakeLists.txt;248;add_test;C:/Users/ASUS/Desktop/Augemnted-ARBAB/40231027/AUG_ARBAB/kir/ARBAB/include/cJSON/CMakeLists.txt;0;")
else()
  add_test(cJSON_test NOT_AVAILABLE)
endif()
subdirs("tests")
subdirs("fuzzing")
