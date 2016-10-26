# Copyright © 2013 Canonical Ltd.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 3 as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# Authored by: Thomas Voss <thomas.voss@canonical.com>
include_directories (
  ${PROCESS_CPP_INCLUDE_DIRS}
)

qt5_wrap_cpp(MOCS test_harness.h)

add_executable(
  app_hub_communication_default_source
  app_hub_communication_default_source.cpp
  ${MOCS}
)

add_executable(
  app_hub_communication_known_sources
  app_hub_communication_known_sources.cpp
  ${MOCS}
)

add_executable(
  app_hub_communication_stores
  app_hub_communication_stores.cpp
  ${MOCS}
)

add_executable(
  app_hub_communication_transfer
  app_hub_communication_transfer.cpp
  ${MOCS}
)

add_executable(
  app_hub_communication_paste
  app_hub_communication_paste.cpp
  ${MOCS}
)

add_executable(
  app_hub_communication_handler
  app_hub_communication_handler.cpp
  ${MOCS}
  transfer_files
)

add_executable(
  test_utils
  test_utils.cpp
)

add_executable(
  test_types
  test_types.cpp
)

add_executable(
  test_hook
  test_hook.cpp
  ${MOCS}
  ${CMAKE_SOURCE_DIR}/src/com/ubuntu/content/service/hook.cpp
  ${CMAKE_SOURCE_DIR}/src/com/ubuntu/content/service/registry.cpp
  good.json
  bad.json
  source_all.json
)

add_executable(
  mimedata_test
  mimedata_test.cpp
  ${MOCS}
)

qt5_use_modules(app_hub_communication_default_source Core Gui DBus Test)
qt5_use_modules(app_hub_communication_known_sources Core Gui DBus Test)
qt5_use_modules(app_hub_communication_stores Core Gui DBus Test)
qt5_use_modules(app_hub_communication_transfer Core Gui DBus Test)
qt5_use_modules(app_hub_communication_paste Core Gui DBus Test)
qt5_use_modules(app_hub_communication_handler Core Gui DBus Test)
qt5_use_modules(test_utils Core Test)
qt5_use_modules(test_hook Core Gui DBus Test)
qt5_use_modules(mimedata_test Core Test)

target_link_libraries(app_hub_communication_stores content-hub gmock gtest gtest_main ${PROCESS_CPP_LIBRARIES})
target_link_libraries(app_hub_communication_default_source content-hub gmock gtest gtest_main ${PROCESS_CPP_LIBRARIES})
target_link_libraries(app_hub_communication_known_sources content-hub gmock gtest gtest_main ${PROCESS_CPP_LIBRARIES})
target_link_libraries(app_hub_communication_stores content-hub gmock gtest gtest_main ${PROCESS_CPP_LIBRARIES})
target_link_libraries(app_hub_communication_transfer content-hub gmock gtest gtest_main ${PROCESS_CPP_LIBRARIES})
target_link_libraries(app_hub_communication_paste content-hub gmock gtest gtest_main ${PROCESS_CPP_LIBRARIES})
target_link_libraries(app_hub_communication_handler content-hub gmock gtest gtest_main ${PROCESS_CPP_LIBRARIES})
target_link_libraries(test_utils content-hub gmock gtest gtest_main)
target_link_libraries(test_types content-hub gmock gtest gtest_main)
target_link_libraries(test_hook content-hub gmock gtest gtest_main ${GSETTINGS_LDFLAGS})
target_link_libraries(mimedata_test content-hub gmock gtest gtest_main)

set(
  RUNNER
  dbus-test-runner -m 120 --task
)

add_test(NAME app_hub_communication_default_source COMMAND ${RUNNER} ${CMAKE_CURRENT_BINARY_DIR}/app_hub_communication_default_source)
add_test(NAME app_hub_communication_known_sources COMMAND ${RUNNER} ${CMAKE_CURRENT_BINARY_DIR}/app_hub_communication_known_sources)
add_test(NAME app_hub_communication_stores COMMAND ${RUNNER} ${CMAKE_CURRENT_BINARY_DIR}/app_hub_communication_stores)
add_test(NAME app_hub_communication_transfer COMMAND ${RUNNER} ${CMAKE_CURRENT_BINARY_DIR}/app_hub_communication_transfer)
add_test(NAME app_hub_communication_paste COMMAND ${RUNNER} ${CMAKE_CURRENT_BINARY_DIR}/app_hub_communication_paste)
add_test(NAME app_hub_communication_handler COMMAND ${RUNNER} ${CMAKE_CURRENT_BINARY_DIR}/app_hub_communication_handler)
add_test(NAME test_utils COMMAND ${RUNNER} ${CMAKE_CURRENT_BINARY_DIR}/test_utils)
add_test(NAME test_types COMMAND ${RUNNER} ${CMAKE_CURRENT_BINARY_DIR}/test_types)
add_test(NAME test_hook COMMAND ${RUNNER} ${CMAKE_CURRENT_BINARY_DIR}/test_hook)
add_test(NAME mimedata_test COMMAND mimedata_test)
set_tests_properties(
  app_hub_communication_default_source
  app_hub_communication_known_sources
  app_hub_communication_stores
  app_hub_communication_transfer
  app_hub_communication_paste
  app_hub_communication_handler
  test_utils
  test_types
  test_hook
  PROPERTIES RUN_SERIAL TRUE
)

set_tests_properties(app_hub_communication_paste
    PROPERTIES ENVIRONMENT "CONTENT_HUB_TESTING=1")

set_target_properties(
  test_hook
  PROPERTIES
  AUTOMOC TRUE
)

file(COPY good.json bad.json source_all.json DESTINATION .)

add_custom_command(
  OUTPUT transfer_files
  COMMAND cmake -E touch file1 file2 file3
)
