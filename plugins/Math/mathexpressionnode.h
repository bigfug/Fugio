#ifndef MATHEXPRESSIONNODE_H
#define MATHEXPRESSIONNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

#include <fugio/core/variant_interface.h>

#include "../libs/exprtk/exprtk.hpp"

class MathExpressionNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "MathExpression" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE MathExpressionNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~MathExpressionNode( void ) {}

	// NodeControlInterface interface

	virtual bool deinitialise( void ) Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual QList<QUuid> pinAddTypesInput() const Q_DECL_OVERRIDE;
	virtual QList<QUuid> pinAddTypesOutput() const Q_DECL_OVERRIDE;
	virtual bool canAcceptPin(fugio::PinInterface *pPin) const Q_DECL_OVERRIDE;

private:
	typedef qreal								 exprtk_type;
	typedef exprtk::symbol_table<exprtk_type>	 symbol_table_t;
	typedef exprtk::expression<exprtk_type>		 expression_t;
	typedef exprtk::parser<exprtk_type>			 parser_t;

	typedef struct ExpVar
	{
		ExpVar( void ) : mType( QMetaType::UnknownType ) {}

		std::string			mName;
		QMetaType::Type		mType;
		QUuid				mUuid;
		exprtk_type			mNumber;
		std::string			mString;
	} ExpVar;

	typedef struct ExpDat
	{
		std::string									 mString;

		symbol_table_t								 mSymbolTable;
		expression_t								 mExpression;
		parser_t									 mParser;

		std::list<ExpVar>							 mVarLst;

		void operator =( const ExpDat & ) = delete;
	} ExpDat;

	void expAddInputs( ExpDat *NewExpDat ) const;

	void expAddOutputs( ExpDat *NewExpDat ) const;

	void expUpdateInputs( ExpDat *pExpDat ) const;

	void expUpdateOutputs();

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputExpression;

	ExpDat										*mExpDat;
};


#endif // MATHEXPRESSIONNODE_H
