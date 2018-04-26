#include "namespacepin.h"

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/osc/namespace_interface.h>
#include <fugio/core/uuid.h>
#include <fugio/osc/uuid.h>
#include <fugio/colour/uuid.h>

NamespacePin::NamespacePin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin )
{

}

QStringList NamespacePin::oscNamespace()
{
	fugio::NodeInterface				*NI;

	if( mPin->direction() == PIN_INPUT )
	{
		NI = mPin->connectedNode();
	}
	else
	{
		NI = mPin->node();
	}

	fugio::osc::NamespaceInterface		*JI = qobject_cast<fugio::osc::NamespaceInterface *>( NI && NI->hasControl() ? NI->control()->qobject() : nullptr );

	if( JI )
	{
		return( JI->oscNamespace() );
	}

	return( QStringList() );
}

QList<fugio::NodeControlInterface::AvailablePinEntry> NamespacePin::oscPins( const QStringList &pCurDir ) const
{
	QList<fugio::NodeControlInterface::AvailablePinEntry>		PinLst;

	for( QHash<QString,QVariant>::const_iterator it = mDataNames.begin() ; it != mDataNames.end() ; it++ )
	{
		QSharedPointer<fugio::PinInterface>		CurPin;

		QStringList		CurLst = it.key().split( '/', QString::SkipEmptyParts );

		if( !pCurDir.isEmpty() )
		{
			if( CurLst.size() <= pCurDir.size() )
			{
				continue;
			}

			bool			CurFnd = true;

			for( int i = 0 ; i < pCurDir.size() ; i++ )
			{
				if( CurLst.first() != pCurDir.at( i ) )
				{
					CurFnd = false;

					break;
				}

				CurLst.removeFirst();
			}

			if( !CurFnd )
			{
				continue;
			}
		}

		QStringList		NewLst = QStringList( CurLst.takeFirst() );

		while( true )
		{
			QString		CurNam = NewLst.join( '/' ).prepend( '/' );

			CurPin = mPin->node()->findOutputPinByName( CurNam );

			if( CurPin )
			{
				break;
			}

			fugio::NodeControlInterface::AvailablePinEntry		PE( "" );

			if( CurLst.isEmpty() )
			{
				QUuid		PinUid = PID_FLOAT;

				switch( QMetaType::Type( it.value().type() ) )
				{
					case QMetaType::Float:
					case QMetaType::Double:
						PinUid = PID_FLOAT;
						break;

					case QMetaType::Bool:
						PinUid = PID_BOOL;
						break;

					case QMetaType::Int:
					case QMetaType::Long:
						PinUid = PID_INTEGER;
						break;

					case QMetaType::QByteArray:
						PinUid = PID_BYTEARRAY;
						break;

					case QMetaType::QColor:
						PinUid = PID_COLOUR;
						break;

					case QMetaType::QString:
						PinUid = PID_STRING;
						break;

					case QMetaType::QVariantList:
						PinUid = PID_LIST;
						break;

					default:
						break;
				}

				PE = fugio::NodeControlInterface::AvailablePinEntry( CurNam, PinUid );
			}
			else
			{
				PE = fugio::NodeControlInterface::AvailablePinEntry( CurNam, PID_OSC_SPLIT );
			}

			if( !PinLst.contains( PE ) )
			{
				PinLst << PE;
			}

			if( CurLst.isEmpty() )
			{
				break;
			}

			NewLst.push_back( CurLst.takeFirst() );
		}
	}

	std::sort( PinLst.begin(), PinLst.end() );

	return( PinLst );
}

