#ifndef TRANSFORMFEEDBACKNODE_H
#define TRANSFORMFEEDBACKNODE_H

#include "opengl_includes.h"

#include <QObject>

#include <fugio/nodecontrolbase.h>
#include <fugio/pin_control_interface.h>
#include <fugio/choice_interface.h>

//#include <fugio/interface_texture.h>
#include <fugio/render_interface.h>
#include <fugio/opengl/buffer_interface.h>
#include <fugio/paired_pins_helper_interface.h>
#include <fugio/opengl/node_render_interface.h>

class TransformFeedbackNode : public fugio::NodeControlBase, public fugio::NodeRenderInterface, public fugio::PairedPinsHelperInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::NodeRenderInterface fugio::PairedPinsHelperInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Transform_Feedback_(OpenGL)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE TransformFeedbackNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~TransformFeedbackNode( void );

	// NodeControlInterface interface
public:
	virtual bool initialise() Q_DECL_OVERRIDE;
	virtual bool deinitialise() Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual QList<QUuid> pinAddTypesInput() const Q_DECL_OVERRIDE;
	virtual bool canAcceptPin(fugio::PinInterface *pPin) const Q_DECL_OVERRIDE;

	// fugio::NodeRenderInterface interface
public:
	virtual void render( qint64 pTimeStamp, QUuid pSourcePinId ) Q_DECL_OVERRIDE;

	// PairedPinsHelperInterface interface
public:
	virtual QUuid pairedPinControlUuid( QSharedPointer<fugio::PinInterface> pPin ) const Q_DECL_OVERRIDE;

protected:
	void render1( qint64 pTimeStamp );
	void render2( qint64 pTimeStamp );

	inline int currTFB( void ) const
	{
		return( m_currTFB );
	}

	inline int nextTFB( void ) const
	{
		return( ( m_currTFB + 1 ) % 2 );
	}

protected:
	QSharedPointer<fugio::PinInterface>				 mPinInputType;
	fugio::ChoiceInterface							*mValInputType;

	QSharedPointer<fugio::PinInterface>				 mPinInputCount;

	QSharedPointer<fugio::PinInterface>				 mPinGeometry1;
	fugio::RenderInterface							*mValGeometry1;

	QSharedPointer<fugio::PinInterface>				 mPinGeometry2;
	fugio::RenderInterface							*mValGeometry2;

	GLuint											 mTrnFbkId[ 2 ];
	int												 m_currTFB;
	GLenum											 mDrawMode;
	GLenum											 mTransformMode;
	int												 mBufferCount;
	bool											 mFirstTransform;

	static QMap<QString,GLenum>						 mModeMap;
};

#endif // TRANSFORMFEEDBACKNODE_H
