#ifndef INTEGERNODE_H
#define INTEGERNODE_H

#include <fugio/nodecontrolbase.h>
#include <fugio/core/uuid.h>
#include <fugio/core/variant_interface.h>
#include <fugio/pin_interface.h>

class IntegerNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "An integer (whole) number in a GUI." )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Integer_(GUI)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit IntegerNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~IntegerNode( void )
	{

	}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual QWidget *gui( void ) Q_DECL_OVERRIDE;

	virtual void loadSettings( QSettings &pSettings ) Q_DECL_OVERRIDE;

	virtual void saveSettings( QSettings &pSettings ) const Q_DECL_OVERRIDE;

signals:
	void valueUpdated( int pValue );

protected slots:
	void valueChanged( int pValue );

private:
	QSharedPointer<fugio::PinInterface>			 mPinValue;
	fugio::VariantInterface						*mValue;
};

#endif // INTEGERNODE_H
