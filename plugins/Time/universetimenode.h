#ifndef UNIVERSEUniverseTimeNode_H
#define UNIVERSEUniverseTimeNode_H

#include <fugio/core/uuid.h>

#include <fugio/nodecontrolbase.h>

class UniverseTimeNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Time" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "UniverseTime" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit UniverseTimeNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~UniverseTimeNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual bool initialise( void ) Q_DECL_OVERRIDE;

	virtual bool deinitialise( void ) Q_DECL_OVERRIDE;

private slots:
	void contextFrameStart( qint64 pTimeStamp );

private:
	QSharedPointer<fugio::PinInterface>			 mPinInputInterval;

	QSharedPointer<fugio::PinInterface>			 mPinOutputTimeMs;
	fugio::VariantInterface						*mValOutputTimeMs;

	QSharedPointer<fugio::PinInterface>			 mPinOutputTimeS;
	fugio::VariantInterface						*mValOutputTimeS;

	qint64										 mLastUpdate;
};


#endif // UNIVERSEUniverseTimeNode_H
