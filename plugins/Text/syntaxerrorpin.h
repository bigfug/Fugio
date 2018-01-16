#ifndef SYNTAXERRORPIN_H
#define SYNTAXERRORPIN_H

#include <QObject>

#include <QByteArray>
#include <QSettings>

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/array_interface.h>
#include <fugio/core/list_interface.h>
#include <fugio/core/size_interface.h>

#include <fugio/pincontrolbase.h>

#include <fugio/serialise_interface.h>

#include <fugio/text/syntax_highlighter_instance_interface.h>

#include <fugio/text/syntax_error_interface.h>
#include <fugio/text/syntax_error_signals.h>

class SyntaxErrorPin : public fugio::PinControlBase, public fugio::SyntaxErrorInterface, public fugio::SerialiseInterface, public fugio::ListInterface, public fugio::SizeInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::SyntaxErrorInterface fugio::SerialiseInterface fugio::ListInterface fugio::SizeInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_PIN_URL( "Syntax_Error" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit SyntaxErrorPin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~SyntaxErrorPin( void ) {}

	static void registerMetaType( void );

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE;

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "Syntax Error" );
	}

	//-------------------------------------------------------------------------
	// fugio::SerialiseInterface

	virtual void serialise( QDataStream &pDataStream ) const  Q_DECL_OVERRIDE;
	virtual void deserialise( QDataStream &pDataStream ) Q_DECL_OVERRIDE;

	//-------------------------------------------------------------------------
	// ListInterface interface

	virtual int listSize() const Q_DECL_OVERRIDE;
	virtual QUuid listPinControl() const Q_DECL_OVERRIDE;
	virtual QVariant listIndex(int pIndex) const Q_DECL_OVERRIDE;
	virtual void listSetIndex( int pIndex, const QVariant &pValue ) Q_DECL_OVERRIDE;
	virtual void listSetSize( int pSize ) Q_DECL_OVERRIDE;

	virtual void listClear() Q_DECL_OVERRIDE
	{
		mSyntaxErrors.clear();
	}

	virtual void listAppend( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		Q_UNUSED( pValue )
	}

	virtual bool listIsEmpty() const Q_DECL_OVERRIDE
	{
		return( mSyntaxErrors.isEmpty() );
	}

	virtual QMetaType::Type listType( void ) const Q_DECL_OVERRIDE
	{
		return( QMetaType::Type( QMetaType::type( "fugio::SyntaxError" ) ) );
	}

	// SizeInterface interface
public:
	virtual int sizeDimensions() const Q_DECL_OVERRIDE;
	virtual float size(int pDimension) const Q_DECL_OVERRIDE;
	virtual float sizeWidth() const Q_DECL_OVERRIDE;
	virtual float sizeHeight() const Q_DECL_OVERRIDE;
	virtual float sizeDepth() const Q_DECL_OVERRIDE;
	virtual QSizeF toSizeF() const Q_DECL_OVERRIDE;
	virtual QVector3D toVector3D() const Q_DECL_OVERRIDE;

	//-------------------------------------------------------------------------
	// SyntaxErrorInterface interface
public:
	virtual fugio::SyntaxErrorSignals *syntaxErrorSignals() Q_DECL_OVERRIDE
	{
		return( &mSyntaxSignals );
	}

	virtual void setSyntaxErrors( QList<fugio::SyntaxError> pSyntaxErrors ) Q_DECL_OVERRIDE
	{
		mSyntaxErrors = pSyntaxErrors;

		emit mSyntaxSignals.syntaxErrorsUpdated( mSyntaxErrors );
	}

	virtual void clearSyntaxErrors() Q_DECL_OVERRIDE
	{
		if( !mSyntaxErrors.isEmpty() )
		{
			mSyntaxErrors.clear();

			emit mSyntaxSignals.syntaxErrorsUpdated( mSyntaxErrors );
		}
	}

	virtual QList<fugio::SyntaxError> syntaxErrors( void ) Q_DECL_OVERRIDE
	{
		return( mSyntaxErrors );
	}

	virtual void setHighlighterUuid(QUuid pUuid) Q_DECL_OVERRIDE
	{
		mHighlighterUuid = pUuid;
	}

	virtual QUuid highlighterUuid() const Q_DECL_OVERRIDE
	{
		return( mHighlighterUuid );
	}

private:
	QList<fugio::SyntaxError>	mSyntaxErrors;

	fugio::SyntaxErrorSignals	mSyntaxSignals;

	QUuid						mHighlighterUuid;
};


#endif // SYNTAXERRORPIN_H
