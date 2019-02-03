if( UNIX AND NOT APPLE )
	add_custom_command( TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND objdump -p ${CMAKE_CURRENT_BINARY_DIR}/$<TARGET_FILE_NAME:${PROJECT_NAME}> | grep NEEDED | awk '{ print $2 }' | xargs dpkg -S | awk '{ print $1 }' | cut -d ':' -f1 | sort | uniq > ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.depends
		BYPRODUCTS ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.depends
		)
endif( UNIX AND NOT APPLE )
