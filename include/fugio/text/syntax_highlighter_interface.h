#ifndef INTERFACE_SYNTAX_HIGHLIGHTER_H
#define INTERFACE_SYNTAX_HIGHLIGHTER_H

#include <QtPlugin>

#include <fugio/global.h>

#define IID_SYNTAX_HIGHLIGHTER			(QUuid("{87253C53-28CA-4B8F-895E-AC513AE03335}"))

FUGIO_NAMESPACE_BEGIN
class SyntaxHighlighterFactoryInterface;
class SyntaxHighlighterInstanceInterface;
FUGIO_NAMESPACE_END

FUGIO_NAMESPACE_BEGIN

class SyntaxHighlighterInterface
{
public:
	virtual ~SyntaxHighlighterInterface( void ) {}

	virtual void registerSyntaxHighlighter( const QUuid &pUuid, const QString &pName, SyntaxHighlighterFactoryInterface *pFactory ) = 0;

	virtual void unregisterSyntaxHighlighter( const QUuid &pUuid ) = 0;

	virtual SyntaxHighlighterFactoryInterface *syntaxHighlighterFactory( const QUuid &pUuid ) const = 0;

	virtual SyntaxHighlighterInstanceInterface *syntaxHighlighterInstance( const QUuid &pUuid ) const = 0;

	typedef QPair<QUuid,QString> SyntaxHighlighterIdentity;

	virtual QList<SyntaxHighlighterIdentity> syntaxHighlighters( void ) const = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::SyntaxHighlighterInterface, "com.bigfug.fugio.syntax-highlighter/1.0" )

#endif // INTERFACE_SYNTAX_HIGHLIGHTER_H
