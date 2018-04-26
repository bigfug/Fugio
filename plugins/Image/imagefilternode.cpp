#include "imagefilternode.h"

#include <fugio/core/uuid.h>
#include <fugio/image/uuid.h>

#include <fugio/image/image.h>
#include <fugio/colour/colour_interface.h>

#include <fugio/performance.h>

#define ChannelBlend_Normal(A,B)     ((quint8)(A))
#define ChannelBlend_Lighten(A,B)    ((quint8)((B > A) ? B:A))
#define ChannelBlend_Darken(A,B)     ((quint8)((B > A) ? A:B))
#define ChannelBlend_Multiply(A,B)   ((quint8)((A * B) / 255))
#define ChannelBlend_Average(A,B)    ((quint8)((A + B) / 2))
#define ChannelBlend_Add(A,B)        ((quint8)(min(255, (A + B))))
#define ChannelBlend_Subtract(A,B)   ((quint8)((A + B < 255) ? 0:(A + B - 255)))
#define ChannelBlend_Difference(A,B) ((quint8)(abs(A - B)))
#define ChannelBlend_Negation(A,B)   ((quint8)(255 - abs(255 - A - B)))
#define ChannelBlend_Screen(A,B)     ((quint8)(255 - (((255 - A) * (255 - B)) >> 8)))
#define ChannelBlend_Exclusion(A,B)  ((quint8)(A + B - 2 * A * B / 255))
#define ChannelBlend_Overlay(A,B)    ((quint8)((B < 128) ? (2 * A * B / 255):(255 - 2 * (255 - A) * (255 - B) / 255)))
#define ChannelBlend_SoftLight(A,B)  ((quint8)((B < 128)?(2*((A>>1)+64))*((float)B/255):(255-(2*(255-((A>>1)+64))*(float)(255-B)/255))))
#define ChannelBlend_HardLight(A,B)  (ChannelBlend_Overlay(B,A))
#define ChannelBlend_ColorDodge(A,B) ((quint8)((B == 255) ? B:min(255, ((A << 8 ) / (255 - B)))))
#define ChannelBlend_ColorBurn(A,B)  ((quint8)((B == 0) ? B:max(0, (255 - ((255 - A) << 8 ) / B))))
#define ChannelBlend_LinearDodge(A,B)(ChannelBlend_Add(A,B))
#define ChannelBlend_LinearBurn(A,B) (ChannelBlend_Subtract(A,B))
#define ChannelBlend_LinearLight(A,B)((quint8)(B < 128)?ChannelBlend_LinearBurn(A,(2 * B)):ChannelBlend_LinearDodge(A,(2 * (B - 128))))
#define ChannelBlend_VividLight(A,B) ((quint8)(B < 128)?ChannelBlend_ColorBurn(A,(2 * B)):ChannelBlend_ColorDodge(A,(2 * (B - 128))))
#define ChannelBlend_PinLight(A,B)   ((quint8)(B < 128)?ChannelBlend_Darken(A,(2 * B)):ChannelBlend_Lighten(A,(2 * (B - 128))))
#define ChannelBlend_HardMix(A,B)    ((quint8)((ChannelBlend_VividLight(A,B) < 128) ? 0:255))
#define ChannelBlend_Reflect(A,B)    ((quint8)((B == 255) ? B:min(255, (A * A / (255 - B)))))
#define ChannelBlend_Glow(A,B)       (ChannelBlend_Reflect(B,A))
#define ChannelBlend_Phoenix(A,B)    ((quint8)(min(A,B) - max(A,B) + 255))
#define ChannelBlend_Alpha(A,B,O)    ((quint8)(O * A + (1 - O) * B))
#define ChannelBlend_AlphaF(A,B,F,O) (ChannelBlend_Alpha(F(A,B),A,O))

#define ColorBlend_Buffer(T,A,B,M)      (T)[0] = ChannelBlend_##M((A)[0], (B)[0]), \
										(T)[1] = ChannelBlend_##M((A)[1], (B)[1]), \
										(T)[2] = ChannelBlend_##M((A)[2], (B)[2])

