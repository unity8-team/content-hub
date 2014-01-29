configure_file("helper-exec-tool.in" "${CMAKE_CURRENT_BINARY_DIR}/helper-exec-tool" @ONLY)

install(
	PROGRAMS "${CMAKE_CURRENT_BINARY_DIR}/helper-exec-tool"
	DESTINATION "${CMAKE_INSTALL_FULL_LIBEXECDIR}/upstart-app-launch/content-hub/"
	RENAME "exec-tool"
)
