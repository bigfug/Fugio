#ifndef FIRSTTIMEWIZARD_H
#define FIRSTTIMEWIZARD_H

#include <QWizard>

class FirstTimeWizard : public QWizard
{
	Q_OBJECT

public:
	explicit FirstTimeWizard( void );

	virtual ~FirstTimeWizard( void );

	static int version( void )
	{
		return( 1 );
	}
};

#endif // FIRSTTIMEWIZARD_H