#define ColorBlend_Normal(T,A,B)        (ColorBlend_Buffer(T,A,B,Normal))
#define ColorBlend_Lighten(T,A,B)       (ColorBlend_Buffer(T,A,B,Lighten))
#define ColorBlend_Darken(T,A,B)        (ColorBlend_Buffer(T,A,B,Darken))
#define ColorBlend_Multiply(T,A,B)      (ColorBlend_Buffer(T,A,B,Multiply))
#define ColorBlend_Average(T,A,B)       (ColorBlend_Buffer(T,A,B,Average))
#define ColorBlend_Add(T,A,B)           (ColorBlend_Buffer(T,A,B,Add))
#define ColorBlend_Subtract(T,A,B)      (ColorBlend_Buffer(T,A,B,Subtract))
#define ColorBlend_Difference(T,A,B)    (ColorBlend_Buffer(T,A,B,Difference))
#define ColorBlend_Negation(T,A,B)      (ColorBlend_Buffer(T,A,B,Negation))
#define ColorBlend_Screen(T,A,B)        (ColorBlend_Buffer(T,A,B,Screen))
#define ColorBlend_Exclusion(T,A,B)     (ColorBlend_Buffer(T,A,B,Exclusion))
#define ColorBlend_Overlay(T,A,B)       (ColorBlend_Buffer(T,A,B,Overlay))
#define ColorBlend_SoftLight(T,A,B)     (ColorBlend_Buffer(T,A,B,SoftLight))
#define ColorBlend_HardLight(T,A,B)     (ColorBlend_Buffer(T,A,B,HardLight))
#define ColorBlend_ColorDodge(T,A,B)    (ColorBlend_Buffer(T,A,B,ColorDodge))
#define ColorBlend_ColorBurn(T,A,B)     (ColorBlend_Buffer(T,A,B,ColorBurn))
#define ColorBlend_LinearDodge(T,A,B)   (ColorBlend_Buffer(T,A,B,LinearDodge))
#define ColorBlend_LinearBurn(T,A,B)    (ColorBlend_Buffer(T,A,B,LinearBurn))
#define ColorBlend_LinearLight(T,A,B)   (ColorBlend_Buffer(T,A,B,LinearLight))quint8
#define ColorBlend_VividLight(T,A,B)    (ColorBlend_Buffer(T,A,B,VividLight))
#define ColorBlend_PinLight(T,A,B)      (ColorBlend_Buffer(T,A,B,PinLight))
#define ColorBlend_HardMix(T,A,B)       (ColorBlend_Buffer(T,A,B,HardMix))
#define ColorBlend_Reflect(T,A,B)       (ColorBlend_Buffer(T,A,B,Reflect))
#define ColorBlend_Glow(T,A,B)          (ColorBlend_Buffer(T,A,B,Glow))
#define ColorBlend_Phoenix(T,A,B)       (ColorBlend_Buffer(T,A,B,Phoenix))

using namespace std;

qint32 Color_HueToRgb(qreal M1, qreal M2, qreal Hue, qreal *Channel)
{
	if (Hue < 0.0)
		Hue += 1.0;
	else if (Hue > 1.0)
		Hue -= 1.0;

	if ((6.0 * Hue) < 1.0)
		*Channel = (M1 + (M2 - M1) * Hue * 6.0);
	else if ((2.0 * Hue) < 1.0)
		*Channel = (M2);
	else if ((3.0 * Hue) < 2.0)
		*Channel = (M1 + (M2 - M1) * ((2.0F / 3.0F) - Hue) * 6.0);
	else
		*Channel = (M1);

	return true;
}

qint32 Color_RgbToHls(quint8 Red, quint8 Green, quint8 Blue, qreal *Hue, qreal *Lumination, qreal *Saturation)
{
	qreal Delta;
	qreal Max, Min;
	qreal Redf, Greenf, Bluef;

	Redf    = ((qreal)Red   / 255.0F);
	Greenf  = ((qreal)Green / 255.0F);
	Bluef   = ((qreal)Blue  / 255.0F);

	Max     = max(max(Redf, Greenf), Bluef);
	Min     = min(min(Redf, Greenf), Bluef);

	*Hue        = 0;
	*Lumination = (Max + Min) / 2.0F;
	*Saturation = 0;

	if (Max == Min)
		return true;

	Delta = (Max - Min);

	if (*Lumination < 0.5)
		*Saturation = Delta / (Max + Min);
	else
		*Saturation = Delta / (2.0 - Max - Min);

	if (Redf == Max)
		*Hue = (Greenf - Bluef) / Delta;
	else if (Greenf == Max)
		*Hue = 2.0 + (Bluef - Redf) / Delta;
	else
		*Hue = 4.0 + (Redf - Greenf) / Delta;

	*Hue /= 6.0;

	if (*Hue < 0.0)
		*Hue += 1.0;

	return true;
}

qint32 Color_HlsToRgb(qreal Hue, qreal Lumination, qreal Saturation, quint8 *Red, quint8 *Green, quint8 *Blue)
{
	qreal M1, M2;
	qreal Redf, Greenf, Bluef;

	if (Saturation == 0)
		{
		Redf    = Lumination;
		Greenf  = Lumination;
		Bluef   = Lumination;
		}
	else
		{
		if (Lumination <= 0.5)
			M2 = Lumination * (1.0 + Saturation);
		else
			M2 = Lumination + Saturation - Lumination * Saturation;

		M1 = (2.0 * Lumination - M2);

		Color_HueToRgb(M1, M2, Hue + (1.0F / 3.0F), &Redf);
		Color_HueToRgb(M1, M2, Hue, &Greenf);
		Color_HueToRgb(M1, M2, Hue - (1.0F / 3.0F), &Bluef);
		}

	*Red    = (quint8)(Redf * 255);
	*Blue   = (quint8)(Bluef * 255);
	*Green  = (quint8)(Greenf * 255);

	return true;
}

