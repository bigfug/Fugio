#ifndef INTERFACE_FILENAME_H
#define INTERFACE_FILENAME_H

#include <fugio/global.h>

#include <QUuid>
#include <QString>

#define PID_FILENAME				(QUuid("{7809c97a-dbb6-452c-adf6-6313510b4c80}"))

#define PIN_SETTING_FILE_PATTERN	("filename/pattern")
#define PIN_SETTING_DIRECTORY		("filename/directory")

FUGIO_NAMESPACE_BEGIN

class FilenameInterface
{
public:
	virtual ~FilenameInterface( void ) {}

	virtual QString filename( void ) const = 0;

	virtual void setFilename( const QString &pFilename ) = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::FilenameInterface, "com.bigfug.fugio.filename/1.0" )

#endif // INTERFACE_FILENAME_H

