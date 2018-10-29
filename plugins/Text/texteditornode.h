#ifndef TEXTEDITORNODE_H
#define TEXTEDITORNODE_H

#include <QObject>

#include <QPlainTextEdit>
#include <QSyntaxHighlighter>
#include <QDockWidget>

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/variant_interface.h>

#include <fugio/core/uuid.h>

#include <fugio/nodecontrolbase.h>

#include <fugio/text/syntax_highlighter_instance_interface.h>
#include <fugio/text/syntax_error_interface.h>

class TextEditorForm;
class TextEditorNode;

class TextEditorNode : public fugio::NodeControlBase
{
	Q_OBJECT

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Adds a text editor window" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Text_Editor_(GUI)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit TextEditorNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~TextEditorNode( void ) Q_DECL_OVERRIDE {}

	typedef enum HighlighterType
	{
		HIGHLIGHT_NONE,
		HIGHLIGHT_DEFAULT,
		HIGHLIGHT_CUSTOM
	} HighlighterType;

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual QWidget *gui( void ) Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual bool initialise( void ) Q_DECL_OVERRIDE;

	virtual bool deinitialise( void ) Q_DECL_OVERRIDE;

	virtual void loadSettings( QSettings &pSettings ) Q_DECL_OVERRIDE;

	virtual void saveSettings( QSettings &pSettings ) const Q_DECL_OVERRIDE;

signals:
	void modified( bool pModified );

	void closeEditor( void );

	void bufferedChanged( bool pBuffered );

	void showEditor( void );

	void textChanged( QString pText );

	void syntaxHighlighterChanged( fugio::SyntaxHighlighterInstanceInterface *pSHEI );

	void syntaxErrorsChanged( QList<fugio::SyntaxError> pSyntaxErrors );

protected:
	bool isBuffered( void ) const;

	void checkHighlighter( void );

private slots:
	void editClicked( void );

	void setDockVisible( bool pVisible );

	void textModified( QString pText );
	void textUpdated( QString pText );

	void outputLinked( QSharedPointer<fugio::PinInterface> pPin );
	void outputUninked( QSharedPointer<fugio::PinInterface> pPin );

	void syntaxErrorsUpdated( QList<fugio::SyntaxError> pSyntaxErrors );

private:
	QSharedPointer<fugio::PinInterface>				 mPinInputBuffer;

	QSharedPointer<fugio::PinInterface>				 mPinOutputString;
	fugio::VariantInterface							*mValOutputString;

	Qt::DockWidgetArea								 mDockArea;
	bool											 mDockVisible;

	HighlighterType									 mHighlighterType;
	QUuid											 mHighlighterUuid;

	fugio::SyntaxErrorInterface						*mSyntaxInterface;
	fugio::SyntaxHighlighterInstanceInterface		*mHighlighter;
};

#endif // TEXTEDITORNODE_H
