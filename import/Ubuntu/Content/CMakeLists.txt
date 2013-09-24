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

project(content-hub-plugin)

find_package(Qt5Quick REQUIRED)
set(CMAKE_AUTOMOC TRUE)

execute_process(
    COMMAND qmake -query QT_INSTALL_QML
    OUTPUT_VARIABLE QT_INSTALL_QML
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
set(CONTENT_HUB_IMPORTS_DIR "${QT_INSTALL_QML}/Ubuntu/Content")

set(PLUGIN ubuntu-content-hub-plugin)

include_directories(${CMAKE_SOURCE_DIR})

set(PLUGIN_HDRS
    contenthub.h
    contenthubplugin.h
    contentitem.h
    contentpeer.h
    contentstore.h
    contenttransfer.h
    contenttype.h
    qmlimportexporthandler.h
    )

set(PLUGIN_SRC
    contenthub.cpp
    contenthubplugin.cpp
    contentitem.cpp
    contentpeer.cpp
    contentstore.cpp
    contenttransfer.cpp
    contenttype.cpp
    qmlimportexporthandler.cpp
    )

add_library(${PLUGIN} MODULE ${PLUGIN_SRC} ${PLUGIN_HDRS})

qt5_use_modules(${PLUGIN} Core Qml Quick)
target_link_libraries(${PLUGIN} content-hub)

install(TARGETS ${PLUGIN} DESTINATION ${CONTENT_HUB_IMPORTS_DIR})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/qmldir DESTINATION ${CONTENT_HUB_IMPORTS_DIR})

# for the qml-tests
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/qmldir
                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/qmldir
                   COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/qmldir ${CMAKE_CURRENT_BINARY_DIR}/qmldir)
add_custom_target(copy_qmldir_file_to_build_dir DEPENDS qmldir)
add_dependencies(${PLUGIN} copy_qmldir_file_to_build_dir)
