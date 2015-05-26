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
  ${APPARMOR_INCLUDE_DIRS}
)

add_executable(
  content-hub-aa-check

  main.cpp
)

qt5_use_modules(content-hub-aa-check Core)
pkg_check_modules(APPARMOR REQUIRED libapparmor)

set_target_properties(
  content-hub-aa-check
  PROPERTIES
  AUTOMOC TRUE
)

target_link_libraries(
  content-hub-aa-check
 
  ${APPARMOR_LDFLAGS}
)

install(
  TARGETS content-hub-aa-check
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
