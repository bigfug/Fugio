#ifndef SYNTAX_ERROR_INTERFACE_H
#define SYNTAX_ERROR_INTERFACE_H

#include <QObject>

#include <fugio/global.h>

#include <fugio/text/syntax_highlighter_instance_interface.h>
#include <fugio/text/syntax_error_signals.h>

FUGIO_NAMESPACE_BEGIN

class SyntaxErrorInterface
{
public:
	virtual ~SyntaxErrorInterface( void ) {}

	virtual void setSyntaxErrors( QList<fugio::SyntaxError> pSyntaxErrors ) = 0;

	virtual void clearSyntaxErrors( void ) = 0;

	virtual QList<fugio::SyntaxError> syntaxErrors( void ) = 0;

	virtual fugio::SyntaxErrorSignals *syntaxErrorSignals( void ) = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::SyntaxErrorInterface, "com.bigfug.fugio.syntax-error/1.0" )

#endif // SYNTAX_ERROR_INTERFACE_H
