#include "qmlnode.h"

#include "qmlinterfacenode.h"

#include <fugio/pin_interface.h>
#include <fugio/pin_signals.h>
#include <fugio/node_signals.h>

#include "qmlpin.h"

QMLNode::QMLNode( QSharedPointer<fugio::NodeInterface> pNode )
	: mNode( pNode )
{
	connect( mNode->qobject(), SIGNAL(nameChanged(QString)), this, SLOT(setName(QString)) );
}

QObject *QMLNode::input( const QString &pName )
{
	QSharedPointer<fugio::PinInterface>		 P = mNode->findInputPinByName( pName );

	return( P ? new QMLPin( P ) : nullptr );
}

QObject *QMLNode::output( const QString &pName )
{
	QSharedPointer<fugio::PinInterface>		 P = mNode->findOutputPinByName( pName );

	return( P ? new QMLPin( P ) : nullptr );
}

void QMLNode::pinUpdated( const QString &pName )
{
	QSharedPointer<fugio::PinInterface>		P = mNode->findOutputPinByName( pName );

	if( P )
	{
		mNode->context()->pinUpdated( P );
	}
}

void QMLNode::pinUpdated( QObject *pObject )
{
	QMLPin			*P = qobject_cast<QMLPin *>( pObject );

	if( P )
	{
		pinUpdated( P->name() );
	}
}

QQmlListProperty<QMLPin> QMLNode::outputs()
{
	return( QQmlListProperty<QMLPin>( this, this, &QMLNode::outputCount, &QMLNode::outputIndex ) );
}

QObject *QMLNode::control() const
{
	return( mNode->control()->qobject() );
}

QString QMLNode::name() const
{
	return( mNode->name() );
}

void QMLNode::signalInputsUpdated( qint64 pTimeStamp )
{
	emit inputsUpdated( pTimeStamp );
}

void QMLNode::setName( QString name )
{
	mNode->setName( name );
}

void QMLNode::signalNameChanged( QString name )
{
	emit nameChanged( name );
}

QMLPin *QMLNode::outputIndex( QQmlListProperty<QMLPin> *pListProp, int pIndex )
{
	return( new QMLPin( reinterpret_cast<QMLNode *>( pListProp->data )->mNode->enumOutputPins().at( pIndex ) ) );
}

int QMLNode::outputCount( QQmlListProperty<QMLPin> *pListProp )
{
	return( reinterpret_cast<QMLNode *>( pListProp->data )->mNode->enumOutputPins().size() );
}
