#ifndef PLAYHEADCONTROLNODE_H
#define PLAYHEADCONTROLNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

class PlayheadControlNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Control for the patch playhead" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Playhead_Control" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE PlayheadControlNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~PlayheadControlNode( void ) {}

	// NodeControlInterface interface

	virtual bool initialise( void ) Q_DECL_OVERRIDE;

	virtual bool deinitialise( void ) Q_DECL_OVERRIDE;

protected slots:
	void contextFrameInitialise( qint64 pTimeStamp );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputPlay;
	QSharedPointer<fugio::PinInterface>			 mPinInputStop;
	QSharedPointer<fugio::PinInterface>			 mPinInputRewind;

	qint64										 mLastTime;
};

#endif // PLAYHEADCONTROLNODE_H
