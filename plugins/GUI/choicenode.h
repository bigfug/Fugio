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

public:
	Q_INVOKABLE explicit ChoiceNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~ChoiceNode( void );

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual bool initialise() Q_DECL_OVERRIDE;

	virtual QWidget *gui( void ) Q_DECL_OVERRIDE;

	virtual void loadSettings( QSettings &pSettings ) Q_DECL_OVERRIDE;

	virtual void saveSettings( QSettings &pSettings ) Q_DECL_OVERRIDE;

signals:
	void valueUpdated( const QString &pValue );

	void choicesUpdated( const QStringList &pChoices );

protected slots:
	void valueChanged( const QString &pValue );

	void pinLinked( QSharedPointer<fugio::PinInterface> P );
	void pinUnlinked( QSharedPointer<fugio::PinInterface> P );

	void clicked( void );

private:
	QSharedPointer<fugio::PinInterface>		 mPinChoice;
	fugio::VariantInterface					*mValChoice;

	QList<QPointer<QPushButton>>			 mGUIs;
};

#endif // CHOICENODE_H
