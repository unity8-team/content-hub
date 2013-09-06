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

include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/src/com/ubuntu/content
)

add_executable(
  content-hub-install-peer

  main.cpp
)

qt5_use_modules(content-hub-install-peer Core)

target_link_libraries(
  content-hub-install-peer
 
  content-hub
)

install(
  TARGETS content-hub-install-peer
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
