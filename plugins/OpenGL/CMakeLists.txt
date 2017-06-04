cmake_minimum_required( VERSION 3.1 )

project( fugio-opengl )

set( SOURCE_FILES
	../../include/fugio/nodecontrolbase.h
	../../include/fugio/pincontrolbase.h
	../../include/fugio/opengl/uuid.h
	../../include/fugio/opengl/node_render_interface.h
	../../include/fugio/opengl/vertex_array_object_interface.h
	../../include/fugio/opengl/buffer_entry_interface.h
	../../include/fugio/opengl/buffer_interface.h
	../../include/fugio/opengl/output_interface.h
	../../include/fugio/opengl/shader_interface.h
	../../include/fugio/opengl/texture_interface.h
	../../include/fugio/opengl/state_interface.h
	../../include/fugio/render_interface.h
	../../include/fugio/output_interface.h
	openglplugin.cpp
	texturenode.cpp
	texturepin.cpp
	syntaxhighlighterglsl.cpp
	texturecopynode.cpp
	texturenodeform.cpp
	shaderinstancenode.cpp
	shaderpin.cpp
	viewportnode.cpp
	bindtexturenode.cpp
	texturecubenode.cpp
	clearnode.cpp
	rendernode.cpp
	imagetotexturenode.cpp
	texturetoimagenode.cpp
	transformfeedbacknode.cpp
	buffertoarraynode.cpp
	bufferentrynode.cpp
	bufferentrypin.cpp
	bufferentryproxypin.cpp
	buffernode.cpp
	contextnode.cpp
	arraytobuffernode.cpp
	vertexarrayobjectnode.cpp
	vertexarrayobjectpin.cpp
	arraytoindexnode.cpp
	instancebuffernode.cpp
	bufferpin.cpp
	drawnode.cpp
	preview.cpp
	previewnode.cpp
	stateform.cpp
	statenode.cpp
	statepin.cpp
	windownode.cpp
	shadercompilernode.cpp
	viewportmatrixnode.cpp
	renderpin.cpp
	cubemaprendernode.cpp
	textureclonenode.cpp
	texturemonitornode.cpp
	texturemonitor.cpp
	texturenode.h
	texturepin.h
	openglplugin.h
	syntaxhighlighterglsl.h
	texturecopynode.h
	texturenodeform.h
	shaderinstancenode.h
	shaderpin.h
	viewportnode.h
	bindtexturenode.h
	texturecubenode.h
	clearnode.h
	rendernode.h
	opengl_includes.h
	imagetotexturenode.h
	texturetoimagenode.h
	transformfeedbacknode.h
	buffertoarraynode.h
	bufferentrynode.h
	bufferentrypin.h
	bufferentryproxypin.h
	buffernode.h
	contextnode.h
	arraytobuffernode.h
	vertexarrayobjectnode.h
	vertexarrayobjectpin.h
	arraytoindexnode.h
	instancebuffernode.h
	bufferpin.h
	drawnode.h
	preview.h
	previewnode.h
	stateform.h
	statenode.h
	statepin.h
	windownode.h
	shadercompilernode.h
	viewportmatrixnode.h
	renderpin.h
	cubemaprendernode.h
	textureclonenode.h
    texturemonitornode.h
    texturemonitor.h

	deviceopengloutput.cpp
deviceopengloutput.h

texturenodeform.ui
	openglstateform.ui
)

set( QRC_FILES
	resources.qrc
)

find_package( glew REQUIRED )

include_directories( ${GLEW_INCLUDE_DIRS} )

add_library( ${PROJECT_NAME} MODULE ${SOURCE_FILES} ${QRC_FILES} )

target_link_libraries( ${PROJECT_NAME} ${GLEW_LIBRARIES} )


set_target_properties( ${PROJECT_NAME} PROPERTIES
  BUNDLE TRUE
)

#—————————————————————————————————-
# Install

install( TARGETS ${PROJECT_NAME} DESTINATION plugins )

#—————————————————————————————————-
# Qt5

find_package( Qt5 COMPONENTS Core Concurrent Widgets REQUIRED )

target_link_libraries( ${PROJECT_NAME} Qt5::Core Qt5::Concurrent Qt5::Widgets )

