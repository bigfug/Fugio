#include "rabbitnode.h"

#include <fugio/core/uuid.h>

#include <fugio/core/variant_interface.h>
#include <fugio/pin_variant_iterator.h>
#include <fugio/context_signals.h>

RabbitNode::RabbitNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_DATA,		"9e154e12-bcd8-4ead-95b1-5a59833bcf4e" )
	FUGID( PIN_OUTPUT_DATA,		"1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" )

	mPinInputData = pinInput( tr( "Input" ), PIN_INPUT_DATA );

	mValOutputData = pinOutput<fugio::VariantInterface *>( tr( "Output" ), mPinOutputData, PID_BYTEARRAY, PIN_OUTPUT_DATA );
}

bool RabbitNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	mParamServer.addTransporter( *this );

	connect( mNode->qobject(), SIGNAL(pinAdded(QSharedPointer<fugio::NodeInterface>,QSharedPointer<fugio::PinInterface>)), this, SLOT(pinAdded(QSharedPointer<fugio::NodeInterface>,QSharedPointer<fugio::PinInterface>)) );
	connect( mNode->qobject(), SIGNAL(pinRemoved(QSharedPointer<fugio::NodeInterface>,QSharedPointer<fugio::PinInterface>)), this, SLOT(pinRemoved(QSharedPointer<fugio::NodeInterface>,QSharedPointer<fugio::PinInterface>)) );

	connect( mNode->qobject(), SIGNAL(pinLinked(QSharedPointer<fugio::PinInterface>, QSharedPointer<fugio::PinInterface>)), this, SLOT(pinLinked(QSharedPointer<fugio::PinInterface>, QSharedPointer<fugio::PinInterface>)) );
	connect( mNode->qobject(), SIGNAL(pinUnlinked(QSharedPointer<fugio::PinInterface>, QSharedPointer<fugio::PinInterface>)), this, SLOT(pinUnlinked(QSharedPointer<fugio::PinInterface>, QSharedPointer<fugio::PinInterface>)) );

	connect( mNode->context()->qobject(), SIGNAL(frameFinalise()), this, SLOT(frameFinalise()) );

	return( true );
}

void RabbitNode::inputsUpdated( qint64 pTimeStamp )
{
	fugio::NodeControlBase::inputsUpdated( pTimeStamp );

	if( mPinInputData->isUpdated( pTimeStamp ) )
	{
		fugio::PinVariantIterator	Input( mPinInputData );

		for( int i = 0 ; i < Input.count() ; i++ )
		{
			QVariant		V = Input.index( i );
			QByteArray		A = V.toByteArray();
			std::istringstream	S( A.toStdString() );

			rcp::Option<rcp::Packet> PacketOption = rcp::Packet::parse( S );

			if( PacketOption.hasValue() )
			{
				rcp::Packet		Packet = PacketOption.getValue();

				switch( Packet.getCommand() )
				{
					case COMMAND_INVALID:
					case COMMAND_MAX_:
					case COMMAND_DISCOVER:
					case COMMAND_INITIALIZE:
					case COMMAND_UPDATEVALUE:
						break;

					case COMMAND_INFO:
						break;

					case COMMAND_UPDATE:
						if( Packet.hasData() )
						{
							rcp::ParameterPtr	p = std::dynamic_pointer_cast<rcp::IParameter>( Packet.getData() );

							if( p )
							{
								QSharedPointer<fugio::PinInterface>	PO = mNode->findPinByLocalId( mOutputParamMap.key( p->getId() ) );

								if( PO && PO->hasControl() )
								{
									fugio::VariantInterface		*VO = qobject_cast<fugio::VariantInterface *>( PO->control()->qobject() );

									QVariant					 NewVar;

									switch( p->getDatatype() )
									{
										case DATATYPE_STRING:
											{
												auto PP = std::dynamic_pointer_cast<rcp::StringParameter>( p );

												NewVar.setValue( QString::fromStdString( PP->getValue() ) );
											}
											break;

										case DATATYPE_FLOAT32:
											{
												auto PP = std::dynamic_pointer_cast<rcp::Float32Parameter>( p );

												NewVar.setValue( PP->getValue() );
											}
											break;

										default:
											break;
									}

									if( VO && !NewVar.isNull() )
									{
										if( VO->variant() != NewVar )
										{
											VO->setVariant( NewVar );

											pinUpdated( PO );
										}

									}
								}
							}
						}
						break;

					case COMMAND_REMOVE:
						break;
				}
			}
		}
	}

	for( QSharedPointer<fugio::PinInterface> PI : mNode->enumInputPins() )
	{
		if( PI->localId() == mPinInputData->localId() )
		{
			continue;
		}

		if( !PI->isUpdated( pTimeStamp ) )
		{
			continue;
		}

		QVariant		PinVar = variant( PI );

		QSharedPointer<fugio::PinInterface> PO = mNode->findPinByLocalId( PI->pairedUuid() );

		if( PO )
		{
			rcp::ParameterPtr	P = mParamServer.getParameter( mOutputParamMap.value( PO->localId(), -1 ) );

			if( P )
			{
				switch( P->getDatatype() )
				{
					case DATATYPE_STRING:
						{
							auto	p = std::dynamic_pointer_cast<rcp::StringParameter>( P );

							p->setValue( PinVar.toString().toStdString() );
						}
						break;

					case DATATYPE_FLOAT32:
						{
							auto	p = std::dynamic_pointer_cast<rcp::Float32Parameter>( P );

							p->setValue( PinVar.toFloat() );
						}
						break;

					default:
						break;
				}

				fugio::VariantInterface		*VO = qobject_cast<fugio::VariantInterface *>( PO->control()->qobject() );

				if( VO && VO->variant() != PinVar )
				{
					VO->setVariant( PinVar );

					pinUpdated( PO );
				}
			}
		}
	}
}

