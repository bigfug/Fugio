#ifndef REGEXPNODE_H
#define REGEXPNODE_H

#include <QObject>
#include <QRegExp>

#include <fugio/node_interface.h>
#include <fugio/pin_interface.h>

#include <fugio/core/variant_interface.h>

#include <fugio/core/uuid.h>

#include <fugio/nodecontrolbase.h>

class RegExpNode : public fugio::NodeControlBase
{
	Q_OBJECT

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Matches patterns in a string using Regular Expressions" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/RegExp" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit RegExpNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~RegExpNode( void );

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual QWidget *gui( void );

	virtual void inputsUpdated( qint64 pTimeStamp );

protected slots:
	void patternUpdate( const QString &pPattern );

private:
	QSharedPointer<fugio::PinInterface>			 mPinInput;

	QSharedPointer<fugio::PinInterface>			 mPinOutput;
	fugio::VariantInterface						*mValOutput;

	QRegExp										 mRegExp;
};

#endif // REGEXPNODE_H
