#ifndef CRONNODE_H
#define CRONNODE_H

#include <QObject>

#include <fugio/core/variant_interface.h>

#include <fugio/nodecontrolbase.h>

namespace Ui {
	class CronForm;
}

class CronNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Create a trigger for specific dates and times" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Cron" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit CronNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~CronNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual bool initialise( void ) Q_DECL_OVERRIDE;

	virtual QWidget *gui( void ) Q_DECL_OVERRIDE;

	virtual void loadSettings( QSettings &pSettings ) Q_DECL_OVERRIDE;
	virtual void saveSettings( QSettings &pSettings ) const Q_DECL_OVERRIDE;

private:
	bool checkYear( int pYear ) const;
	bool checkDayOfWeek( int pDayOfWeek ) const;
	bool checkMonth( int pMonth ) const;
	bool checkDayOfMonth( int pDayOfMonth ) const;
	bool checkHour( int pHour ) const;
	bool checkMinute( int pMinute ) const;

	bool parseYear( const QString &pText );
	bool parseDayOfWeek( const QString &pText );
	bool parseDayOfMonth( const QString &pText );
	bool parseMonth( const QString &pText );
	bool parseHour( const QString &pText );
	bool parseMinute( const QString &pText );

	bool parseText(const QString &pText, int pMin, int pMax, QList<QPair<int, int> > &PairList);

private slots:
	void frameStart( qint64 pTimeStamp );

	void parseYear( void );
	void parseDayOfWeek( void );
	void parseDayOfMonth( void );
	void parseMonth( void );
	void parseHour( void );
	void parseMinute( void );

private:
	QSharedPointer<fugio::PinInterface>	 mPinTrigger;

	qint64							 mLastTime;

	QList<QPair<int,int>>			 mYears;
	QList<QPair<int,int>>			 mDayOfWeek;
	QList<QPair<int,int>>			 mMonths;
	QList<QPair<int,int>>			 mDayOfMonth;
	QList<QPair<int,int>>			 mHours;
	QList<QPair<int,int>>			 mMinutes;

	QString							 mYearText;
	QString							 mMonthText;
	QString							 mDayOfMonthText;
	QString							 mDayOfWeekText;
	QString							 mHourText;
	QString							 mMinuteText;

	QWidget							*mGUI;
	Ui::CronForm					*mForm;
};

#endif // CRONNODE_H