bool RabbitNode::canAcceptPin( fugio::PinInterface *pPin ) const
{
	Q_UNUSED( pPin )

	return( false );
}

bool RabbitNode::pinShouldAutoRename(fugio::PinInterface *pPin) const
{
	return( pPin->localId() != mPinInputData->localId() );
}

QUuid RabbitNode::pairedPinControlUuid( QSharedPointer<fugio::PinInterface> pPin ) const
{
	fugio::VariantInterface	*PI = input<fugio::VariantInterface *>( pPin );

	return( PI ? PI->variantPinControl() : PID_VARIANT );
}

void RabbitNode::pinAdded( QSharedPointer<fugio::NodeInterface> pNode, QSharedPointer<fugio::PinInterface> pPin )
{
	Q_UNUSED( pNode )

	if( pPin->direction() == PIN_OUTPUT )
	{
		rcp::StringParameter	&sp = mParamServer.createStringParameter( pPin->name().toStdString() );

		mParamServer.addParameter( sp );

		mOutputParamMap.insert( pPin->localId(), sp.getId() );
	}
}

void RabbitNode::pinRemoved(QSharedPointer<fugio::NodeInterface> pNode, QSharedPointer<fugio::PinInterface> pPin)
{

}

void RabbitNode::pinLinked(QSharedPointer<fugio::PinInterface> pPinSrc, QSharedPointer<fugio::PinInterface> pPinDst)
{

}

void RabbitNode::pinUnlinked(QSharedPointer<fugio::PinInterface> pPinSrc, QSharedPointer<fugio::PinInterface> pPinDst)
{

}

void RabbitNode::frameFinalise()
{
	mValOutputData->variantClear();

	mParamServer.update();

	if( mValOutputData->variantCount() > 0 )
	{
		pinUpdated( mPinOutputData );
	}
}


void RabbitNode::bind(int port)
{
	Q_UNUSED( port )
}

void RabbitNode::unbind()
{
}

void RabbitNode::sendToOne(std::istream &data, void *id)
{
	Q_UNUSED( id )
}

void RabbitNode::sendToAll(std::istream &data, void *excludeId)
{
	Q_UNUSED( excludeId )

	QByteArray		A;
	char			B[ 256 ];

	while( !data.eof() )
	{
		int			C = data.read( B, sizeof( B ) ).gcount();

		A.append( B, C );
	}

	mValOutputData->variantAppend( A );
}

int RabbitNode::getConnectionCount()
{
	return( 1 );
}

QList<QUuid> RabbitNode::pinAddTypesOutput() const
{
	static QList<QUuid> PinLst =
	{
		PID_FLOAT,
		PID_STRING
	};

	return( PinLst );
}
