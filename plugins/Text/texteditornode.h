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

class TextEditorForm;
class TextEditorNode;

class TextEditorGui : public QObject
{
	Q_OBJECT

public:
	TextEditorGui( TextEditorNode *pNode )
		: mNode( pNode ), mDockWidget( Q_NULLPTR ), mTextEdit( Q_NULLPTR ),
		mHighlighter( Q_NULLPTR )
	{

	}

	Q_INVOKABLE void initialise( void );

private:
	void setupTextEditor( QPlainTextEdit *pTextEdit );

	void checkHighlighter();

private:
	TextEditorNode		*mNode;
	QDockWidget										*mDockWidget;
	TextEditorForm									*mTextEdit;
	fugio::SyntaxHighlighterInstanceInterface		*mHighlighter;

};

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

	virtual ~TextEditorNode( void );

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

	void checkHighlighter( void );

protected:
	bool isBuffered( void ) const;

private slots:
	void onEditClicked( void );

	void onTextModified( bool pModified );

	void onTextUpdate( void );

	void onTextPinUpdated( void );

	void dockSetVisible( bool pVisible );

	void textChanged( void );

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
};

#endif // TEXTEDITORNODE_H
