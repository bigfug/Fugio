#ifndef SCREENNODE_H
#define SCREENNODE_H

#include <QObject>
#include <QScreen>

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/variant_interface.h>
#include <fugio/choice_interface.h>

#include <fugio/gui/uuid.h>

#include <fugio/nodecontrolbase.h>

class ScreenNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Screen" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Screen_(GUI)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit ScreenNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~ScreenNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface interface
public:
	virtual bool initialise() Q_DECL_OVERRIDE;

	virtual void inputsUpdated(qint64 pTimeStamp) Q_DECL_OVERRIDE;

	virtual void loadSettings(QSettings &pSettings) Q_DECL_OVERRIDE;
	virtual void saveSettings(QSettings &pSettings) const Q_DECL_OVERRIDE;

protected slots:
	void setScreenName( QString pName );

	void screenVirtualGeometryChanged( QRect pRect );

	void updateScreenList( void );

protected:
	QScreen *fromName( QString pName ) const;

private:
	QString									 mScreenName;

	QSharedPointer<fugio::PinInterface>		 mPinInputScreen;
	fugio::ChoiceInterface					*mValInputScreen;

	QSharedPointer<fugio::PinInterface>		 mPinOutputGeometry;
	fugio::VariantInterface					*mValOutputGeometry;
};

#endif // SCREENNODE_H
