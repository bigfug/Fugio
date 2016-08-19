#ifndef INTERFACE_OUTPUT_H
#define INTERFACE_OUTPUT_H

#include <QString>

#include <fugio/global.h>

class QWindow;

FUGIO_NAMESPACE_BEGIN

class OutputInterface
{
public:
	virtual ~OutputInterface( void ) {}

    virtual void screengrab( const QString &pFileName = QString() ) = 0;

    virtual void resize( int pWidth, int pHeight ) = 0;

    virtual bool showCursor( void ) const = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::OutputInterface, "com.bigfug.fugio.output/1.0" )

#endif // INTERFACE_OUTPUT_H
