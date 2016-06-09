#include "slidernode.h"

#include <QSlider>
#include <QSettings>

SliderNode::SliderNode(QSharedPointer<fugio::NodeInterface> pNode)
	: NodeControlBase( pNode )
{
	mValue = pinOutput<fugio::VariantInterface *>( "Number", mPinValue, PID_FLOAT );
}

SliderNode::~SliderNode()
{

}


QWidget *SliderNode::gui()
{
	QSlider		*Slider = new QSlider();

	Slider->setOrientation( Qt::Horizontal );

	Slider->setMinimum( 0 );
	Slider->setMaximum( 1000 );

	Slider->setValue( int( mValue->variant().toFloat() * 1000.0f ) );

	connect( Slider, SIGNAL(sliderMoved(int)), this, SLOT(sliderValueChanged(int)) );

	connect( this, SIGNAL(valueChanged(int)), Slider, SLOT(setValue(int)) );

	return( Slider );
}

void SliderNode::sliderValueChanged( int pValue )
{
	float		NewVal = float( pValue ) / 1000.0f;

	if( NewVal != mValue->variant().toFloat() )
	{
		mValue->setVariant( NewVal );

		pinUpdated( mPinValue );
	}
}

void SliderNode::loadSettings( QSettings &pSettings)
{
	float		NewVal = pSettings.value( "value", mValue->variant() ).toFloat();

	if( NewVal != mValue->variant().toFloat() )
	{
		mValue->setVariant( NewVal );

		pinUpdated( mPinValue );
	}

	emit valueChanged( int( NewVal * 1000.0f ) );
}

void SliderNode::saveSettings( QSettings &pSettings )
{
	pSettings.setValue( "value", mValue->variant().toFloat() );
}
