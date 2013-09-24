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

qt5_wrap_cpp(MOCS test_harness.h)

add_executable(
  app_hub_communication_default_peer
  app_hub_communication_default_peer.cpp
  ${MOCS}
)

add_executable(
  app_hub_communication_known_peers
  app_hub_communication_known_peers.cpp
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
  app_hub_communication_handler
  app_hub_communication_handler.cpp
  ${MOCS}
  transfer_files
)

add_executable(
  test_utils
  test_utils.cpp
)

qt5_use_modules(app_hub_communication_default_peer Core Test)
qt5_use_modules(app_hub_communication_known_peers Core Test)
qt5_use_modules(app_hub_communication_stores Core Test)
qt5_use_modules(app_hub_communication_transfer Core Test)
qt5_use_modules(app_hub_communication_handler Core Test)
qt5_use_modules(test_utils Core Test)
target_link_libraries(app_hub_communication_stores content-hub gmock gtest gtest_main)
target_link_libraries(app_hub_communication_default_peer content-hub gmock gtest gtest_main)
target_link_libraries(app_hub_communication_known_peers content-hub gmock gtest gtest_main)
target_link_libraries(app_hub_communication_stores content-hub gmock gtest gtest_main)
target_link_libraries(app_hub_communication_transfer content-hub gmock gtest gtest_main)
target_link_libraries(app_hub_communication_handler content-hub gmock gtest gtest_main)
target_link_libraries(test_utils content-hub gmock gtest gtest_main)

add_test(app_hub_communication_default_peer app_hub_communication_default_peer)
add_test(app_hub_communication_known_peers app_hub_communication_known_peers)
add_test(app_hub_communication_stores app_hub_communication_stores)
add_test(app_hub_communication_transfer app_hub_communication_transfer)
add_test(app_hub_communication_handler app_hub_communication_handler)
add_test(test_utils test_utils)

add_custom_command(
  OUTPUT transfer_files
  COMMAND cmake -E touch file1 file2 file3
)
