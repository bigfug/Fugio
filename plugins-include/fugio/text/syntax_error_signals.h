#ifndef SYNTAX_ERROR_SIGNALS_H
#define SYNTAX_ERROR_SIGNALS_H

#include <QObject>

#include <fugio/global.h>

#include <fugio/text/syntax_highlighter_instance_interface.h>

FUGIO_NAMESPACE_BEGIN

class SyntaxErrorSignals : public QObject
{
	Q_OBJECT

public:
	SyntaxErrorSignals( void );

	virtual ~SyntaxErrorSignals( void );

signals:
	void syntaxErrorsUpdated( QList<fugio::SyntaxError> pSyntaxErrors );
};

FUGIO_NAMESPACE_END

#endif // SYNTAX_ERROR_SIGNALS_H
