set( TS_FILES
	translations/translations_de.ts
	translations/translations_es.ts
	translations/translations_fr.ts
	)

qt5_create_translation( QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${TS_FILES} )

configure_file( ${CMAKE_SOURCE_DIR}/translations.qrc ${CMAKE_CURRENT_BINARY_DIR} COPYONLY )

target_sources( ${PROJECT_NAME} PRIVATE
	${TS_FILES}
	${QM_FILES}
	${CMAKE_CURRENT_BINARY_DIR}/translations.qrc
	)
