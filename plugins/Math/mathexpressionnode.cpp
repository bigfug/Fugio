#include "mathexpressionnode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>

MathExpressionNode::MathExpressionNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mExpDat( nullptr )
{
	FUGID( PIN_INPUT_EXPRESSION, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );

	mPinInputExpression = pinInput( "Expression", PIN_INPUT_EXPRESSION );

	mPinInputExpression->registerPinInputType( PID_STRING );
}

bool MathExpressionNode::deinitialise()
{
	if( mExpDat )
	{
		delete mExpDat;

		mExpDat = nullptr;
	}

	return( NodeControlBase::deinitialise() );
}

void MathExpressionNode::expAddInputs( ExpDat *NewExpDat ) const
{
	for( QSharedPointer<fugio::PinInterface> P : mNode->enumInputPins() )
	{
		ExpVar			NewExpVar;

		if( P == mPinInputExpression )
		{
			continue;
		}

		NewExpVar.mName = P->name().toStdString();
		NewExpVar.mUuid = P->globalId();

		fugio::VariantInterface		*VI = input<fugio::VariantInterface *>( P );

		if( VI )
		{
			const QVariant	V = VI->variant();
			QMetaType::Type T = QMetaType::Type( V.type() );

			switch( T )
			{
				case QMetaType::Bool:
					NewExpVar.mType = T;
					NewExpVar.mNumber = V.toBool();
					break;

				case QMetaType::Int:
				case QMetaType::Float:
				case QMetaType::Double:
					NewExpVar.mType = T;
					NewExpVar.mNumber = V.toReal();
					break;

				case QMetaType::QString:
					NewExpVar.mType = T;
					NewExpVar.mString = V.toString().toStdString();
					break;

				default:
					break;
			}
		}

		if( NewExpVar.mType != QMetaType::UnknownType )
		{
			NewExpDat->mVarLst.push_back( NewExpVar );
		}
	}
}

void MathExpressionNode::expAddOutputs( ExpDat *NewExpDat ) const
{
	for( QSharedPointer<fugio::PinInterface> P : mNode->enumOutputPins() )
	{
		if( !P->hasControl() )
		{
			return;
		}

		ExpVar			NewExpVar;

		NewExpVar.mName = P->name().toStdString();
		NewExpVar.mUuid = P->globalId();

		fugio::VariantInterface		*VI = qobject_cast<fugio::VariantInterface *>( P->control()->qobject() );

		if( VI )
		{
			const QVariant	V = VI->variant();
			QMetaType::Type T = QMetaType::Type( V.type() );

			switch( T )
			{
				case QMetaType::Bool:
				case QMetaType::Int:
				case QMetaType::Float:
				case QMetaType::Double:
					NewExpVar.mType = T;
					break;

				case QMetaType::QString:
					NewExpVar.mType = T;
					break;

				default:
					break;
			}
		}

		if( NewExpVar.mType != QMetaType::UnknownType )
		{
			NewExpDat->mVarLst.push_back( NewExpVar );
		}
	}
}

void MathExpressionNode::expUpdateInputs( ExpDat *pExpDat ) const
{
	for( ExpVar &V : pExpDat->mVarLst )
	{
		QSharedPointer<fugio::PinInterface> P = mNode->findPinByGlobalId( V.mUuid );

		if( !P || P->direction() == PIN_OUTPUT )
		{
			continue;
		}

		fugio::VariantInterface		*VI = input<fugio::VariantInterface *>( P );

		if( VI )
		{
			const QVariant	Q = VI->variant();
			QMetaType::Type T = QMetaType::Type( Q.type() );

			if( T == V.mType )
			{
				switch( T )
				{
					case QMetaType::Bool:
						V.mNumber = Q.toBool();
						break;

					case QMetaType::Int:
					case QMetaType::Float:
					case QMetaType::Double:
						V.mNumber = Q.toReal();
						break;

					case QMetaType::QString:
						V.mString = Q.toString().toStdString();
						break;

					default:
						break;
				}
			}
		}
	}
}

