#ifndef IMAGEPREVIEWNODE_H
#define IMAGEPREVIEWNODE_H

#include <QObject>

#include <QDockWidget>
#include <QLabel>

#include <fugio/core/variant_interface.h>
#include <fugio/image/image.h>
#include <fugio/gui/input_events_interface.h>

#include <fugio/nodecontrolbase.h>

class ImagePreview;

class ImagePreviewNode : public fugio::NodeControlBase
{
	Q_OBJECT

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Displays an image" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Preview_(Image)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit ImagePreviewNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~ImagePreviewNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual QWidget *gui( void ) Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual bool initialise( void ) Q_DECL_OVERRIDE;

	virtual bool deinitialise( void ) Q_DECL_OVERRIDE;

	virtual void loadSettings( QSettings &pSettings ) Q_DECL_OVERRIDE;

	virtual void saveSettings( QSettings &pSettings ) const Q_DECL_OVERRIDE;

protected slots:
	void onShowClicked();

	void contextFrameInitialise( void );

private:
	QSharedPointer<fugio::PinInterface>			 mPinImage;

	QSharedPointer<fugio::PinInterface>			 mPinOutputEvents;
	fugio::InputEventsInterface					*mValOutputEvents;

	QSharedPointer<fugio::PinInterface>			 mPinOutputSize;
	fugio::VariantInterface						*mValOutputSize;

	QDockWidget									*mDockWidget;
	ImagePreview								*mGUI;

	Qt::DockWidgetArea							 mDockArea;
};

#endif // IMAGEPREVIEWNODE_H
