cmake_minimum_required( VERSION 3.1 )

project( fugio-opengl )

set( SOURCE_FILES
	../../include/fugio/nodecontrolbase.h
	../../include/fugio/pincontrolbase.h

	../../include/fugio/opengl/buffer_entry_interface.h
	../../include/fugio/opengl/buffer_interface.h
	../../include/fugio/opengl/node_render_interface.h
	../../include/fugio/opengl/output_interface.h
	../../include/fugio/opengl/shader_interface.h
	../../include/fugio/opengl/state_interface.h
	../../include/fugio/opengl/texture_interface.h
	../../include/fugio/opengl/uuid.h
	../../include/fugio/opengl/vertex_array_object_interface.h
	../../include/fugio/output_interface.h
	../../include/fugio/render_interface.h
	
	arraytobuffernode.cpp
	arraytobuffernode.h
	arraytoindexnode.cpp
	arraytoindexnode.h
	bindtexturenode.cpp
	bindtexturenode.h
	bufferentrynode.cpp
	bufferentrynode.h
	bufferentrypin.cpp
	bufferentrypin.h
	bufferentryproxypin.cpp
	bufferentryproxypin.h
	buffernode.cpp
	buffernode.h
	bufferpin.cpp
	bufferpin.h
	buffertoarraynode.cpp
	buffertoarraynode.h
	clearnode.cpp
	clearnode.h
	contextnode.cpp
	contextnode.h
	cubemaprendernode.cpp
	cubemaprendernode.h
	deviceopengloutput.cpp
	deviceopengloutput.h
	drawnode.cpp
	drawnode.h
	imagetotexturenode.cpp
	imagetotexturenode.h
	instancebuffernode.cpp
	instancebuffernode.h
	opengl_includes.h
	openglplugin.cpp
	openglplugin.h
	preview.cpp
	preview.h
	previewnode.cpp
	previewnode.h
	rendernode.cpp
	rendernode.h
	renderpin.cpp
	renderpin.h
	rendertotexturenode.cpp
	rendertotexturenode.h
	shadercompilernode.cpp
	shadercompilernode.h
	shaderinstancenode.cpp
	shaderinstancenode.h
	shaderpin.cpp
	shaderpin.h
	stateform.cpp
	stateform.h
	statenode.cpp
	statenode.h
	statepin.cpp
	statepin.h
	syntaxhighlighterglsl.cpp
	syntaxhighlighterglsl.h
	textureclonenode.cpp
	textureclonenode.h
	texturecopynode.cpp
	texturecopynode.h
	texturecubenode.cpp
	texturecubenode.h
	texturemonitor.cpp
	texturemonitornode.cpp
	texturenode.cpp
	texturenode.h
	texturenodeform.cpp
	texturenodeform.h
	texturepin.cpp
	texturepin.h
	texturetoimagenode.cpp
	texturetoimagenode.h
	transformfeedbacknode.cpp
	transformfeedbacknode.h
	vertexarrayobjectnode.cpp
	vertexarrayobjectnode.h
	vertexarrayobjectpin.cpp
	vertexarrayobjectpin.h
	viewportmatrixnode.cpp
	viewportmatrixnode.h
	viewportnode.cpp
	viewportnode.h
	windownode.cpp
	windownode.h
    texturemonitor.h
    texturemonitornode.h

	easyshader2dnode.cpp
	easyshader2dnode.h
	shadercompilerdata.cpp
	shadercompilerdata.h
	arraytotexturebuffernode.cpp
	arraytotexturebuffernode.h

	openglstateform.ui
	texturenodeform.ui
)

set( QRC_FILES
#	resources.qrc
)

#------------------------------------------------------------------------------

add_library( ${PROJECT_NAME} MODULE ${SOURCE_FILES} ${QRC_FILES} )

set_target_properties( ${PROJECT_NAME} PROPERTIES
  BUNDLE TRUE
)

#------------------------------------------------------------------------------
# Install

install( TARGETS ${PROJECT_NAME} DESTINATION ${PATH_PLUGIN} )

#------------------------------------------------------------------------------
# Qt5

find_package( Qt5 COMPONENTS Core Concurrent OpenGL Widgets LinguistTools REQUIRED )

target_link_libraries( ${PROJECT_NAME} Qt5::Core Qt5::Concurrent Qt5::OpenGL Qt5::Widgets )

include( ${CMAKE_SOURCE_DIR}/cmake/AddTranslations.cmake )

