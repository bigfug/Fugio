#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

#include <fugio/gui/keyboard_interface.h>

class MainWindowNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Main_Window" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE MainWindowNode( QSharedPointer<fugio::NodeInterface> pNode);

	virtual ~MainWindowNode( void ) {}

	// NodeControlInterface interface
public:
	virtual bool initialise() Q_DECL_OVERRIDE;
	virtual bool deinitialise() Q_DECL_OVERRIDE;

	// QObject interface
private:
	virtual bool eventFilter( QObject *, QEvent * ) Q_DECL_OVERRIDE;

protected slots:
	void contextFrameStart( void );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinKeyboard;
	fugio::KeyboardInterface					*mKeyboard;

	QList<fugio::KeyboardEvent>					 mEvtLst;

	fugio::KeyboardEvent::Modifiers translateModifiers( Qt::KeyboardModifiers pModifiers );
};

#endif // MAINWINDOW_H
