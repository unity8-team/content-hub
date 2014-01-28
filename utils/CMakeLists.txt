

install(
	PROGRAMS helper-exec-tool.sh
	DESTINATION "${CMAKE_INSTALL_FULL_LIBEXECDIR}/upstart-app-launch/content-hub/"
	RENAME "exec-tool"
)
