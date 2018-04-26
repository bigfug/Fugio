#ifndef IMAGETOTEXTURENODE_H
#define IMAGETOTEXTURENODE_H

#include "opengl_includes.h"

#include <QObject>
#include <QMap>

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/opengl/texture_interface.h>
#include <fugio/image/image.h>
#include <fugio/nodecontrolbase.h>

#include <fugio/opengl/uuid.h>

class ImageToTextureNode : public fugio::NodeControlBase
{
	Q_OBJECT

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Creates an OpenGL texture from an image" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Image_To_Texture_(OpenGL)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit ImageToTextureNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~ImageToTextureNode( void );

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual QWidget *gui( void ) Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual bool initialise( void ) Q_DECL_OVERRIDE;

	virtual void loadSettings( QSettings &pSettings ) Q_DECL_OVERRIDE;

	virtual void saveSettings( QSettings &pSettings ) const Q_DECL_OVERRIDE;

signals:
	void editable( bool pEditable );

private slots:
	void onEditClicked( void );

	void transferTexture( qint64 pTimeStamp );

private:
	QSharedPointer<fugio::PinInterface>			 mPinInputImage;

	QSharedPointer<fugio::PinInterface>			 mPinOutput;
	QSharedPointer<fugio::PinInterface>			 mPinOutputTextureSize;

	fugio::OpenGLTextureInterface				*mTexture;
	fugio::VariantInterface						*mOutputTextureSize;
};


#endif // IMAGETOTEXTURENODE_H
