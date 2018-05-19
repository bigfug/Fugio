#ifndef TUIOCLIENTNODE_H
#define TUIOCLIENTNODE_H

#include <QObject>

#include <TuioClient.h>
#include <TuioListener.h>
#include <OscReceiver.h>

#include <fugio/nodecontrolbase.h>
#include <fugio/core/variant_interface.h>

class TuioClientNode : public fugio::NodeControlBase, public TUIO::TuioListener, public TUIO::OscReceiver
{
	Q_OBJECT

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "TUIO Client" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Tuio_Client" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE TuioClientNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~TuioClientNode( void ) {}

	// NodeControlInterface interface

	virtual bool initialise() Q_DECL_OVERRIDE;
	virtual bool deinitialise() Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	// TuioListener interface
public:
	virtual void addTuioObject(TUIO::TuioObject *tobj) Q_DECL_OVERRIDE;
	virtual void updateTuioObject(TUIO::TuioObject *tobj) Q_DECL_OVERRIDE;
	virtual void removeTuioObject(TUIO::TuioObject *tobj) Q_DECL_OVERRIDE;
	virtual void addTuioCursor(TUIO::TuioCursor *tcur) Q_DECL_OVERRIDE;
	virtual void updateTuioCursor(TUIO::TuioCursor *tcur) Q_DECL_OVERRIDE;
	virtual void removeTuioCursor(TUIO::TuioCursor *tcur) Q_DECL_OVERRIDE;
	virtual void addTuioBlob(TUIO::TuioBlob *tblb) Q_DECL_OVERRIDE;
	virtual void updateTuioBlob(TUIO::TuioBlob *tblb) Q_DECL_OVERRIDE;
	virtual void removeTuioBlob(TUIO::TuioBlob *tblb) Q_DECL_OVERRIDE;
	virtual void refresh(TUIO::TuioTime ftime) Q_DECL_OVERRIDE;

	// OscReceiver interface
public:
	virtual void connect(bool lock) Q_DECL_OVERRIDE;
	virtual void disconnect() Q_DECL_OVERRIDE;

protected:
	void reportBlob( TUIO::TuioBlob *tblb, const QString &pType );
	void reportCursor( TUIO::TuioCursor *tcur, const QString &pType );
	void reportObject( TUIO::TuioObject *tobj, const QString &pType );

protected:
	TUIO::TuioClient							*mClient;

	QSharedPointer<fugio::PinInterface>			 mPinInput;

	QSharedPointer<fugio::PinInterface>			 mPinOutputData;

	fugio::VariantInterface						*mValOutputData;
};

#endif // TUIOCLIENTNODE_H
