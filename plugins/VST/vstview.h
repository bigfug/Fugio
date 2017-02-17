#ifndef VSTVIEW_H
#define VSTVIEW_H

#include <QWidget>

#if defined( VST_SUPPORTED )

#include <pluginterfaces/gui/iplugview.h>

#include <base/source/fobject.h>

namespace Steinberg {
class IPlugView;
namespace Vst {

class IComponent;
class IEditController;
}
}

using namespace Steinberg;

#endif

class VSTView : public QWidget
#if defined( VST_SUPPORTED )
		, public FObject, public IPlugFrame
#endif
{
	Q_OBJECT

public:
#if defined( VST_SUPPORTED )
	explicit VSTView( Steinberg::IPlugView *pPluginView );

	~VSTView();
#else
	explicit VSTView( void ) {}
#endif

#if defined( VST_SUPPORTED )
	virtual Steinberg::tresult PLUGIN_API resizeView( Steinberg::IPlugView* view, Steinberg::ViewRect* newSize );

	Steinberg::IPlugView *view( void )
	{
		return( mPluginView );
	}

//------------------------------------------------------------------------

	OBJ_METHODS(VSTView, FObject)
	REFCOUNT_METHODS(FObject)
	DEF_INTERFACES_1(IPlugFrame, FObject)

//------------------------------------------------------------------------

protected:
	virtual void resizeEvent( QResizeEvent *pEvent );

signals:

public slots:
private:
	Steinberg::IPlugView			*mPluginView;
#endif
};

#endif // VSTVIEW_H
