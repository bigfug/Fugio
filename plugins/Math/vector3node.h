#ifndef VECTOR3NODE_H
#define VECTOR3NODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

FUGIO_NAMESPACE_BEGIN
class VariantInterface;
FUGIO_NAMESPACE_END

class Vector3Node : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Vector3_(GUI)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE Vector3Node( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~Vector3Node( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual QWidget *gui() Q_DECL_OVERRIDE;

	virtual void loadSettings( QSettings &pSettings ) Q_DECL_OVERRIDE;
	virtual void saveSettings( QSettings &pSettings ) const Q_DECL_OVERRIDE;

signals:
	void updatedX( double pValue );
	void updatedY( double pValue );
	void updatedZ( double pValue );

protected slots:
	void updateX( double pValue );
	void updateY( double pValue );
	void updateZ( double pValue );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinVec3;

	fugio::VariantInterface						*mVec3;
};

#endif // VECTOR3NODE_H
