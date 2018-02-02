#ifndef TEXTEDITORREMOTENODE_H
#define TEXTEDITORREMOTENODE_H

#include <QObject>
#include <QByteArray>
#include <QBuffer>
#include <QDataStream>

#include <fugio/nodecontrolbase.h>

#include <fugio/text/syntax_error_interface.h>

class TextEditorRemoteNode : public fugio::NodeControlBase
{
	Q_OBJECT

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Text_Editor_Remote" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit TextEditorRemoteNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~TextEditorRemoteNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual bool initialise() Q_DECL_OVERRIDE;
	virtual bool deinitialise() Q_DECL_OVERRIDE;

	virtual void loadSettings( QSettings &pSettings ) Q_DECL_OVERRIDE;
	virtual void saveSettings( QSettings &pSettings ) const Q_DECL_OVERRIDE;
	
	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

private:
	void processIncomingVariant( const QVariant &pV );
	void processIncomingPacket( const QByteArray &B );

	void sendText( const QString &pText );

	void sendHighlighter( const QUuid &pUuid );

	void receiveTextPacket( QDataStream &DS );
	void receiveHighlighter( QDataStream &DS );
	void receiveErrors( QDataStream &DS );

	enum
	{
		PACKET_UNKNOWN = 0,
		PACKET_TEXT,
		PACKET_HIGHLIGHTER,
		PACKET_ERRORS
	};

	class Packet
	{
	public:
		Packet( quint16 pPacketType )
			: B( &A ), D( &B )
		{
			if( B.open( QBuffer::WriteOnly ) )
			{
				D.setDevice( &B );

				D << pPacketType;
			}
		}

		inline bool isEmpty( void ) const
		{
			return( A.isEmpty() );
		}

		QDataStream &stream( void )
		{
			return( D );
		}

		QByteArray array( void ) const
		{
			return( A );
		}

	private:
		QByteArray		A;
		QBuffer			B;
		QDataStream		D;
	};

	void appendPacket( const Packet &P );

private slots:
	void textLinked( QSharedPointer<fugio::PinInterface> pPin );
	void textUninked( QSharedPointer<fugio::PinInterface> pPin );

	void syntaxErrorsUpdated( QList<fugio::SyntaxError> pSyntaxErrors );

private:
	QSharedPointer<fugio::PinInterface>			 mPinInputPackets;

	QSharedPointer<fugio::PinInterface>			 mPinInputText;
	fugio::SyntaxErrorInterface					*mValInputText;

	QSharedPointer<fugio::PinInterface>			 mPinOutputPackets;
	fugio::VariantInterface						*mValOutputPackets;

	QSharedPointer<fugio::PinInterface>			 mPinOutputText;
	fugio::VariantInterface						*mValOutputText;

	qint64										 mLastTime;
};

#endif // TEXTEDITORREMOTENODE_H