void MathExpressionNode::expUpdateOutputs()
{
	for( ExpVar &V : mExpDat->mVarLst )
	{
		QSharedPointer<fugio::PinInterface> P = mNode->findPinByGlobalId( V.mUuid );

		if( !P || P->direction() == PIN_INPUT )
		{
			continue;
		}

		fugio::VariantInterface		*VI = qobject_cast<fugio::VariantInterface *>( P->control()->qobject() );

		if( VI )
		{
			const QVariant	Q = VI->variant();
			QMetaType::Type T = QMetaType::Type( Q.type() );

			if( T == V.mType )
			{
				switch( T )
				{
					case QMetaType::Bool:
						if( VI->variant().toBool() != V.mNumber )
						{
							VI->setVariant( !V.mNumber ? false : true );

							pinUpdated( P );
						}
						break;

					case QMetaType::Int:
					case QMetaType::Float:
					case QMetaType::Double:
						if( VI->variant().toReal() != V.mNumber )
						{
							VI->setVariant( V.mNumber );

							pinUpdated( P );
						}
						break;

					case QMetaType::QString:
						{
							const QString	NewStr = QString::fromStdString( V.mString );

							if( VI->variant().toString() != NewStr )
							{
								VI->setVariant( NewStr );

								pinUpdated( P );
							}
						}
						break;

					default:
						break;
				}
			}

			pinUpdated( P );
		}
	}
}

void MathExpressionNode::inputsUpdated( qint64 pTimeStamp )
{
	if( pTimeStamp && mPinInputExpression->isUpdated( pTimeStamp ) )
	{
		ExpDat		*NewExpDat = new ExpDat();

		if( !NewExpDat )
		{
			return;
		}

		NewExpDat->mString = variant( mPinInputExpression ).toString().toStdString();

		NewExpDat->mSymbolTable.add_constants();

		expAddInputs( NewExpDat );

		expAddOutputs( NewExpDat );

		for( ExpVar &V : NewExpDat->mVarLst )
		{
			switch( V.mType )
			{
				case QMetaType::Bool:
				case QMetaType::Int:
				case QMetaType::Float:
				case QMetaType::Double:
					if( !NewExpDat->mSymbolTable.add_variable( V.mName, V.mNumber ) )
					{
						qWarning() << "Couldn't register variable" << QString::fromStdString( V.mName );
					}
					break;

				case QMetaType::QString:
					if( !NewExpDat->mSymbolTable.add_stringvar( V.mName, V.mString ) )
					{
						qWarning() << "Couldn't register string variable" << QString::fromStdString( V.mName );
					}
					break;

				default:
					break;
			}
		}

		NewExpDat->mExpression.register_symbol_table( NewExpDat->mSymbolTable );

		if( !NewExpDat->mParser.compile( NewExpDat->mString, NewExpDat->mExpression ) )
		{
			for( std::size_t i = 0; i < NewExpDat->mParser.error_count(); ++i )
			{
				typedef exprtk::parser_error::type error_t;

				error_t error = NewExpDat->mParser.get_error( i );

				exprtk::parser_error::update_error( error, NewExpDat->mString );

				qWarning() << QString( "Error[%1] Type[%2] at line: %3 column: %4 - %5" )
							.arg( i )
							.arg( exprtk::parser_error::to_str( error.mode ).c_str() )
							.arg( error.line_no )
							.arg( error.column_no )
							.arg( error.diagnostic.c_str() );
			}

			delete NewExpDat;

			return;
		}

		if( mExpDat )
		{
			delete mExpDat;
		}

		mExpDat = NewExpDat;
	}

	if( !mExpDat )
	{
		return;
	}

	expUpdateInputs( mExpDat );

	mExpDat->mExpression.value();

	expUpdateOutputs();
}


QList<QUuid> MathExpressionNode::pinAddTypesInput() const
{
	static const QList<QUuid>	PinLst =
	{
		PID_BOOL,
		PID_FLOAT,
		PID_INTEGER,
		PID_STRING,
		PID_VARIANT
	};

	return( PinLst );
}

QList<QUuid> MathExpressionNode::pinAddTypesOutput() const
{
	static const QList<QUuid>	PinLst =
	{
		PID_BOOL,
		PID_FLOAT,
		PID_INTEGER,
		PID_STRING
	};

	return( PinLst );
}

bool MathExpressionNode::canAcceptPin( fugio::PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_OUTPUT );
}
