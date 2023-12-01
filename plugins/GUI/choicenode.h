#ifndef CHOICENODE_H
#define CHOICENODE_H

#include <QObject>
#include <QStringList>
#include <QPointer>
#include <QPushButton>

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/variant_interface.h>

#include <fugio/nodecontrolbase.h>

class ChoiceNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Provides a simple GUI button that can be clicked on to generate a trigger, boolean, and toggle." )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Choice_(GUI)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit ChoiceNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~ChoiceNode( void ) Q_DECL_OVERRIDE {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual bool initialise() Q_DECL_OVERRIDE;

	virtual QWidget *gui( void ) Q_DECL_OVERRIDE;

	virtual void loadSettings( QSettings &pSettings ) Q_DECL_OVERRIDE;

	virtual void saveSettings( QSettings &pSettings ) const Q_DECL_OVERRIDE;

signals:
	void valueUpdated( const QString &pValue );

	void choicesUpdated( const QStringList &pChoices );

	void guiTextUpdated( QString pText );

	void guiEnabled( bool pEnabled );

protected slots:
	void valueChanged( const QString &pValue );

	void pinLinked( QSharedPointer<fugio::PinInterface> P );
	void pinUnlinked( QSharedPointer<fugio::PinInterface> P );

	void clicked( void );

	void choicesChanged( void );

private:
	QSharedPointer<fugio::PinInterface>		 mPinChoice;
	fugio::VariantInterface					*mValChoice;

	//QList<QPointer<QPushButton>>			 mGUIs;
};

#endif // CHOICENODE_H
