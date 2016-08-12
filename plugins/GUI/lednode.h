#ifndef LEDNODE_H
#define LEDNODE_H

#include <QObject>
#include <QColor>

#include <fugio/nodecontrolbase.h>

class LedNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "GUI light for displaying boolean/trigger values." )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/LED" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE LedNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~LedNode( void ) {}

	// NodeControlInterface interface

	virtual QGraphicsItem *guiItem() Q_DECL_OVERRIDE;

	virtual bool initialise( void ) Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual void loadSettings( QSettings &pSettings ) Q_DECL_OVERRIDE;
	virtual void saveSettings( QSettings &pSettings ) const Q_DECL_OVERRIDE;

protected slots:
	void onFrameStart( qint64 pTimeStamp );

	void onColourChanged( const QColor &pColour );

signals:
	void brightnessChanged( double pBrightness );

	void colourChanged( const QColor &pColour );

protected:
	typedef enum LedMode
	{
		BOOLEAN,
		FADE,
		INTERPOLATE
	} LedMode;

	QSharedPointer<fugio::PinInterface>			 mPinInput;
	double										 mLedVal;
	double										 mLedTgt;
	qint64										 mLastTime;
	QColor										 mLedColour;
	LedMode										 mLedMode;
};

#endif // LEDNODE_H
