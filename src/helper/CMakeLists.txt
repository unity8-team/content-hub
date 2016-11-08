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

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -fPIE ${GCOV_FLAGS}")

include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${UBUNTU_LAUNCH_INCLUDE_DIRS}
  ${GIO_INCLUDE_DIRS}
  ${GLIB_INCLUDE_DIRS}
  ${CLICK_INCLUDE_DIRS}
  ${MIRCLIENT_INCLUDE_DIRS}
)

add_executable(
  exec-tool

  exec-tool.c
)

target_link_libraries(
  exec-tool
 
  ${UBUNTU_LAUNCH_LDFLAGS}
  ${CLICK_LDFLAGS}
  ${GLIB_LDFLAGS}
  ${GIO_LDFLAGS}
  ${MIRCLIENT_LDFLAGS}
)

install(
  PROGRAMS "${CMAKE_CURRENT_BINARY_DIR}/exec-tool"
  DESTINATION "${CMAKE_INSTALL_FULL_LIBEXECDIR}/ubuntu-app-launch/content-hub"
)
