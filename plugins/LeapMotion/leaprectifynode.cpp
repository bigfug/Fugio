#include "leaprectifynode.h"

#include <math.h>
#include <float.h>

#include <fugio/core/uuid.h>
#include <fugio/image/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/image/image.h>
#include <fugio/core/variant_interface.h>
#include <fugio/performance.h>

LeapRectifyNode::LeapRectifyNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mPinInputImage = pinInput( "Image" );

	mPinInputDistortion = pinInput( "Distortion" );

	mPinInputWidth = pinInput( "Width" );

	mPinInputHeight = pinInput( "Height" );

	mValOutput = pinOutput<fugio::VariantInterface *>( "Image", mPinOutput, PID_IMAGE );
}

//float truncf(float x)
//{
//  float y;
//  if (x < 0.0L)
//	{
//	  y = -(1.0L / FLT_EPSILON - x - 1.0 / FLT_EPSILON);
//	  if (y < x)
//		y = y + 1.0L;
//	}
//  else
//	{
//	  y = 1.0L / FLT_EPSILON + x - 1.0 / FLT_EPSILON;
//	  if (y > x)
//		y = y - 1.0L;
//	}

//  return y;
//}

void LeapRectifyNode::inputsUpdated( qint64 pTimeStamp )
{
	fugio::Performance	Perf( mNode, "inputsUpdated", pTimeStamp );

	if( !pTimeStamp )
	{
		return;
	}

	QSize						 S;

	fugio::VariantInterface		*V;

	if( mPinInputWidth->isConnected() && ( V = input<fugio::VariantInterface *>( mPinInputWidth ) ) )
	{
		S.setWidth( V->variant().toInt() );
	}
	else
	{
		S.setWidth( mPinInputWidth->value().toInt() );
	}

	if( mPinInputHeight->isConnected() && ( V = input<fugio::VariantInterface *>( mPinInputHeight ) ) )
	{
		S.setHeight( V->variant().toInt() );
	}
	else
	{
		S.setHeight( mPinInputHeight->value().toInt() );
	}

	if( !S.isValid() || S.isEmpty() )
	{
		return;
	}

	fugio::Image	DstImg = mValOutput->variant().value<fugio::Image>();

	if( S != DstImg.size() )
	{
		DstImg.setFormat( fugio::ImageFormat::GRAY8 );

		DstImg.setSize( S.width(), S.height() );

		DstImg.setLineSize( 0, S.width() );

		memset( DstImg.internalBuffer( 0 ), 0, DstImg.bufferSize( 0 ) );
	}

	if( !mPinInputImage->isConnected() || !mPinInputImage->isUpdated( pTimeStamp ) )
	{
		return;
	}

	if( !mPinInputDistortion->isConnected() )
	{
		return;
	}

	fugio::Image	SRC = variant<fugio::Image>( mPinInputImage );
	fugio::Image	DST = variant<fugio::Image>( mPinInputDistortion );

	if( !SRC.isValid() || !DST.isValid() )
	{
		return;
	}

	float destinationWidth = S.width();
	float destinationHeight = S.height();
	unsigned char *destination = DstImg.internalBuffer( 0 );

	//define needed variables outside the inner loop
	float calibrationX, calibrationY;
	float weightX, weightY;
	float dX, dX1, dX2, dX3, dX4;
	float dY, dY1, dY2, dY3, dY4;
	int x1, x2, y1, y2;
	int denormalizedX, denormalizedY;
	int i, j;

	const unsigned char* raw = SRC.buffer( 0 );
	const float* distortion_buffer = (const float *)DST.buffer( 0 );

	//Local variables for values needed in loop
	const int distortionWidth = DST.size().width() * 2;
	const int width = SRC.size().width();
	const int height = SRC.size().height();

	for (i = 0; i < destinationWidth; i++) {
		for (j = 0; j < destinationHeight; j++) {
			//Calculate the position in the calibration map (still with a fractional part)
			calibrationX = 63.0f * float(i)/destinationWidth;
			calibrationY = 62.0f * (1.0f - float(j)/destinationHeight); // The y origin is at the bottom
			//Save the fractional part to use as the weight for interpolation
			weightX = calibrationX - floorf(calibrationX);
			weightY = calibrationY - floorf(calibrationY);

			//Get the x,y coordinates of the closest calibration map points to the target pixel
			x1 = calibrationX; //Note truncation to int
			y1 = calibrationY;
			x2 = x1 + 1;
			y2 = y1 + 1;

			//Look up the x and y values for the 4 calibration map points around the target
			dX1 = distortion_buffer[x1 * 2 + y1 * distortionWidth];
			dX2 = distortion_buffer[x2 * 2 + y1 * distortionWidth];
			dX3 = distortion_buffer[x1 * 2 + y2 * distortionWidth];
			dX4 = distortion_buffer[x2 * 2 + y2 * distortionWidth];
			dY1 = distortion_buffer[x1 * 2 + y1 * distortionWidth + 1];
			dY2 = distortion_buffer[x2 * 2 + y1 * distortionWidth + 1];
			dY3 = distortion_buffer[x1 * 2 + y2 * distortionWidth + 1];
			dY4 = distortion_buffer[x2 * 2 + y2 * distortionWidth + 1];

			//Bilinear interpolation of the looked-up values:
			// X value
			dX = dX1 * (1 - weightX) * (1 - weightY) +
				 dX2 * weightX * (1 - weightY) +
				 dX3 * (1 - weightX) * weightY +
				 dX4 * weightX * weightY;

			// Y value
			dY = dY1 * (1 - weightX) * (1 - weightY) +
				 dY2 * weightX * (1 - weightY) +
				 dY3 * (1 - weightX) * weightY +
				 dY4 * weightX * weightY;

			// Reject points outside the range [0..1]
			if((dX >= 0) && (dX <= 1) && (dY >= 0) && (dY <= 1)) {
				//Denormalize from [0..1] to [0..width] or [0..height]
				denormalizedX = dX * width;
				denormalizedY = dY * height;

				//look up the brightness value for the target pixel
				destination[i+j*S.width()] = raw[denormalizedX + denormalizedY * width];
			} else {
				destination[i+j*S.width()] = -1;
			}
		}
	}

	mNode->context()->pinUpdated( mPinOutput );
}
