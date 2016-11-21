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

# Build with system gmock and embedded gtest
set (GMOCK_INCLUDE_DIR "/usr/include/gmock/include" CACHE PATH "gmock source include directory")
set (GMOCK_SOURCE_DIR "/usr/src/gmock" CACHE PATH "gmock source directory")
set (GTEST_INCLUDE_DIR "${GMOCK_SOURCE_DIR}/gtest/include" CACHE PATH "gtest source include directory")

add_definitions(-DQT_NO_KEYWORDS)

add_subdirectory(${GMOCK_SOURCE_DIR} "${CMAKE_CURRENT_BINARY_DIR}/gmock")

include_directories (
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/src/com/ubuntu/content
  ${GIO_INCLUDE_DIRS}
  ${LIBERTINE_INCLUDE_DIRS}
  ${UBUNTU_LAUNCH_INCLUDE_DIRS}
  ${CMAKE_BINARY_DIR}/src
  ${CMAKE_BINARY_DIR}/src/com/ubuntu/content
  ${GMOCK_INCLUDE_DIR}
  ${GTEST_INCLUDE_DIR}
)

add_subdirectory(acceptance-tests)
add_subdirectory(qml-tests)
add_subdirectory(peers)
