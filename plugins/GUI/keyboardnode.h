#ifndef KEYBOARDNODE_H
#define KEYBOARDNODE_H

#include <QObject>

#include <QShortcut>
#include <QList>
#include <QPair>
#include <QSharedPointer>
#include <QEvent>

#include <tuple>

#include <fugio/pin_interface.h>
#include <fugio/nodecontrolbase.h>

class KeyboardNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Generate triggers from a keyboard." )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Keyboard_(GUI)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit KeyboardNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~KeyboardNode( void );

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual QList<QUuid> pinAddTypesOutput() const Q_DECL_OVERRIDE;

	virtual bool canAcceptPin(fugio::PinInterface *pPin) const Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual QWidget *gui( void ) Q_DECL_OVERRIDE;

signals:
	void listening( bool pListening );

protected slots:
	void onButton( bool pChecked );

protected:
	QSharedPointer<fugio::PinInterface>					 mPinKeyboard;

	typedef std::tuple<QEvent::Type,int,QString>		 KeyTuple;

	QList<KeyTuple>										 mKeyEvt;

	bool												 mListening;
};

#endif // KEYBOARDNODE_H
