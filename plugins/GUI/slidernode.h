#ifndef SLIDERNODE_H
#define SLIDERNODE_H

#include <QObject>

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>
#include <fugio/core/variant_interface.h>

#include <fugio/core/uuid.h>

#include <fugio/nodecontrolbase.h>

class SliderNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Slider" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Slider_(GUI)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit SliderNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~SliderNode( void );

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual QWidget *gui( void ) Q_DECL_OVERRIDE;

	virtual void loadSettings(QSettings &pSettings) Q_DECL_OVERRIDE;
	virtual void saveSettings( QSettings &pSettings ) const Q_DECL_OVERRIDE;

signals:
	void valueChanged( int pValue );

protected slots:
	void sliderValueChanged( int pValue );

private:
	QSharedPointer<fugio::PinInterface>		 mPinValue;
	fugio::VariantInterface					*mValue;
};


#endif // SLIDERNODE_H
