# Copyright © 2016 Canonical Ltd.
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
# Authored by: Arthur Mello <arthur.mello@canonical.com>

set(CLIPBOARD_APP content-hub-clipboard)
set(CLIPBOARD_APP_DIR ${CMAKE_INSTALL_DATADIR}/${CAMERA_APP})
set(PLUGIN_BASE ${CMAKE_INSTALL_LIBDIR}/${CLIPBOARD_APP})

configure_file(config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h @ONLY)

include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
)

set(clipboard_app_HDRS
    paste-data-model.h
    paste-data-deleted-model.h
    clipboardapplication.h)

set(clipboard_app_SRCS
    paste-data-model.cpp
    paste-data-deleted-model.cpp
    clipboardapplication.cpp
    main.cpp)

add_executable(${CLIPBOARD_APP}
    ${clipboard_app_SRCS}
)

qt5_use_modules(${CLIPBOARD_APP} Core Gui DBus Qml Quick)

set_target_properties(${CLIPBOARD_APP}
  PROPERTIES
  AUTOMOC TRUE
)

target_link_libraries(${CLIPBOARD_APP}
  content-hub
)

file(GLOB QML_JS_FILES *.qml *.js)

install(FILES ${QML_JS_FILES}
    DESTINATION ${CLIPBOARD_APP_DIR}
    )

install(
  TARGETS ${CLIPBOARD_APP}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(
  FILES content-hub-clipboard.desktop
  DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
)
