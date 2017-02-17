#include "vstview.h"

#if defined( VST_SUPPORTED )

//#include "pluginterfaces/test/itest.h"
//#include "pluginterfaces/base/ipluginbase.h"

//#include "pluginterfaces/vst/ivstcomponent.h"
//#include "pluginterfaces/vst/ivstaudioprocessor.h"
//#include "pluginterfaces/vst/ivsthostapplication.h"
//#include "pluginterfaces/vst/ivsteditcontroller.h"
//#include "pluginterfaces/vst/ivstunits.h"

//#include "base/source/fstring.h"
//#include "base/source/fcontainer.h"
//#include "base/source/tarray.h"
//#include "base/source/tdictionary.h"
//#include "base/source/fcommandline.h"

#include <pluginterfaces/gui/iplugview.h>

using namespace Steinberg;

#endif // defined( VST_SUPPORTED )

#if defined( VST_SUPPORTED )
VSTView::VSTView( Steinberg::IPlugView *pPluginView )
	: mPluginView( pPluginView )
{
	mPluginView->attached( reinterpret_cast<void *>( winId() ), Steinberg::kPlatformTypeHWND );

	if( mPluginView->canResize() == kResultTrue )
	{
		Steinberg::ViewRect	VR;

		VR.left   = rect().left();
		VR.right  = rect().right();
		VR.bottom = rect().bottom();
		VR.top    = rect().top();

		mPluginView->checkSizeConstraint( &VR );

		QRect				NR;

		NR.setLeft( VR.left );
		NR.setRight( VR.right );
		NR.setBottom( VR.bottom );
		NR.setTop( VR.top );

		setMinimumSize( NR.size() );

		mPluginView->onSize( &VR );
	}
	else
	{

	}
}

VSTView::~VSTView()
{
	mPluginView->removed();
}

tresult VSTView::resizeView( IPlugView *view, ViewRect *newSize )
{
	Q_UNUSED( view )
	Q_UNUSED( newSize )

	return( Steinberg::kResultOk );
}

void VSTView::resizeEvent( QResizeEvent * )
{

}

#endif
