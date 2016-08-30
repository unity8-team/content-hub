# add a target to generate API documentation with qdoc
FIND_PROGRAM(QDOC_EXECUTABLE qdoc)
if(QDOC_EXECUTABLE)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/ubuntu-content-hub.qdocconf.in ${CMAKE_CURRENT_BINARY_DIR}/ubuntu-content-hub.qdocconf @ONLY)
add_custom_target(qmldoc
${QDOC_EXECUTABLE} -qt=qt5 ${CMAKE_CURRENT_BINARY_DIR}/ubuntu-content-hub.qdocconf
WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
COMMENT "Generating QML API documentation with qdoc" VERBATIM
)

# copy stylesheet files into build directory for shadow builds
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/css"
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
    )

install(
  DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html/
  DESTINATION ${CMAKE_INSTALL_DOCDIR}/qml/html
)

add_dependencies(doc qmldoc)

endif(QDOC_EXECUTABLE)


