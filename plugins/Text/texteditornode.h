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

class TextEditorForm;

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

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual QWidget *gui( void ) Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual bool initialise( void ) Q_DECL_OVERRIDE;

	virtual bool deinitialise( void ) Q_DECL_OVERRIDE;

	virtual void loadSettings( QSettings &pSettings ) Q_DECL_OVERRIDE;

	virtual void saveSettings( QSettings &pSettings ) const Q_DECL_OVERRIDE;

private:
	void setupTextEditor( QPlainTextEdit *pTextEdit );

signals:
	void modified( bool pModified );

protected:
	void checkHighlighter();

	bool isBuffered( void ) const;

private slots:
	void onEditClicked( void );

	void onTextModified( bool pModified );

	void onTextUpdate( void );

	void onTextPinUpdated( void );

	void dockSetVisible( bool pVisible );

	void textChanged( void );

private:
	QSharedPointer<fugio::PinInterface>		 mPinInputBuffer;

	QSharedPointer<fugio::PinInterface>		 mPinOutputString;
	fugio::VariantInterface					*mValOutputString;

	QDockWidget								*mDockWidget;
	TextEditorForm							*mTextEdit;

	Qt::DockWidgetArea						 mDockArea;
	bool									 mDockVisible;

	QUuid									 mPinUuid;

	QSyntaxHighlighter						*mHighlighter;
};

#endif // TEXTEDITORNODE_H
