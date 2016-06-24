#ifndef ENCODERNODE_H
#define ENCODERNODE_H

#include <fugio/nodecontrolbase.h>
#include <fugio/osc/uuid.h>
#include <fugio/osc/join_interface.h>
#include <fugio/osc/namespace_interface.h>

class EncoderNode : public fugio::NodeControlBase, public fugio::osc::JoinInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::osc::JoinInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Encoder_OSC" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit EncoderNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~EncoderNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

public:
	virtual bool initialise() Q_DECL_OVERRIDE;
	virtual bool deinitialise() Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual QList<QUuid> pinAddTypesInput() const Q_DECL_OVERRIDE;

	virtual bool canAcceptPin( fugio::PinInterface *pPin ) const Q_DECL_OVERRIDE;

	virtual QUuid pinAddControlUuid(fugio::PinInterface *pPin) const Q_DECL_OVERRIDE;

	//-------------------------------------------------------------------------
	// OutputInterface interface

public:
	virtual void oscJoin( QStringList pPath, const QVariant &pValue ) Q_DECL_OVERRIDE;

	virtual void oscPath( QStringList &pPath ) const Q_DECL_OVERRIDE;

private slots:
	void contextFrameFinalise( qint64 pTimeStamp );

private:
	static void oscMessage(QByteArray &pMessage, const QString &pName, const QString &pArgs, const char *pBuffer, qint32 pBufLen);
	static void addData(QByteArray &pBundle, const QByteArray &pPacket);
	static void addData(QByteArray &pBundle, const QString &pName, const QString &pArgs, const char *pBuffer, qint32 pBufLen);
	static void buffer(QByteArray &pArray);

	void output( const QString &pAddress, const QVariant &pValue );

private:
	QSharedPointer<fugio::PinInterface>			 mPinInputNamespace;

	QSharedPointer<fugio::PinInterface>			 mPinOutputData;
	fugio::VariantInterface						*mValOutputData;

	QHash<QString,QVariant>						 mDataOutput;
};

#endif // ENCODERNODE_H
