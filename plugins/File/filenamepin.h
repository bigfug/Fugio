#ifndef FILENAMEPIN_H
#define FILENAMEPIN_H

#include <QObject>

#include <fugio/core/uuid.h>

#include <fugio/pincontrolbase.h>

#include <fugio/file/filename_interface.h>
#include <fugio/core/variant_helper.h>

class FilenamePin : public fugio::PinControlBase, public fugio::FilenameInterface, public fugio::VariantHelper<QString>
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
		return( mValues.first() );
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
		return( mValues.first() );
	}

	virtual void setFilename( const QString &pFilename ) Q_DECL_OVERRIDE
	{
		mValues[ 0 ] = pFilename;
	}
};

#endif // FILENAMEPIN_H
