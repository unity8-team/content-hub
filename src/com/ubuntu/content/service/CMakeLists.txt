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

qt5_add_dbus_adaptor(
  CONTENT_SERVICE_SKELETON ${CMAKE_SOURCE_DIR}/src/com/ubuntu/content/detail/com.ubuntu.content.Service.xml
  detail/service.h com::ubuntu::content::detail::Service)

include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/src/com/ubuntu/content
  ${GSETTINGS_INCLUDE_DIRS}
)

add_executable(
  content-hub-service

  main.cpp
  registry.cpp
  ../debug.cpp
  ${CONTENT_SERVICE_SKELETON}
)

qt5_use_modules(content-hub-service Core DBus Gui)

target_link_libraries(
  content-hub-service
 
  content-hub
  ${GSETTINGS_LDFLAGS}
)

install(
  TARGETS content-hub-service
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(
  FILES com.ubuntu.content.dbus.Service.service 
  DESTINATION ${CMAKE_INSTALL_DATADIR}/dbus-1/services
)

###########################
# GSettings
###########################

add_schema ("com.ubuntu.content.hub.gschema.xml")

add_executable(
  content-hub-peer-hook

  helper.cpp
  registry.cpp
  hook.cpp
)

qt5_use_modules(content-hub-peer-hook Core Gui DBus)

target_link_libraries(
  content-hub-peer-hook
 
  content-hub
  ${GSETTINGS_LDFLAGS}
)

install(
  TARGETS content-hub-peer-hook
  RUNTIME DESTINATION "${CMAKE_INSTALL_FULL_LIBEXECDIR}/content-hub"
)

set_target_properties(
  content-hub-peer-hook
  PROPERTIES
  AUTOMOC TRUE
)

set(CLICK_HOOK
"${CMAKE_CURRENT_BINARY_DIR}/content-hub.hook"
)

configure_file("content-hub.hook.in"
  "${CLICK_HOOK}"
  @ONLY
)

install(
FILES
"${CLICK_HOOK}"
DESTINATION "/usr/share/click/hooks"
)
