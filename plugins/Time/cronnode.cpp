#include "cronnode.h"

#include <QDateTime>
#include <QDebug>
#include <QWidget>

#include <fugio/node_interface.h>
#include <fugio/context_interface.h>
#include <fugio/context_signals.h>
#include <fugio/core/uuid.h>

#include "ui_cronform.h"

CronNode::CronNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mLastTime( -1 ), mGUI( nullptr ), mForm( nullptr )
{
	pinOutput<fugio::PinControlInterface *>( "Trigger", mPinTrigger, PID_TRIGGER );

	mYears << QPair<int,int>( 1970, 2099 );
	mDayOfWeek << QPair<int,int>( 0, 6 );
	mMonths << QPair<int,int>( 0, 11 );
	mDayOfMonth << QPair<int,int>( 0, 31 );
	mHours << QPair<int,int>( 0, 23 );
	mMinutes << QPair<int,int>( 0, 59 );
}

bool CronNode::initialise( void )
{
	if( !fugio::NodeControlBase::initialise() )
	{
		return( false );
	}

	connect( mNode->context()->qobject(), SIGNAL(frameStart(qint64)), this, SLOT(frameStart(qint64)) );

	return( true );
}

bool CronNode::checkYear( int pYear ) const
{
	for( const QPair<int,int> &P : mYears )
	{
		if( pYear >= P.first && pYear <= P.second )
		{
			return( true );
		}
	}

	return( false );
}

bool CronNode::checkDayOfWeek( int pDayOfWeek ) const
{
	for( const QPair<int,int> &P : mDayOfWeek )
	{
		if( pDayOfWeek >= P.first && pDayOfWeek <= P.second )
		{
			return( true );
		}
	}

	return( false );
}

bool CronNode::checkMonth( int pMonth ) const
{
	for( const QPair<int,int> &P : mMonths )
	{
		if( pMonth >= P.first && pMonth <= P.second )
		{
			return( true );
		}
	}

	return( false );
}

bool CronNode::checkDayOfMonth( int pDayOfMonth ) const
{
	for( const QPair<int,int> &P : mDayOfMonth )
	{
		if( pDayOfMonth >= P.first && pDayOfMonth <= P.second )
		{
			return( true );
		}
	}

	return( false );
}

bool CronNode::checkHour( int pHour ) const
{
	for( const QPair<int,int> &P : mHours )
	{
		if( pHour >= P.first && pHour <= P.second )
		{
			return( true );
		}
	}

	return( false );
}

bool CronNode::checkMinute( int pMinute ) const
{
	for( const QPair<int,int> &P : mMinutes )
	{
		if( pMinute >= P.first && pMinute <= P.second )
		{
			return( true );
		}
	}

	return( false );
}

void CronNode::frameStart( qint64 pTimeStamp )
{
	qint64			ClockTime = pTimeStamp;

	// Remove milliseconds < 1000

	ClockTime = ClockTime - ( ClockTime % ( 60 * 1000 ) );

	if( ClockTime == mLastTime )
	{
		return;
	}

	mLastTime = ClockTime;

	// Check if the time matches

	QDateTime		DT = QDateTime::fromMSecsSinceEpoch( ClockTime );

	if( !DT.isValid() )
	{
		return;
	}

	//qDebug() << DT.toString();

	if( !checkYear( DT.date().year() ) )
	{
		return;
	}

	if( !checkMonth( DT.date().month() ) )
	{
		return;
	}

	if( !checkDayOfMonth( DT.date().day() ) )
	{
		return;
	}

	if( !checkDayOfWeek( DT.date().dayOfWeek() ) )
	{
		return;
	}

	if( !checkHour( DT.time().hour() ) )
	{
		return;
	}

	if( !checkMinute( DT.time().minute() ) )
	{
		return;
	}

	mNode->context()->pinUpdated( mPinTrigger );
}

bool CronNode::parseText( const QString &pText, int pMin, int pMax, QList<QPair<int,int>> &PairList )
{
	QStringList				TextPart = pText.trimmed().split( ',' );

	if( TextPart.isEmpty() )
	{
		PairList << QPair<int,int>( pMin, pMax );

		return( true );
	}

	for( QString P : TextPart )
	{
		QStringList			PairPart = P.trimmed().split( '-' );

		if( PairPart.size() > 2 )
		{
			return( false );
		}

		if( PairPart.size() == 2 )
		{
			bool	o1, o2;

			int		y1 = PairPart.first().toInt( &o1 );
			int		y2 = PairPart.last().toInt( &o2 );

			if( !o1 || !o2 )
			{
				return( false );
			}

			y1 = qBound( pMin, y1, pMax );
			y2 = qBound( pMin, y2, pMax );

			if( y1 > y2 )
			{
				return( false );
			}

			PairList << QPair<int,int>( y1, y2 );
		}
		else
		{
			bool	o1;

			int		y1 = PairPart.first().toInt( &o1 );

			if( !o1 )
			{
				return( false );
			}

			y1 = qBound( pMin, y1, pMax );

			PairList << QPair<int,int>( y1, y1 );
		}
	}

	return( true );
}

bool CronNode::parseYear( const QString &pText )
{
	QList<QPair<int,int>>	PairList;

	if( parseText( pText, 1970, 2099, PairList) )
	{
		mYears = PairList;

		qDebug() << "Years" << mYears;

		return( true );
	}

	return( false );
}

void CronNode::parseYear( void )
{
	if( !parseYear( mForm->mYear->text() ) )
	{
		qApp->beep();
	}
	else
	{
		mYearText = mForm->mYear->text();
	}
}

