#ifndef QMLNODE_H
#define QMLNODE_H

#include <QObject>
#include <QQuickView>
#include <QQuickWidget>
#include <QQuickImageProvider>

#include <fugio/nodecontrolbase.h>

#include <fugio/text/syntax_error_interface.h>

#include "qmlnode.h"

class QMLWidget;
class QDockWidget;
class QMLInterfaceNode;

class ImageProvider : public QQuickImageProvider
{
public:
	ImageProvider( QMLInterfaceNode *pNode ) : QQuickImageProvider( QQmlImageProviderBase::Image ), mNode( pNode )
	{

	}

	virtual QImage requestImage( const QString &id, QSize *size, const QSize &requestedSize ) Q_DECL_OVERRIDE;

private:
	QMLInterfaceNode		*mNode;
};

class QMLInterfaceNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "QML" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/QML" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE QMLInterfaceNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~QMLInterfaceNode( void ) {}

	// NodeControlInterface interface

	virtual bool initialise() Q_DECL_OVERRIDE;
	virtual bool deinitialise() Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual QList<QUuid> pinAddTypesInput() const Q_DECL_OVERRIDE;
	virtual QList<QUuid> pinAddTypesOutput() const Q_DECL_OVERRIDE;

	void loadSettings( QSettings &pSettings ) Q_DECL_OVERRIDE;

	void saveSettings( QSettings &pSettings ) const Q_DECL_OVERRIDE;

	virtual bool canAcceptPin( fugio::PinInterface *pPin ) const Q_DECL_OVERRIDE;

	void registerOwnership( QObject *O );

	QImage imageRequest( const QString &pId, QSize *pSize, const QSize &pRequestedSize );

signals:
	void signalInputsUpdated( qint64 pTimeStamp );

private slots:
	void nodeNameUpdated( const QString &pName );

	void viewStatusChanged( QQuickView::Status pStatus );

	void widgetStatusChanged( QQuickWidget::Status pStatus );

	void dockWidgetLocationUpdated( Qt::DockWidgetArea pArea );

private:
	void processObject( QObject *O );

	void processErrors( const QList<QQmlError> &pErrLst ) const;

protected:
	QSharedPointer<fugio::PinInterface>		 mPinSource;
	fugio::SyntaxErrorInterface				*mValInputSource;

	QDockWidget								*mDockWidget;
	QMLWidget								*mWidget;
	QQuickView								*mQuickView;
	QMLNode									 mQML;
	Qt::DockWidgetArea						 mDockArea;
};

#endif // QMLNODE_H
