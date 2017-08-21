#ifndef VERTEX_ARRAY_OBJECT_PIN_H
#define VERTEX_ARRAY_OBJECT_PIN_H

#include "opengl_includes.h"

#include <fugio/core/uuid.h>

#include <QObject>
#include <QOpenGLVertexArrayObject>

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/opengl/vertex_array_object_interface.h>

#include <fugio/pincontrolbase.h>

class VertexArrayObjectPin : public fugio::PinControlBase, public fugio::VertexArrayObjectInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::VertexArrayObjectInterface )

public:
	Q_INVOKABLE explicit VertexArrayObjectPin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~VertexArrayObjectPin( void ) {}

	// PinControlInterface interface
public:
	virtual QString toString() const Q_DECL_OVERRIDE;
	virtual QString description() const Q_DECL_OVERRIDE;

	// VertexArrayObjectInterface interface
public:
//	virtual QOpenGLVertexArrayObject &vao() Q_DECL_OVERRIDE
//	{
//		return( mVAO );
//	}

private:
	fugio::VertexArrayObjectInterface *vaoParent( void );
	fugio::VertexArrayObjectInterface *vaoParent( void ) const;

private:
//	QOpenGLVertexArrayObject	 mVAO;

	// VertexArrayObjectInterface interface
public:
	virtual void vaoBind() Q_DECL_OVERRIDE;
	virtual void vaoRelease() Q_DECL_OVERRIDE;
	virtual GLuint vaoId() const Q_DECL_OVERRIDE;
};

#endif // VERTEX_ARRAY_OBJECT_PIN_H
