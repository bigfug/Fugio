#ifndef CLIENTCONSOLE_H
#define CLIENTCONSOLE_H

#include <QObject>
#include <QTimer>

class ClientConsole : public QObject
{
	Q_OBJECT

public:
	explicit ClientConsole(QObject *parent = nullptr);

private:
	void setTimer( void );

private slots:
	void timeout( void );

private:
	QTimer				 mTimer;
};

#endif // CLIENTCONSOLE_H
