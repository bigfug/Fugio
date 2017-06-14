#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QTimer>
#include <QHostAddress>
#include <QElapsedTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private slots:
	void on_mButton_clicked(bool checked);

	void sendTime( void );

private:
	Ui::MainWindow *ui;
	QUdpSocket *udpSocket;
	QTimer *timer;
	QHostAddress groupAddress;
	QElapsedTimer	mUniverseTimer;
};

#endif // MAINWINDOW_H
