#ifndef FF10NODE_H
#define FF10NODE_H

#include <QObject>
#include <QLibrary>

#include <fugio/nodecontrolbase.h>

#include <fugio/core/variant_interface.h>
#include <fugio/image/image.h>

#include <FreeFrame.h>

#include "freeframelibrary.h"

class FF10Node : public fugio::NodeControlBase
{
	Q_OBJECT

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "FreeFrame 1.0" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "FF10_(FreeFrame)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE FF10Node( QSharedPointer<fugio::NodeInterface> pNode);

	virtual ~FF10Node( void ) {}

	// InterfaceNodeControl interface

	virtual bool initialise( void ) Q_DECL_OVERRIDE;

	virtual bool deinitialise( void ) Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

private:
	static void processStatic( FF10Node *pNode, qint64 pTimeStamp );

	void process( qint64 pTimeStamp );

protected:
	FreeframeLibrary								*mLibrary;
	FFInstanceID									 mInstanceId;

	QSharedPointer<fugio::PinInterface>				 mPinOutput;
	fugio::VariantInterface							*mValOutput;

	QVector<QSharedPointer<fugio::PinInterface>>	 mInputs;
	QVector<QSharedPointer<fugio::PinInterface>>	 mParams;

	FFUInt32										 mBitDepth;
	QVector<quint8>									 mDstBuf;
};

#endif // FF10NODE_H
