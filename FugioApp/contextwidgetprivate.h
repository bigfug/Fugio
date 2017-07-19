#ifndef CONTEXTWIDGETPRIVATE_H
#define CONTEXTWIDGETPRIVATE_H

#include <QWidget>
#include <QSharedPointer>
#include <QUndoStack>
#include <QSplitter>
#include <QButtonGroup>
#include <QGraphicsScene>

#include <fugio/context_interface.h>
#include <fugio/playhead_interface.h>
#include <fugio/context_widget_interface.h>
#include <fugio/context_widget_signals.h>

#include "model/contextmodel.h"

namespace Ui {
class ContextWidgetPrivate;
}

class ContextView;
class NodeItem;

class ContextWidgetPrivate : public QWidget, public fugio::ContextWidgetInterface
{
    Q_OBJECT
	Q_INTERFACES( fugio::ContextWidgetInterface )

    friend class fugio::ContextWidgetSignals;

public:
    explicit ContextWidgetPrivate(QWidget *parent = 0);

    virtual ~ContextWidgetPrivate();

    void setContext( QSharedPointer<fugio::ContextInterface> pContext );

    QSharedPointer<fugio::ContextInterface> context( void );

    void timeout( void );

    ContextView *view( void )
    {
        return( mContextView );
    }

	QString filename( void ) const
	{
		return( mFileName );
	}

    //-------------------------------------------------------------------------
	// fugio::IContextWidget

    virtual QWidget *widget( void )
    {
        return( this );
    }

    virtual void addWidget( QWidget *pWidget );

    //-------------------------------------------------------------------------
	// fugio::IContextWidget interface

    virtual QUndoStack *undoStack();
	virtual fugio::ContextWidgetSignals *qobject();

private:
	void generateRecoveryFilename();

	// QWidget interface
protected:
	virtual void paintEvent( QPaintEvent * );

signals:
    void contextFilenameChanged( const QString &pFileName );

	void groupLinkActivated( const QString pLink );

public slots:
    void userSave( void );

    void userSaveAs( void );

	void userSaveImage( void );

	void load( const QString &pFileName );

    void cut( void );

    void copy( void );

    void paste( void );

	void saveSelectedTo( const QString &pFileName );

	void groupSelected( void );
	void ungroupSelected( void );

	void groupParent( void );

	void loadRecovery( const QString &pFileName );

	void saveRecovery( void );

	void setWindowTitleFromFileName( const QString &pFileName );

	void setGroupWidgetText( const QString pText );

private slots:
    void on_mButtonPlay_clicked();

    void on_mButtonStop_clicked();

    void on_mTimeSlider_sliderMoved(int position);

    void onContextDurationChanged( qreal pDuration );

    //	void onNodeInspection( NodeItem *pNodeItem );

    void on_mTimeText_textEdited(const QString &arg1);

	void on_mGroupLinks_linkActivated(const QString &link);

private:
    Ui::ContextWidgetPrivate				*ui;
    QSplitter								*mSplitter;
    ContextView								*mContextView;
    QButtonGroup							*mButtonGroup;
	QSharedPointer<fugio::ContextInterface>	 mContext;
    QUndoStack								 mUndoStack;
	fugio::ContextWidgetSignals				 mContextWidget;
	QString									 mFileName;
	QString									 mRecoveryFilename;
};

#endif // CONTEXTWIDGETPRIVATE_H
