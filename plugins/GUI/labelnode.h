#ifndef LABELNODE_H
#define LABELNODE_H

#include <QPointer>
#include <QGraphicsTextItem>

#include <fugio/nodecontrolbase.h>

class LabelNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Display text" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Label_(GUI)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit LabelNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~LabelNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

//	virtual QWidget *gui( void );

	virtual QGraphicsItem *guiItem() Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

signals:
	void valueUpdated( QString pValue );

private:
	QSharedPointer<fugio::PinInterface>			 mPinInput;
	QString										 mValInput;
	QPointer<QGraphicsTextItem>					 mTextItem;
};

#endif // LABELNODE_H
