#ifndef JSONQUERYNODE_H
#define JSONQUERYNODE_H

#include <fugio/nodecontrolbase.h>
#include <fugio/core/variant_interface.h>

#include <QJsonDocument>
#include <QJsonArray>

class JsonQueryNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "JSON Query" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Json_Query" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE JsonQueryNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~JsonQueryNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp );

protected:
	class JsonQuery
	{
	public:
		JsonQuery( QJsonDocument &JD, QStringList &SL );

	private:
		void parseObject( const QJsonObject &JO, QStringList SL );

		void parseArray( const QJsonArray &JA, QStringList SL );

		void parseValue( const QJsonValue &JV, QStringList SL );

		const	QJsonDocument	mJD;

	public:
		QJsonArray				mRV;
	};

	QSharedPointer<fugio::PinInterface>			 mPinInputJson;
	QSharedPointer<fugio::PinInterface>			 mPinInputQuery;

	QSharedPointer<fugio::PinInterface>			 mPinOutputResults;
	fugio::VariantInterface						*mValOutputResults;
};

#endif // JSONQUERYNODE_H
