#ifndef NUMBERMONITORFORM_H
#define NUMBERMONITORFORM_H

#include <QWidget>

namespace Ui {
class NumberMonitorForm;
}

class NumberMonitorForm : public QWidget
{
	Q_OBJECT

public:
	explicit NumberMonitorForm(QWidget *parent = 0);
	~NumberMonitorForm();

	void value( const QList< QPair<QColor,qreal> > &pValLst );

public slots:
	void setNodeName( const QString &pName );

protected:
	virtual void paintEvent( QPaintEvent *pEvent );

	void contextMenuEvent( QContextMenuEvent *pEvent ) Q_DECL_OVERRIDE;

private slots:
	void saveImage( void );

private:
	Ui::NumberMonitorForm *ui;

	QImage									 mImage;
	int										 mIndex;
	double									 mMin, mMax;
	QVector<int>							 mY;
	QString									 mSaveImageDir;
};

#endif // NUMBERMONITORFORM_H
