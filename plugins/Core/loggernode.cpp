#include "loggernode.h"

#include <QJsonDocument>

#include <fugio/pin_variant_iterator.h>

LoggerNode::LoggerNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_STRING, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );

	mPinInputString = pinInput( "String", PIN_INPUT_STRING );
}

void LoggerNode::inputsUpdated( qint64 pTimeStamp )
{
	if( mPinInputString->isUpdated( pTimeStamp ) )
	{
		fugio::PinVariantIterator	I( mPinInputString );

		for( int i = 0 ; i < I.count() ; i++ )
		{
			QVariant	V = I.index( i );

			switch( QMetaType::Type( V.type() ) )
			{
				case QMetaType::QString:
					qInfo().noquote() << V.toString();
					break;

				case QMetaType::QJsonDocument:
					qInfo().noquote() << QString::fromLatin1( V.toJsonDocument().toJson() );
					break;

				default:
					{
						QString	S = V.toString();

						if( !S.isEmpty() )
						{
							qInfo().noquote() << S;
						}
					}
					break;
			}
		}
	}
}
