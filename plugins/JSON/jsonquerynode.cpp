#include "jsonquerynode.h"

#include <fugio/core/uuid.h>
#include <fugio/json/uuid.h>

#include <QJsonArray>

#include <fugio/pin_variant_iterator.h>

JsonQueryNode::JsonQueryNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_JSON,			"B8397BEA-CCB5-459B-B952-0C14B1527E0C" );
	FUGID( PIN_INPUT_QUERY,			"78EE5967-367D-4B6A-9876-27EBD1539800" );
	FUGID( PIN_OUTPUT_RESULTS,		"87DF03AA-D3DE-4F44-8201-AB5580585481" );

	mPinInputJson  = pinInput( "JSON", PIN_INPUT_JSON );
	mPinInputQuery = pinInput( "Query", PIN_INPUT_QUERY );

	mValOutputResults = pinOutput<fugio::VariantInterface *>( "Results", mPinOutputResults, PID_VARIANT, PIN_OUTPUT_RESULTS );
}

void JsonQueryNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	bool	UpdateOutput = mPinOutputResults->alwaysUpdate();

	QVariant	JsonVariant = variant( mPinInputJson );

	QJsonDocument	Json;

	switch( QMetaType::Type( JsonVariant.type() ) )
	{
		case QMetaType::QJsonDocument:
			Json = JsonVariant.toJsonDocument();
			break;

		case QMetaType::QString:
			Json = QJsonDocument::fromJson( JsonVariant.toString().toLatin1() );
			break;

		case QMetaType::QByteArray:
			Json = QJsonDocument::fromJson( JsonVariant.toByteArray() );
			break;

		default:
			break;

	}

	if( Json.isEmpty() )
	{
		mValOutputResults->setVariantType( QMetaType::QJsonDocument );

		mValOutputResults->setVariant( QJsonDocument() );

		UpdateOutput = true;
	}
	else if( JsonVariant.isValid() )
	{
		// Quick and dirty query

		QStringList		SL = variant<QString>( mPinInputQuery ).split( QRegExp( "[\\[\\.]" ), QString::SkipEmptyParts );

		if( !SL.isEmpty() )
		{
			JsonQuery			JQ( Json, SL );

			mValOutputResults->setVariantType( QMetaType::QJsonDocument );

			mValOutputResults->setVariant( QJsonDocument( JQ.mRV ) );

			UpdateOutput = true;
		}
	}

	pinUpdated( mPinOutputResults, UpdateOutput );
}

JsonQueryNode::JsonQuery::JsonQuery( QJsonDocument &JD, QStringList &SL )
	: mJD( JD )
{
	if( mJD.isObject() )
	{
		parseObject( JD.object(), SL );
	}
	else if( JD.isArray() )
	{
		parseArray( JD.array(), SL );
	}
}

void JsonQueryNode::JsonQuery::parseObject(const QJsonObject &JO, QStringList SL)
{
	QString	K = SL.takeFirst();

	if( K == QStringLiteral( "*" ) )
	{
		for( QJsonObject::const_iterator it = JO.begin() ; it != JO.end() ; it++ )
		{
			if( SL.isEmpty() )
			{
				mRV << it.value();
			}
			else
			{
				parseValue( it.value(), SL );
			}
		}
	}
	else
	{
		for( QJsonObject::const_iterator it = JO.constBegin() ; it != JO.constEnd() ; it++ )
		{
			if( it.key() != K )
			{
				continue;
			}

			if( SL.isEmpty() )
			{
				mRV << it.value();
			}
			else
			{
				parseValue( it.value(), SL );
			}
		}
	}
}

void JsonQueryNode::JsonQuery::parseArray(const QJsonArray &JA, QStringList SL)
{
	QString	K = SL.takeFirst();

	if( K == QStringLiteral( "*" ) )
	{
		for( int i = 0 ; i < JA.count() ; i++ )
		{
			if( SL.isEmpty() )
			{
				mRV << JA.at( i );
			}
			else
			{
				parseValue( JA.at( i ), SL );
			}
		}
	}
	else if( K.endsWith( ']' ) )
	{
		K = K.mid( 0, K.length() - 1 ).trimmed();

		if( K.contains( '=' ) )
		{
			QStringList		Parts = K.split( '=' );

			if( Parts.size() == 2 )
			{
				for( int i = 0 ; i < Parts.size() ; i++ )
				{
					Parts[ i ] = Parts[ i ].trimmed();
				}

				QJsonValue	CmpVal;

				if( Parts[ 1 ] == "true" )
				{
					CmpVal = true;
				}
				else if( Parts[ 1 ] == "false" )
				{
					CmpVal = false;
				}
				else if( Parts[ 1 ].startsWith( '\'' ) && Parts[ 1 ].endsWith( '\'' ) )
				{
					CmpVal = Parts[ 1 ].mid( 1, Parts[ 1 ].size() - 2 );
				}
				else if( Parts[ 1 ].contains( '.' ) )
				{
					CmpVal = Parts[ 1 ].toDouble();
				}
				else
				{
					CmpVal = Parts[ 1 ].toInt();
				}

				if( !Parts[ 0 ].isEmpty() && !Parts[ 1 ].isEmpty() )
				{
					for( int i = 0 ; i < JA.count() ; i++ )
					{
						QJsonValue	V = JA.at( i );

						if( V.isObject() )
						{
							QJsonObject	O = V.toObject();
							QJsonObject::const_iterator	it = O.constFind( Parts[ 0 ] );

							if( it != O.constEnd() )
							{
								if( it.value() == CmpVal )
								{
									if( SL.isEmpty() )
									{
										mRV << V;
									}
									else
									{
										parseValue( V, SL );
									}
								}
							}
						}
					}
				}
			}
		}
		else
		{
			bool	OK;
			int		i = K.toInt( &OK );

			if( OK && i >= 0 && JA.count() > i )
			{
				if( SL.isEmpty() )
				{
					mRV << JA.at( i );
				}
				else
				{
					parseValue( JA.at( i ), SL );
				}
			}
		}
	}
}

void JsonQueryNode::JsonQuery::parseValue(const QJsonValue &JV, QStringList SL)
{
	if( JV.isObject() )
	{
		parseObject( JV.toObject(), SL );
	}
	else if( JV.isArray() )
	{
		parseArray( JV.toArray(), SL );
	}
}
