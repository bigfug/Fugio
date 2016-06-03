#ifndef PERFORMANCEFORM_H
#define PERFORMANCEFORM_H

#include <QWidget>

namespace Ui {
class PerformanceForm;
}

class PerformanceForm : public QWidget
{
	Q_OBJECT

public:
	explicit PerformanceForm(QWidget *parent = 0);

	~PerformanceForm();

protected slots:
	void onTimer( void );

private:
	Ui::PerformanceForm *ui;
};

#endif // PERFORMANCEFORM_H
