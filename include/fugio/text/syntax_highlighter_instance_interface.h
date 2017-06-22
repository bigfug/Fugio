#ifndef SYNTAX_HIGHLIGHTER_INSTANCE_INTERFACE_H
#define SYNTAX_HIGHLIGHTER_INSTANCE_INTERFACE_H

#include <QtPlugin>
#include <QSyntaxHighlighter>

#include <fugio/global.h>

#define IID_SYNTAX_HIGHLIGHTER_INSTANCE			(QUuid("{E8D7D957-61EB-43cd-A51E-5236622AF5A9}"))

FUGIO_NAMESPACE_BEGIN

typedef struct SyntaxError
{
	int			mLineStart;
	int			mLineEnd;
	int			mColumnStart;
	int			mColumnEnd;
	QString		mError;
} SyntaxError;

class SyntaxHighlighterInstanceInterface
{
public:
	virtual ~SyntaxHighlighterInstanceInterface( void ) {}

	virtual QSyntaxHighlighter *highlighter( QTextDocument *pDocument ) = 0;

	virtual QList<SyntaxError> errorList( void ) const = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::SyntaxHighlighterInstanceInterface, "com.bigfug.fugio.syntax-highlighter-instance/1.0" )

#endif // SYNTAX_HIGHLIGHTER_INSTANCE_INTERFACE_H
