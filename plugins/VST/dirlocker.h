#ifndef DIRLOCKER_H
#define DIRLOCKER_H

#include <QDir>

class DirLocker
{
public:
	DirLocker( const QString &pDir )
		: mNewDir( pDir ), mOldDir( QDir::currentPath() )
	{
		QDir::setCurrent( mNewDir );
	}

	~DirLocker( void )
	{
		QDir::setCurrent( mOldDir );
	}

private:
	QString			mNewDir;
	QString			mOldDir;
};

#endif // DIRLOCKER_H
