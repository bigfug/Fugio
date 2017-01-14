#ifndef STRINGNODE_H
#define STRINGNODE_H

#include <QObject>

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/variant_interface.h>

#include <fugio/core/uuid.h>

#include <fugio/nodecontrolbase.h>

class StringNode : public fugio::NodeControlBase
{
	Q_OBJECT

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Creates a GUI to input a string" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "String_(GUI)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit StringNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~StringNode( void );

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual QWidget *gui( void ) Q_DECL_OVERRIDE;

	virtual void loadSettings( QSettings &pSettings ) Q_DECL_OVERRIDE;

	virtual void saveSettings( QSettings &pSettings ) const Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

signals:
	void valueUpdated( const QString &pValue );

protected slots:
	void valueChanged( const QString &pValue );

	void editingFinished( void );

	void contextFrameStart( void );

private:
	QSharedPointer<fugio::PinInterface>			 mPinInput;

	QSharedPointer<fugio::PinInterface>			 mPinValue;
	fugio::VariantInterface						*mString;

	qint64										 mLastUpdate;
	qint64										 mLastText;
	QString										 mText;
};
#endif // STRINGNODE_H
