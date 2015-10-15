# Copyright © 2015 Canonical Ltd.
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
# Authored by: Ken VanDine <ken.vandine@canonical.com>

include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/src/com/ubuntu/content
  ${UBUNTU_LAUNCH_INCLUDE_DIRS}
)

add_executable(
  content-hub-qicon-helper

  main.cpp
)

qt5_use_modules(content-hub-qicon-helper Gui DBus)

set_target_properties(
  content-hub-qicon-helper
  PROPERTIES
  AUTOMOC TRUE
)

target_link_libraries(
  content-hub-qicon-helper
 
  content-hub 
  ${UBUNTU_LAUNCH_LDFLAGS}
)

install(
  TARGETS content-hub-qicon-helper
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
