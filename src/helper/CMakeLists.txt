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

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(
  exec-tool

  exec-tool.cpp
)

qt5_use_modules(exec-tool Core)

target_link_libraries(
  exec-tool
 
  ${UBUNTU_LAUNCH_LDFLAGS}
)

install(
  PROGRAMS "${CMAKE_CURRENT_BINARY_DIR}/exec-tool"
  DESTINATION "${CMAKE_INSTALL_FULL_LIBEXECDIR}/ubuntu-app-launch/content-hub"
)
