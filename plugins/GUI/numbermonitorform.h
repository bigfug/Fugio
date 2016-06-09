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

protected:
	virtual void paintEvent( QPaintEvent *pEvent );

private:
	Ui::NumberMonitorForm *ui;

	QImage									 mImage;
	int										 mIndex;
	double									 mMin, mMax;
	QVector<int>							 mY;
};

#endif // NUMBERMONITORFORM_H
