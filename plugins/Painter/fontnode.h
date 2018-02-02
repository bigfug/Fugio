#ifndef FONTNODE_H
#define FONTNODE_H

#include <fugio/nodecontrolbase.h>
#include <fugio/core/variant_interface.h>

class FontNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Font" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Font_(GUI)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit FontNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~FontNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual void loadSettings(QSettings &pSettings) Q_DECL_OVERRIDE;
	virtual void saveSettings(QSettings &pSettings) const Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual QWidget *gui( void ) Q_DECL_OVERRIDE;

protected slots:
	void buttonClicked( bool );

private:
	QSharedPointer<fugio::PinInterface>			 mPinOutputFont;
	fugio::VariantInterface						*mValOutputFont;
};

#endif // FONTNODE_H
