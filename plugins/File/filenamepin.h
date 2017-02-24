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

	virtual QString toString( void ) const
	{
		return( mFilename );
	}

	virtual QString description( void ) const
	{
		return( "Filename" );
	}

	virtual void loadSettings(QSettings &pSettings);
	virtual void saveSettings(QSettings &pSettings) const;

	//-------------------------------------------------------------------------
	// InterfaceFilename

	virtual QString filename( void ) const
	{
		return( mFilename );
	}

	virtual void setFilename( const QString &pFilename )
	{
		mFilename = pFilename;
	}

	// VariantInterface interface
public:
	virtual void setVariant(const QVariant &pValue);
	virtual QVariant variant() const;
	virtual void setFromBaseVariant(const QVariant &pValue);
	virtual QVariant baseVariant() const;

private:
	QString		mFilename;
};

#endif // FILENAMEPIN_H
