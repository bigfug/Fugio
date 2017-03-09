#ifndef OCULUSRIFTNODE_H
#define OCULUSRIFTNODE_H

#include <QObject>
#include <QVector>

#include <fugio/nodecontrolbase.h>

#include <fugio/render_interface.h>
#include <fugio/opengl/node_render_interface.h>

#include "deviceoculusrift.h"

#if defined( OCULUS_PLUGIN_SUPPORTED )
using namespace OVR;
#endif

class OculusRiftNode : public fugio::NodeControlBase, public fugio::NodeRenderInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::NodeRenderInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Renders a view for an Oculus Rift" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Oculus_Rift" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE OculusRiftNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~OculusRiftNode( void ) {}

	// InterfaceNodeControl interface

	virtual bool initialise( void ) Q_DECL_OVERRIDE;

	virtual bool deinitialise( void ) Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual QList<QUuid> pinAddTypesInput() const Q_DECL_OVERRIDE;

	virtual bool canAcceptPin(fugio::PinInterface *pPin) const Q_DECL_OVERRIDE;

	// fugio::NodeRenderInterface interface
public:
	virtual void render( qint64 pTimeStamp, QUuid pSourcePinId ) Q_DECL_OVERRIDE;

private slots:
	void onContextFrame( void );

protected:
	QSharedPointer<DeviceOculusRift>		 mOculusRift;

	QSharedPointer<fugio::PinInterface>		 mPinViewMatrix;
	QSharedPointer<fugio::PinInterface>		 mPinNearPlane;
	QSharedPointer<fugio::PinInterface>		 mPinFarPlane;

	QSharedPointer<fugio::PinInterface>		 mPinGeometry;
	fugio::RenderInterface					*mGeometry;

	QSharedPointer<fugio::PinInterface>		 mPinProjection;
	fugio::VariantInterface					*mProjection;

	QSharedPointer<fugio::PinInterface>		 mPinView;
	fugio::VariantInterface					*mView;
};

#endif // OCULUSRIFTNODE_H
