#ifndef FONTMETRICSNODE_H
#define FONTMETRICSNODE_H

#include <fugio/nodecontrolbase.h>

#include <fugio/core/variant_interface.h>

class FontMetricsNode : public fugio::NodeControlBase
{
	Q_OBJECT

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "FontMetrics" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "FontMetrics_(Painter)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE FontMetricsNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~FontMetricsNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

protected:
	QSharedPointer<fugio::PinInterface>		 mPinInputFont;
	QSharedPointer<fugio::PinInterface>		 mPinInputText;

	QSharedPointer<fugio::PinInterface>		 mPinOutputBounds;
	fugio::VariantInterface					*mValOutputBounds;
};

#endif // FONTMETRICSNODE_H
