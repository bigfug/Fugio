#include "vector3node.h"

#include <fugio/core/uuid.h>
#include <fugio/math/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/core/variant_interface.h>

#include <QFormLayout>
#include <QDoubleSpinBox>
#include <QVector3D>
#include <QSettings>

Vector3Node::Vector3Node( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mVec3 = pinOutput<fugio::VariantInterface *>( "Vector3", mPinVec3, PID_VECTOR3 );
}

QWidget *Vector3Node::gui()
{
	QWidget		*W = new QWidget();

	QFormLayout	*L = new QFormLayout();

	QDoubleSpinBox	*X = new QDoubleSpinBox( W );
	QDoubleSpinBox	*Y = new QDoubleSpinBox( W );
	QDoubleSpinBox	*Z = new QDoubleSpinBox( W );

	X->setRange( -1000000000, 1000000000 );
	Y->setRange( -1000000000, 1000000000 );
	Z->setRange( -1000000000, 1000000000 );

	QVector3D		 V = mVec3->variant().value<QVector3D>();

	X->setValue( V.x() );
	Y->setValue( V.y() );
	Z->setValue( V.z() );

	L->addRow( "X:", X );
	L->addRow( "Y:", Y );
	L->addRow( "Z:", Z );

	W->setLayout( L );

	L->setContentsMargins( 0, 0, 0, 0 );

	connect( X, SIGNAL(valueChanged(double)), this, SLOT(updateX(double)) );
	connect( Y, SIGNAL(valueChanged(double)), this, SLOT(updateY(double)) );
	connect( Z, SIGNAL(valueChanged(double)), this, SLOT(updateZ(double)) );

	connect( this, SIGNAL(updatedX(double)), X, SLOT(setValue(double)) );
	connect( this, SIGNAL(updatedY(double)), Y, SLOT(setValue(double)) );
	connect( this, SIGNAL(updatedZ(double)), Z, SLOT(setValue(double)) );

	return( W );
}

void Vector3Node::updateX(double pValue)
{
	QVector3D		 V = mVec3->variant().value<QVector3D>();

	if( V.x() == pValue )
	{
		return;
	}

	V.setX( pValue );

	mVec3->setVariant( V );

	pinUpdated( mPinVec3 );

	emit updatedX( pValue );
}

void Vector3Node::updateY(double pValue)
{
	QVector3D		 V = mVec3->variant().value<QVector3D>();

	if( V.y() == pValue )
	{
		return;
	}

	V.setY( pValue );

	mVec3->setVariant( V );

	pinUpdated( mPinVec3 );

	emit updatedY( pValue );
}

void Vector3Node::updateZ(double pValue)
{
	QVector3D		 V = mVec3->variant().value<QVector3D>();

	if( V.z() == pValue )
	{
		return;
	}

	V.setZ( pValue );

	mVec3->setVariant( V );

	pinUpdated( mPinVec3 );

	emit updatedZ( pValue );
}

void Vector3Node::loadSettings(QSettings &pSettings)
{
	mVec3->setVariant( pSettings.value( "v", mVec3->variant() ) );

	pinUpdated( mPinVec3 );
}

void Vector3Node::saveSettings( QSettings &pSettings ) const
{
	pSettings.setValue( "v", mVec3->variant() );
}

void Vector3Node::inputsUpdated( qint64 pTimeStamp )
{
	Q_UNUSED( pTimeStamp )

	QVector3D		 V = mVec3->variant().value<QVector3D>();

	emit updatedX( V.x() );
	emit updatedY( V.y() );
	emit updatedZ( V.z() );
}
