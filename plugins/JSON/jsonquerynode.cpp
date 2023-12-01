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
	FUGID( PIN_INPUT_FORMAT,		"3d995a98-0f56-4371-a1fa-67ecaefeba1c" );
	FUGID( PIN_OUTPUT_RESULTS,		"87DF03AA-D3DE-4F44-8201-AB5580585481" );

	mPinInputJson  = pinInput( "JSON", PIN_INPUT_JSON );
	mPinInputQuery = pinInput( "Query", PIN_INPUT_QUERY );

	mValInputFormat = pinInput<fugio::ChoiceInterface *>( tr( "Format" ), mPinInputFormat, PID_CHOICE, PIN_INPUT_FORMAT );

	mValInputFormat->setChoices( QStringList() << "JSON" << "Variant" );

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

			if( variant<QString>( mPinInputFormat ) == "Variant" )
			{
				QJsonValue::Type  ValueType = QJsonValue::Undefined;
				bool		      AllSame   = true;

				for( const QJsonValue V : JQ.mRV )
				{
					if( ValueType == QJsonValue::Undefined )
					{
						ValueType = V.type();
						AllSame   = true;
					}
					else if( ValueType != V.type() )
					{
						AllSame = false;
					}
				}

				if( AllSame )
				{
					mValOutputResults->setVariantType( QMetaType::UnknownType );
					mValOutputResults->setVariantCount( 0 );

					switch( ValueType )
					{
						case QJsonValue::Bool:
							mValOutputResults->setVariantType( QMetaType::Bool );
							mValOutputResults->setVariantCount( JQ.mRV.count() );

							for( int i = 0 ; i < JQ.mRV.count() ; i++ )
							{
								mValOutputResults->setVariant( i, JQ.mRV.at( i ).toBool() );
							}
							break;

						case QJsonValue::Double:
							mValOutputResults->setVariantType( QMetaType::Double );
							mValOutputResults->setVariantCount( JQ.mRV.count() );

							for( int i = 0 ; i < JQ.mRV.count() ; i++ )
							{
								mValOutputResults->setVariant( i, JQ.mRV.at( i ).toDouble() );
							}
							break;

						case QJsonValue::String:
							mValOutputResults->setVariantType( QMetaType::QString );
							mValOutputResults->setVariantCount( JQ.mRV.count() );

							for( int i = 0 ; i < JQ.mRV.count() ; i++ )
							{
								mValOutputResults->setVariant( i, JQ.mRV.at( i ).toString() );
							}
							break;

						case QJsonValue::Array:
							{
								QJsonArray      OD;

								mValOutputResults->setVariantType( QMetaType::QJsonDocument );
								mValOutputResults->setVariantCount( 1 );

								for( int i = 0 ; i < JQ.mRV.count() ; i++ )
								{
									OD << JQ.mRV.at( i ).toArray();
								}

								mValOutputResults->setVariant( QJsonDocument( OD ) );
							}
							break;

						case QJsonValue::Object:
							{
								QJsonArray      OD;

								mValOutputResults->setVariantType( QMetaType::QJsonDocument );
								mValOutputResults->setVariantCount( 1 );

								for( int i = 0 ; i < JQ.mRV.count() ; i++ )
								{
									OD << JQ.mRV.at( i ).toObject();
								}
								mValOutputResults->setVariantType( QMetaType::QJsonDocument );

								mValOutputResults->setVariant( QJsonDocument( OD ) );
							}
							break;

						default:
							break;
					}

					UpdateOutput = true;
				}
				else
				{
					mValOutputResults->setVariantType( QMetaType::QJsonDocument );

					mValOutputResults->setVariantCount( 1 );

					mValOutputResults->setVariant( QJsonDocument( JQ.mRV ) );

					UpdateOutput = true;
				}
			}
			else
			{
				mValOutputResults->setVariantType( QMetaType::QJsonDocument );

				mValOutputResults->setVariantCount( 1 );

				mValOutputResults->setVariant( QJsonDocument( JQ.mRV ) );

				UpdateOutput = true;
			}
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
					QJsonObject::const_iterator	it = JO.constFind( Parts[ 0 ] );

					if( it != JO.constEnd() )
					{
						if( it.value() == CmpVal )
						{
							if( SL.isEmpty() )
							{
								mRV << JO;
							}
							else
							{
								parseValue( JO, SL );
							}
						}
					}
				}
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
