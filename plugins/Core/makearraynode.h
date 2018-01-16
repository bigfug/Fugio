#ifndef MAKEARRAYNODE_H
#define MAKEARRAYNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

class MakeArrayNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Make_(Array)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE MakeArrayNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~MakeArrayNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

//	virtual QWidget *gui() Q_DECL_OVERRIDE;

//	virtual QList<QUuid> pinAddTypesInput() const Q_DECL_OVERRIDE;

	virtual bool canAcceptPin( fugio::PinInterface *pPin ) const Q_DECL_OVERRIDE;

	virtual bool pinShouldAutoRename(fugio::PinInterface *pPin) const Q_DECL_OVERRIDE;

//	virtual void loadSettings(QSettings &pSettings) Q_DECL_OVERRIDE;
//	virtual void saveSettings(QSettings &pSettings) const Q_DECL_OVERRIDE;

protected slots:
//	void setType( int pIndex );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInput;

	QSharedPointer<fugio::PinInterface>			 mPinOutput;
	fugio::VariantInterface						*mValOutput;

//	QMetaType::Type								 mType;
};

#endif // MAKEARRAYNODE_H
