#ifndef FILENAMEPIN_H
#define FILENAMEPIN_H

#include <QObject>

#include <fugio/core/uuid.h>

#include <fugio/pincontrolbase.h>

#include <fugio/file/filename_interface.h>
#include <fugio/core/variant_interface.h>

class FilenamePin : public fugio::PinControlBase, public fugio::FilenameInterface, public fugio::VariantInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::VariantInterface fugio::FilenameInterface )

public:
	Q_INVOKABLE explicit FilenamePin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~FilenamePin( void ) {}

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE
	{
		return( mFilenames.first() );
	}

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "Filename" );
	}

	virtual void loadSettings(QSettings &pSettings) Q_DECL_OVERRIDE;
	virtual void saveSettings(QSettings &pSettings) const Q_DECL_OVERRIDE;

	//-------------------------------------------------------------------------
	// InterfaceFilename

	virtual QString filename( void ) const Q_DECL_OVERRIDE
	{
		return( mFilenames.first() );
	}

	virtual void setFilename( const QString &pFilename ) Q_DECL_OVERRIDE
	{
		mFilenames[ 0 ] = pFilename;
	}

	// VariantInterface interface
public:
	virtual void setVariant( const QVariant &pValue ) Q_DECL_OVERRIDE;
	virtual void setVariant( int pIndex, const QVariant &pValue ) Q_DECL_OVERRIDE;
	virtual QVariant variant( int pIndex = 0 ) const Q_DECL_OVERRIDE;

	virtual void setVariantCount( int pCount ) Q_DECL_OVERRIDE
	{
		mFilenames.resize( pCount );
	}

	virtual int variantCount( void ) const Q_DECL_OVERRIDE
	{
		return( mFilenames.size() );
	}

	virtual QMetaType::Type variantType( void ) const Q_DECL_OVERRIDE
	{
		return( QMetaType::QString );
	}

	virtual void setFromBaseVariant( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		setFromBaseVariant( 0, pValue );
	}

	virtual void setFromBaseVariant( int pIndex, const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		setVariant( pIndex, pValue );
	}

	virtual QVariant baseVariant( int pIndex ) const Q_DECL_OVERRIDE
	{
		return( variant( pIndex ) );
	}

	virtual void setVariantType( QMetaType::Type ) Q_DECL_OVERRIDE
	{

	}

private:
	QVector<QString>		mFilenames;
};

#endif // FILENAMEPIN_H
