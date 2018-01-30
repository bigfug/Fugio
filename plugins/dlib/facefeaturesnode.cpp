#include "facefeaturesnode.h"

#include <QtConcurrent/QtConcurrentRun>

#include <fugio/core/uuid.h>

#include <fugio/image/image.h>
#include <fugio/performance.h>

#if defined( DLIB_SUPPORTED )
#include <dlib/image_processing.h>
#include <dlib/image_processing/frontal_face_detector.h>

namespace dlib
{
template <
		typename pixel_type
		>
class fugio_image
{
public:
	typedef pixel_type type;
	typedef default_memory_manager mem_manager_type;

	fugio_image( fugio::Image *pImgInt )
	{
		//			DLIB_CASSERT(img.depth() == cv::DataType<typename pixel_traits<pixel_type>::basic_pixel_type>::depth &&
		//						 img.channels() == pixel_traits<pixel_type>::num,
		//						 "The pixel type you gave doesn't match pixel used by the open cv Mat object."
		//						 << "\n\t img.depth():    " << img.depth()
		//						 << "\n\t img.cv::DataType<typename pixel_traits<pixel_type>::basic_pixel_type>::depth: "
		//							<< cv::DataType<typename pixel_traits<pixel_type>::basic_pixel_type>::depth
		//						 << "\n\t img.channels(): " << img.channels()
		//						 << "\n\t img.pixel_traits<pixel_type>::num: " << pixel_traits<pixel_type>::num
		//						 );
		init(pImgInt);
	}

	fugio_image() : _data(0) {}

	unsigned long size () const { return static_cast<unsigned long>(nr()*nc()); }

	inline pixel_type* operator[](const long row )
	{
		// make sure requires clause is not broken
		DLIB_ASSERT(0 <= row && row < nr(),
					"\tpixel_type* fugio_image::operator[](row)"
					<< "\n\t you have asked for an out of bounds row "
					<< "\n\t row:  " << row
					<< "\n\t nr(): " << nr()
					<< "\n\t this:  " << this
					);

		return reinterpret_cast<pixel_type*>( _data->internalBuffer( 0 ) + width_step() * row);
	}

	inline const pixel_type* operator[](const long row ) const
	{
		// make sure requires clause is not broken
		DLIB_ASSERT(0 <= row && row < nr(),
					"\tconst pixel_type* fugio_image::operator[](row)"
					<< "\n\t you have asked for an out of bounds row "
					<< "\n\t row:  " << row
					<< "\n\t nr(): " << nr()
					<< "\n\t this:  " << this
					);

		return reinterpret_cast<const pixel_type*>( _data->buffer( 0 ) + width_step() * row);
	}

	long nr() const { return _data->height(); }
	long nc() const { return _data->width(); }
	long width_step() const { return _data->lineSize( 0 ); }

	fugio_image& operator=( const fugio_image& item)
	{
		_data = item._data;
		return *this;
	}

	fugio_image& operator=( const fugio::VariantInterface* img)
	{
		init(img);
		return *this;
	}

private:

	void init (const fugio::Image* img)
	{
		//			DLIB_CASSERT( img->dataOrder == 0, "Only interleaved color channels are supported with fugio_image");
		//			DLIB_CASSERT((img->depth&0xFF)/8*img->nChannels == sizeof(pixel_type),
		//						 "The pixel type you gave doesn't match the size of pixel used by the open cv image struct");

		_data = img;
	}

	const fugio::Image* _data;
};

// ----------------------------------------------------------------------------------------

template <
		typename T
		>
const matrix_op<op_array2d_to_mat<fugio_image<T> > > mat (
		const fugio_image<T>& m
		)
{
	typedef op_array2d_to_mat<fugio_image<T> > op;
	return matrix_op<op>(op(m));
}

// ----------------------------------------------------------------------------------------

// Define the global functions that make fugio_image a proper "generic image" according to
// ../image_processing/generic_image.h
template <typename T>
struct image_traits<fugio_image<T> >
{
	typedef T pixel_type;
};

template <typename T>
inline long num_rows( const fugio_image<T>& img) { return img.nr(); }
template <typename T>
inline long num_columns( const fugio_image<T>& img) { return img.nc(); }

template <typename T>
inline void* image_data(
		fugio_image<T>& img
		)
{
	if (img.size() != 0)
		return img._data->internalBuffer( 0 );
	else
		return 0;
}

template <typename T>
inline const void* image_data(
		const fugio_image<T>& img
		)
{
	if (img.size() != 0)
		return img._data->buffer( 0 );
	else
		return 0;
}

template <typename T>
inline long width_step(
		const fugio_image<T>& img
		)
{
	return img.width_step();
}

// ----------------------------------------------------------------------------------------

}

#endif

