#ifndef DIALNODE_H
#define DIALNODE_H

#include <QObject>

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>
#include <fugio/core/variant_interface.h>

#include <fugio/core/uuid.h>

#include <fugio/nodecontrolbase.h>

class DialNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Dial" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Dial_(GUI)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit DialNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~DialNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual void loadSettings(QSettings &pSettings) Q_DECL_OVERRIDE;
	virtual void saveSettings( QSettings &pSettings ) const Q_DECL_OVERRIDE;

	virtual QGraphicsItem *guiItem() Q_DECL_OVERRIDE;

signals:
	void valueChanged( qreal pValue );

protected slots:
	void dialValueChanged( qreal pValue );

private:
	QSharedPointer<fugio::PinInterface>		 mPinOutputValue;
	fugio::VariantInterface					*mValOutputValue;
};

#endif // DIALNODE_H
