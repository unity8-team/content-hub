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
)

qt5_add_resources(RESOURCES qml.qrc)
add_executable(
  content-hub-peer-picker

  main.cpp
  ${RESOURCES}
)

qt5_use_modules(content-hub-peer-picker Core Gui Quick Qml)

set_target_properties(
  content-hub-peer-picker
  PROPERTIES
  AUTOMOC TRUE
)

install(
  TARGETS content-hub-peer-picker
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(
  FILES content-hub-peer-picker.desktop
  DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
)