FaceFeaturesNode::FaceFeaturesNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mLoading( false ), mLoaded( false )
{
	FUGID( PIN_INPUT_DATA, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_IMAGE, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_OUTPUT_RECTS, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );
	FUGID( PIN_OUTPUT_SHAPES, "249f2932-f483-422f-b811-ab679f006381" );
	FUGID( PIN_OUTPUT_CHIPS, "ce8d578e-c5a4-422f-b3c4-a1bdf40facdb" );

	mPinInputImage = pinInput( "Image", PIN_INPUT_IMAGE );

	mPinInputData = pinInput( "Data", PIN_INPUT_DATA );

	mValOutputRects = pinOutput<fugio::VariantInterface *>( "Rects", mPinOutputRects, PID_RECT, PIN_OUTPUT_RECTS );

	mValOutputShapes = pinOutput<fugio::VariantInterface *>( "Shapes", mPinOutputShapes, PID_POLYGON, PIN_OUTPUT_SHAPES );

	mValOutputChips = pinOutput<fugio::VariantInterface *>( "Chips", mPinOutputChips, PID_POLYGON, PIN_OUTPUT_CHIPS );

	mValOutputRects->variantClear();

	mValOutputShapes->variantClear();

	mValOutputChips->variantClear();

#if defined( DLIB_SUPPORTED )
	mDetector = get_frontal_face_detector();
#endif
}

void FaceFeaturesNode::loadDataFile( const QString &pFilename )
{
	mLoading = true;
	mLoaded  = false;

#if defined( DLIB_SUPPORTED )
	try
	{
		dlib::deserialize( pFilename.toStdString() ) >> mShapePredictor;

		mNode->context()->updateNode( mNode );

		mLoaded = true;
	}
	catch( ... )
	{
	}
#endif

	mLoading = false;
}

void FaceFeaturesNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	if( mPinInputData->isUpdated( pTimeStamp ) )
	{
		QString		Filename = variant( mPinInputData ).toString();

		if( !Filename.isEmpty() && !mLoading )
		{
			mNode->setStatus( fugio::NodeInterface::Initialising );

			QtConcurrent::run( this, &FaceFeaturesNode::loadDataFile, Filename );
		}
	}

	if( mNode->status() != fugio::NodeInterface::Initialised )
	{
		mNode->setStatus( fugio::NodeInterface::Initialised );
	}

	if( mPinInputImage->isUpdated( pTimeStamp ) )
	{
		fugio::Image				I = variant( mPinInputImage ).value<fugio::Image>();

		if( I.isEmpty() )
		{
			if( I.format() != fugio::ImageFormat::GRAY8 )
			{
				return;
			}

			fugio::Performance		Perf( mNode, "inputsUpdated", pTimeStamp );

#if defined( DLIB_SUPPORTED )
			try
			{
				dlib::matrix<unsigned char>		SrcMat;

				assign_image( SrcMat, fugio_image<unsigned char>( &I ) );

				std::vector<rectangle> dets = mDetector( SrcMat );

				mValOutputRects->setVariantCount( dets.size() );

				mValOutputShapes->setVariantCount( mLoaded ? dets.size() : 0 );

				mValOutputChips->setVariantCount( mLoaded ? dets.size() : 0 );

				if( !dets.empty() )
				{
					std::vector<full_object_detection> shapes;

					for( unsigned long j = 0; j < dets.size(); ++j )
					{
						const rectangle	&r = dets[ j ];

						mValOutputRects->setVariant( j, QRectF( r.left(), r.top(), r.width(), r.height() ) );

						if( mLoaded )
						{
							full_object_detection shape = mShapePredictor( SrcMat, r );

							shapes.push_back( shape );

							QPolygonF		Polygon;

							for( unsigned long i = 0; i < shape.num_parts() ; ++i )
							{
								const point &p = shape.part( i );

								Polygon << QPointF( p.x(), p.y() );
							}

							mValOutputShapes->setVariant( j, Polygon );
						}
					}

					if( mLoaded )
					{
						const std::vector<chip_details> chips = get_face_chip_details( shapes );

						for( unsigned long j = 0 ; j < chips.size() ; ++j )
						{
							const chip_details	&c = chips[ j ];

							const dlib::vector<double,2> cent = center( c.rect );

							const dlib::vector<double,2> r0 = rotate_point<double>( cent, c.rect.tl_corner(), c.angle );
							const dlib::vector<double,2> r1 = rotate_point<double>( cent, c.rect.tr_corner(), c.angle );
							const dlib::vector<double,2> r2 = rotate_point<double>( cent, c.rect.br_corner(), c.angle );
							const dlib::vector<double,2> r3 = rotate_point<double>( cent, c.rect.bl_corner(), c.angle );

							QPolygonF		Polygon;

							Polygon << QPointF( r0.x(), r0.y() );
							Polygon << QPointF( r1.x(), r1.y() );
							Polygon << QPointF( r2.x(), r2.y() );
							Polygon << QPointF( r3.x(), r3.y() );

							mValOutputChips->setVariant( j, Polygon );
						}
					}
				}

				pinUpdated( mPinOutputRects );
				pinUpdated( mPinOutputShapes );
				pinUpdated( mPinOutputChips );
			}
			catch( ... )
			{
				return;
			}
#endif
		}
	}
}
