#include "performanceform.h"
#include "ui_performanceform.h"

#include <QTimer>

#include "app.h"
#include <fugio/global_interface.h>
#include <fugio/context_interface.h>

PerformanceForm::PerformanceForm(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::PerformanceForm)
{
	ui->setupUi(this);

	QTimer::singleShot( 1000, this, SLOT(onTimer()) );
}

PerformanceForm::~PerformanceForm()
{
	delete ui;
}

void PerformanceForm::onTimer()
{
	ui->mPerfTree->clear();

	QList<QTreeWidgetItem *> items;

	for( fugio::ContextInterface *C : gApp->global().contexts() )
	{
		QList<fugio::PerfData>		PerfList = C->perfdata();

		for( const fugio::PerfData &PD : PerfList )
		{
			QStringList		SL;

			SL << PD.mNode;
			SL << PD.mName;
			SL << QString( "%1" ).arg( PD.mCount );
			SL << QString( "%1" ).arg( PD.mTime );

			items << new QTreeWidgetItem( (QTreeWidget *)0, SL );
		}

		ui->mPerfTree->insertTopLevelItems( 0, items );
	}

	QTimer::singleShot( 1000, this, SLOT(onTimer()) );
}
