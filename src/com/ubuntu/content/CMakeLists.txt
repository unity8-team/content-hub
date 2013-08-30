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

add_subdirectory(service)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

qt5_add_dbus_interface(
  CONTENT_SERVICE_STUB ${CMAKE_CURRENT_SOURCE_DIR}/detail/com.ubuntu.content.Service.xml 
  ContentServiceInterface)
qt5_add_dbus_adaptor(
  CONTENT_SERVICE_SKELETON ${CMAKE_CURRENT_SOURCE_DIR}/detail/com.ubuntu.content.Service.xml 
  detail/service.h com::ubuntu::content::detail::Service)

qt5_add_dbus_interface(
  CONTENT_TRANSFER_STUB ${CMAKE_CURRENT_SOURCE_DIR}/detail/com.ubuntu.content.Transfer.xml 
  ContentTransferInterface)
qt5_add_dbus_adaptor(
  CONTENT_TRANSFER_SKELETON ${CMAKE_CURRENT_SOURCE_DIR}/detail/com.ubuntu.content.Transfer.xml 
  detail/transfer.h com::ubuntu::content::detail::Transfer)

qt5_add_dbus_interface(
  CONTENT_HANDLER_STUB ${CMAKE_CURRENT_SOURCE_DIR}/detail/com.ubuntu.content.Handler.xml 
  ContentHandlerInterface)
qt5_add_dbus_adaptor(
  CONTENT_HANDLER_SKELETON ${CMAKE_CURRENT_SOURCE_DIR}/detail/com.ubuntu.content.Handler.xml 
  detail/handler.h com::ubuntu::content::detail::Handler)

qt5_wrap_cpp(CONTENT_HUB_MOCS ${CMAKE_SOURCE_DIR}/include/com/ubuntu/content/hub.h)
qt5_wrap_cpp(CONTENT_HUB_MOCS ${CMAKE_SOURCE_DIR}/include/com/ubuntu/content/import_export_handler.h)
qt5_wrap_cpp(CONTENT_HUB_MOCS ${CMAKE_SOURCE_DIR}/include/com/ubuntu/content/item.h)
qt5_wrap_cpp(CONTENT_HUB_MOCS ${CMAKE_SOURCE_DIR}/include/com/ubuntu/content/peer.h)
qt5_wrap_cpp(CONTENT_HUB_MOCS ${CMAKE_SOURCE_DIR}/include/com/ubuntu/content/store.h)
qt5_wrap_cpp(CONTENT_HUB_MOCS ${CMAKE_SOURCE_DIR}/include/com/ubuntu/content/transfer.h)
qt5_wrap_cpp(CONTENT_HUB_MOCS ${CMAKE_SOURCE_DIR}/include/com/ubuntu/content/type.h)

add_library(
  content-hub SHARED

  hub.cpp
  import_export_handler.cpp
  item.cpp
  peer.cpp
  store.cpp
  transfer.cpp
  type.cpp
  utils.cpp

  detail/service.cpp
  detail/transfer.cpp
  detail/handler.cpp

  ${CONTENT_HUB_MOCS}
  ${CONTENT_SERVICE_STUB}
  ${CONTENT_SERVICE_SKELETON}
  ${CONTENT_TRANSFER_STUB}
  ${CONTENT_TRANSFER_SKELETON}
  ${CONTENT_HANDLER_STUB}
  ${CONTENT_HANDLER_SKELETON}
)

set_target_properties(
  content-hub

  PROPERTIES
  VERSION ${CONTENT_HUB_VERSION_MAJOR}.${CONTENT_HUB_VERSION_MINOR}.${CONTENT_HUB_VERSION_PATCH}
  SOVERSION ${CONTENT_HUB_VERSION_MAJOR}
  AUTOMOC TRUE
)

qt5_use_modules(content-hub Core DBus Xml)

install(
  TARGETS content-hub
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
