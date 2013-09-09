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
    contenttransfer.h
    contenttype.h
    qmlimportexporthandler.h
    )

set(PLUGIN_SRC
    contenthub.cpp
    contenthubplugin.cpp
    contentitem.cpp
    contentpeer.cpp
    contenttransfer.cpp
    contenttype.cpp
    qmlimportexporthandler.cpp
    )

add_library(${PLUGIN} MODULE ${PLUGIN_SRC} ${PLUGIN_HDRS})

qt5_use_modules(${PLUGIN} Core Qml Quick)
target_link_libraries(${PLUGIN} content-hub)

install(TARGETS ${PLUGIN} DESTINATION ${CONTENT_HUB_IMPORTS_DIR})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/qmldir DESTINATION ${CONTENT_HUB_IMPORTS_DIR})

file(GLOB QML_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.qml qmldir)
install(FILES ${QML_FILES} DESTINATION ${CONTENT_HUB_IMPORTS_DIR})

if(NOT ${CMAKE_CURRENT_BINARY_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
  # copy qml files and assets over to build dir to be able to import them uninstalled
  foreach(_file ${QML_FILES})
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${_file}
                       DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${_file}
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/${_file} ${CMAKE_CURRENT_BINARY_DIR}/${_file})
  endforeach(_file)
  add_custom_target(copy_files_to_build_dir DEPENDS ${QML_FILES})
  add_dependencies(${PLUGIN} copy_files_to_build_dir)
endif()
