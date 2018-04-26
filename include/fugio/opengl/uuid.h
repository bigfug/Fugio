#ifndef FUGIO_OPENGL_H
#define FUGIO_OPENGL_H

#include <QUuid>
#include <QObject>

#define NID_OPENGL_TEXTURE				(QUuid("{c0e19b9a-4058-4c3e-858b-1f28fb265b4a}"))
#define NID_OPENGL_TEXTURE_COPY			(QUuid("{098ED8FF-7776-4b6b-87AD-2F637C2DAC17}"))
#define NID_OPENGL_TEXTURE_CLONE		(QUuid("{829b267c-57b7-4f44-a63b-be52f8259702}"))
#define NID_OPENGL_STATE				(QUuid("{D15AD252-636E-428C-A21E-104EAD4A2C26}"))
#define NID_OPENGL_PREVIEW				(QUuid("{1df41fed-06aa-46c6-b3b3-a5ae55898e75}"))
#define NID_OPENGL_DRAW					(QUuid("{62a1a753-b0f7-43bc-aac7-e6b99cd15047}"))
#define NID_OPENGL_SHADER_INSTANCE		(QUuid("{607d94a3-2630-40af-a3d5-149690144403}"))
#define NID_OPENGL_SHADER_COMPILER		(QUuid("{e4255ca9-6d97-4f69-8823-8b164699a93b}"))
#define NID_OPENGL_VIEWPORT				(QUuid("{3f65812f-3e2e-47f2-84f9-e5417552f088}"))
#define NID_OPENGL_BIND_TEXTURE			(QUuid("{070fb8fe-a685-4f05-aaa4-cef18a0ec9bb}"))
#define	NID_OPENGL_WINDOW				(QUuid("{3f4cda9b-a97f-4020-8f4f-63e2a4ab4b09}"))
#define	NID_OPENGL_SKYBOX				(QUuid("{64291061-5de1-46df-983c-5fd904e8f30f}"))
#define NID_OPENGL_TEXTURE_CUBE			(QUuid("{3bdefd06-3cf7-4490-8f78-a5d5ae11909d}"))
#define NID_OPENGL_CLEAR				(QUuid("{36b7a906-1e5d-41d5-8e86-e8a2ab171393}"))
#define NID_OPENGL_RENDER				(QUuid("{eab1306a-54b2-4ae6-913e-b0175e8ca9a9}"))
#define NID_OPENGL_IMAGE_TO_TEXTURE		(QUuid("{44aa8911-9e96-4e83-9793-42b7b81c84c2}"))
#define NID_OPENGL_TEXTURE_TO_IMAGE		(QUuid("{C6F67F73-4651-4631-9BEE-4CD9B329D0A6}"))
#define NID_OPENGL_TRANSFORM_FEEDBACK	(QUuid("{fafe5f60-666f-473a-b69f-914090a31b0b}"))
#define NID_OPENGL_BUFFER_TO_ARRAY		(QUuid("{7dd527d3-798d-4cf6-8518-65f6ff06b6a9}"))
#define NID_OPENGL_BUFFER_ENTRY			(QUuid("{7931a29b-7c7e-45d9-9579-bf3953ada21c}"))
#define NID_OPENGL_BUFFER				(QUuid("{030DF236-EE36-484C-8BD0-6D3E3A7226AE"))
#define NID_OPENGL_ARRAY_TO_BUFFER		(QUuid("{e28b66d0-2e4a-41ba-be21-51c00d251607}"))
#define NID_OPENGL_VERTEX_ARRAY_OBJECT	(QUuid("{0fe29f89-88ce-46ec-a161-305955e5e1aa}"))
#define NID_OPENGL_CONTEXT				(QUuid("{d0c15581-4aad-4f81-86d1-c87cdc4fdd64}"))
#define NID_OPENGL_ARRAY_TO_INDEX		(QUuid("{1592b1fb-8c72-46c9-87ce-2d6443dd9788}"))
#define NID_OPENGL_INSTANCE_BUFFER		(QUuid("{C2569AD8-6398-47EA-AA0D-D217FBA04C39}"))
#define NID_OPENGL_VIEWPORT_MATRIX		(QUuid("{0ACF74B6-1541-4760-9D71-1E64C88E9990}"))
#define NID_OPENGL_CUBEMAP_RENDER		(QUuid("{905ea949-6501-4183-832e-59433ee4ad5f}"))
#define NID_OPENGL_TEXTURE_MONITOR		(QUuid("{1EF6A715-A38C-490E-875B-1478B7484FC0}"))
#define NID_OPENGL_RENDER_TO_TEXTURE	(QUuid("{2be7aad9-e60c-43e8-9728-f7a8df5af058}"))
#define NID_OPENGL_EASY_SHADER_2D		(QUuid("{BA9DFE93-AEF5-4269-A80B-85B01C37AB2F}"))
#define NID_OPENGL_ARRAY_TO_TEXTURE		(QUuid("{DE843971-5AEE-4200-987F-32BDC8B377A8}"))

#define PID_RENDER						(QUuid("{fc5472cd-03b2-42e1-8e65-6ada5c860962}"))
#define	PID_OPENGL_TEXTURE				(QUuid("{1dbaa75d-81bd-478a-94a8-ae1a5c1b7f12}"))
#define	PID_OPENGL_STATE				(QUuid("{13545443-42C0-44A6-9C2A-0032FFE4651F}"))
#define PID_OPENGL_BUFFER				(QUuid("{fcadb6b8-cca9-45f9-a139-c2615527c75f}"))
#define PID_OPENGL_SHADER				(QUuid("{18c2cfcd-fd22-4b27-8725-5ca9195b8e42}"))
#define PID_OPENGL_BUFFER_ENTRY			(QUuid("{fefc7f77-f26b-40a5-be4f-31d4853740cc}"))
#define PID_OPENGL_BUFFER_ENTRY_PROXY	(QUuid("{8B974058-1F1B-4E85-A340-0EB65A80F997}"))
#define PID_OPENGL_VERTEX_ARRAY_OBJECT	(QUuid("{262d7dd5-a1dc-4011-a6e8-38c48d1ac550}"))

#define	IID_OPENGL						(QUuid("{1dbaa75d-eeee-478a-94a8-ae1a5c1b7f12}"))

#define SYNTAX_HIGHLIGHTER_GLSL			(QUuid("{69630335-9F83-4E45-BF88-1C28CBC2530B}"))

class InterfaceOpenGL
{
public:
	virtual ~InterfaceOpenGL( void ) {}

//	virtual void registerContext( QOpenGLContext *pContext ) = 0;

//	virtual void unregisterContext( QOpenGLContext *pContext ) = 0;

	virtual void checkErrors( void ) = 0;

	virtual void checkErrors( const char *file, int line ) = 0;

	virtual void registerOutputWindowHook( QObject *pObject, const char *pMember ) = 0;

	virtual int triangleCount( void ) const = 0;

	virtual void resetTriangleCount( void ) = 0;

	virtual void incrementTriangleCount( int pTriangleCount ) = 0;

	virtual bool hasContext( void ) = 0;
};

Q_DECLARE_INTERFACE( InterfaceOpenGL, "com.bigfug.fugio.opengl/1.0" )

#if !defined( BUFFER_OFFSET )
#define BUFFER_OFFSET(offset) ((void*)(offset))
#endif

#endif // FUGIO_OPENGL_H
