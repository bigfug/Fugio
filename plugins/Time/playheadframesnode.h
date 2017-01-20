#ifndef PLAYHEADFRAMESNODE_H
#define PLAYHEADFRAMESNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

class PlayheadFramesNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Control for the patch playhead in frames" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Playhead_Frames" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE PlayheadFramesNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~PlayheadFramesNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

protected slots:
	void contextFrameInitialise( void );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputTrigger;
	QSharedPointer<fugio::PinInterface>			 mPinInputFPS;
	QSharedPointer<fugio::PinInterface>			 mPinInputFrameNumber;
	QSharedPointer<fugio::PinInterface>			 mPinInputRewind;

	QSharedPointer<fugio::PinInterface>			 mPinOutputFrameNumber;

	fugio::VariantInterface						*mValOutputFrameNumber;

	qint64										 mLastTime;
	qint32										 mFrameNumber;
};

#endif // PLAYHEADFRAMESNODE_H