#define ColorBlend_Hls(T,A,B,O1,O2,O3) { \
	qreal HueA, LuminationA, SaturationA; \
	qreal HueB, LuminationB, SaturationL; \
	Color_RgbToHls((A)[2],(A)[1],(A)[0], &HueA, &LuminationA, &SaturationA); \
	Color_RgbToHls((B)[2],(B)[1],(B)[0], &HueB, &LuminationB, &SaturationB); \
	Color_HlsToRgb(O1,O2,O3,&(T)[2],&(T)[1],&(T)[0]); \
	}

#define ColorBlend_Hue(T,A,B)            ColorBlend_Hls(T,A,B,HueB,LuminationA,SaturationA)
#define ColorBlend_Saturation(T,A,B)     ColorBlend_Hls(T,A,B,HueA,LuminationA,SaturationB)
#define ColorBlend_Color(T,A,B)          ColorBlend_Hls(T,A,B,HueB,LuminationA,SaturationB)
#define ColorBlend_Luminosity(T,A,B)     ColorBlend_Hls(T,A,B,HueA,LuminationB,SaturationA)

ImageFilterNode::ImageFilterNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mPinInputImage1 = pinInput( "Image 1" );
	mPinInputImage2 = pinInput( "Image 2" );

	mPinInputImage1->registerPinInputType( PID_IMAGE );
	mPinInputImage2->registerPinInputType( PID_IMAGE );

	mValFilter = pinInput<fugio::ChoiceInterface *>( "Filter", mPinFilter, PID_CHOICE );

	QStringList		FilterNames;

	FilterNames << "Normal";
	FilterNames << "Lighten";
	FilterNames << "Darken";

	mValFilter->setChoices( FilterNames );

	mPinFilter->setValue( FilterNames.first() );

	mValOutputImage = pinOutput<fugio::VariantInterface *>( "Image", mPinOutputImage, PID_IMAGE );
}

void ImageFilterNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	fugio::Image	SrcImg1 = variant<fugio::Image>( mPinInputImage1 );

	if( !SrcImg1.isValid() )
	{
		mNode->setStatus( fugio::NodeInterface::Warning );

		return;
	}

	if( SrcImg1.format() != fugio::ImageFormat::RGB8 )
	{
		mNode->setStatus( fugio::NodeInterface::Warning );

		return;
	}

	fugio::Performance	Perf( mNode, "inputsUpdated", pTimeStamp );

	fugio::Image	DstImg = mValOutputImage->variant().value<fugio::Image>();

	if( DstImg.size() != SrcImg1.size() )
	{
		DstImg.setFormat( fugio::ImageFormat::RGB8 );
		DstImg.setSize( SrcImg1.width(), SrcImg1.height() );
		DstImg.setLineSize( 0, SrcImg1.width() * 3 );

		memcpy( DstImg.internalBuffer( 0 ), SrcImg1.buffer( 0 ), DstImg.bufferSize( 0 ) );

		pinUpdated( mPinOutputImage );
	}

	fugio::Image	SrcImg2 = variant<fugio::Image>( mPinInputImage2 );

	if( !SrcImg2.isValid() )
	{
		mNode->setStatus( fugio::NodeInterface::Warning );

		return;
	}

	if( SrcImg2.format() != fugio::ImageFormat::RGB8 )
	{
		mNode->setStatus( fugio::NodeInterface::Warning );

		return;
	}

	if( SrcImg1.size() != SrcImg2.size() )
	{
		mNode->setStatus( fugio::NodeInterface::Warning );

		return;
	}

	mNode->setStatus( fugio::NodeInterface::Initialised );

	for( int y = 0 ; y < SrcImg1.height() ; y++ )
	{
		const quint8		*SrcPtr1 = SrcImg1.buffer( 0 ) + SrcImg1.lineSize( 0 ) * y;
		const quint8		*SrcPtr2 = SrcImg2.buffer( 0 ) + SrcImg2.lineSize( 0 ) * y;
		quint8				*DstPtr = DstImg.internalBuffer( 0 ) + DstImg.lineSize( 0 ) * y;

		for( int x = 0 ; x < SrcImg1.width() ; x++ )
		{
			ColorBlend_Darken( DstPtr, SrcPtr1, SrcPtr2 );

			SrcPtr1 += 3;
			SrcPtr2 += 3;
			DstPtr  += 3;
		}
	}

	pinUpdated( mPinOutputImage );
}