bool CronNode::parseMonth( const QString &pText )
{
	QList<QPair<int,int>>	PairList;

	if( parseText( pText, 0, 11, PairList ) )
	{
		mMonths = PairList;

		qDebug() << "Months" << mMonths;

		return( true );
	}

	return( false );
}

void CronNode::parseMonth()
{
	if( !parseMonth( mForm->mMonth->text() ) )
	{
		qApp->beep();
	}
	else
	{
		mDayOfWeekText = mForm->mDayOfWeek->text();
	}
}

bool CronNode::parseDayOfMonth( const QString &pText )
{
	QList<QPair<int,int>>	PairList;

	if( parseText( pText, 0, 31, PairList ) )
	{
		mDayOfMonth = PairList;

		qDebug() << "DayOfMonth" << mDayOfMonth;

		return( true );
	}

	return( false );
}

void CronNode::parseDayOfMonth()
{
	if( !parseDayOfMonth( mForm->mDayOfMonth->text() ) )
	{
		qApp->beep();
	}
	else
	{
		mDayOfMonthText = mForm->mDayOfMonth->text();
	}
}

bool CronNode::parseDayOfWeek( const QString &pText )
{
	QList<QPair<int,int>>	PairList;

	if( parseText( pText, 0, 6, PairList ) )
	{
		mDayOfWeek = PairList;

		qDebug() << "DayOfWeek" << mDayOfWeek;

		return( true );
	}

	return( false );
}

void CronNode::parseDayOfWeek()
{
	if( !parseDayOfWeek( mForm->mDayOfWeek->text() ) )
	{
		qApp->beep();
	}
	else
	{
		mDayOfWeekText = mForm->mDayOfWeek->text();
	}
}

bool CronNode::parseHour( const QString &pText )
{
	QList<QPair<int,int>>	PairList;

	if( parseText( pText, 0, 23, PairList ) )
	{
		mHours = PairList;

		qDebug() << "Hours" << mHours;

		return( true );
	}

	return( false );
}

void CronNode::parseHour()
{
	if( !parseHour( mForm->mHour->text() ) )
	{
		qApp->beep();
	}
	else
	{
		mHourText = mForm->mHour->text();
	}
}

bool CronNode::parseMinute( const QString &pText )
{
	QList<QPair<int,int>>	PairList;

	if( parseText( pText, 0, 59, PairList ) )
	{
		mMinutes = PairList;

		qDebug() << "Minutes" << mMinutes;

		return( true );
	}

	return( false );
}

void CronNode::parseMinute()
{
	if( !parseMinute( mForm->mMinute->text() ) )
	{
		qApp->beep();
	}
	else
	{
		mMinuteText = mForm->mMinute->text();
	}
}

QWidget *CronNode::gui()
{
	if( !mGUI )
	{
		if( ( mGUI = new QWidget() ) )
		{
			if( ( mForm = new Ui::CronForm() ) )
			{
				mForm->setupUi( mGUI );

				mForm->mYear->setText( mYearText );
				mForm->mMonth->setText( mMonthText );
				mForm->mDayOfWeek->setText( mDayOfWeekText );
				mForm->mDayOfMonth->setText( mDayOfMonthText );
				mForm->mHour->setText( mHourText );
				mForm->mMinute->setText( mMinuteText );

				connect( mForm->mYear, SIGNAL(editingFinished()), this, SLOT(parseYear()) );
				connect( mForm->mMonth, SIGNAL(editingFinished()), this, SLOT(parseMonth()) );
				connect( mForm->mDayOfWeek, SIGNAL(editingFinished()), this, SLOT(parseDayOfWeek()) );
				connect( mForm->mDayOfMonth, SIGNAL(editingFinished()), this, SLOT(parseDayOfMonth()) );
				connect( mForm->mHour, SIGNAL(editingFinished()), this, SLOT(parseHour()) );
				connect( mForm->mMinute, SIGNAL(editingFinished()), this, SLOT(parseMinute()) );
			}
		}
	}

	return( mGUI );
}

void CronNode::loadSettings( QSettings &pSettings )
{
	mYearText      = pSettings.value( "year", mYearText ).toString();
	mMonthText = pSettings.value( "month", mMonthText ).toString();
	mDayOfWeekText = pSettings.value( "day-of-week", mDayOfWeekText ).toString();
	mDayOfMonthText = pSettings.value( "day-of-month", mDayOfMonthText ).toString();
	mHourText = pSettings.value( "hour", mHourText ).toString();
	mMinuteText = pSettings.value( "minute", mMinuteText ).toString();

	parseYear( mYearText );
	parseMonth( mMonthText );
	parseDayOfWeek( mDayOfWeekText );
	parseDayOfMonth( mDayOfMonthText );
	parseHour( mHourText );
	parseMinute( mMinuteText );

	if( mForm )
	{
		mForm->mYear->setText( mYearText );
		mForm->mMonth->setText( mMonthText );
		mForm->mDayOfWeek->setText( mDayOfWeekText );
		mForm->mDayOfMonth->setText( mDayOfMonthText );
		mForm->mHour->setText( mHourText );
		mForm->mMinute->setText( mMinuteText );
	}
}

void CronNode::saveSettings( QSettings &pSettings ) const
{
	pSettings.setValue( "year", mYearText );
	pSettings.setValue( "month", mMonthText );
	pSettings.setValue( "day-of-week", mDayOfWeekText );
	pSettings.setValue( "day-of-month", mDayOfMonthText );
	pSettings.setValue( "hour", mHourText );
	pSettings.setValue( "minute", mMinuteText );
}
