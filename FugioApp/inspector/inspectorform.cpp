#include "inspectorform.h"
#include "app.h"
#include "ui_inspectorform.h"

#include <QDebug>
#include <QLabel>
#include <QLineEdit>

#include <fugio/pin_interface.h>
#include <fugio/inspector_widget_interface.h>

InspectorForm::InspectorForm(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::InspectorForm)
{
	ui->setupUi(this);
}

InspectorForm::~InspectorForm()
{
	delete ui;
}

void InspectorForm::inspectNode( QSharedPointer<fugio::ContextInterface> pContext, QUuid pNodeId )
{
	qDebug() << "inspectNode" << pNodeId;

	if( ui->mFormLayout )
	{
		QLayoutItem* item;
		while( !ui->mFormLayout->isEmpty() && ( item = ui->mFormLayout->takeAt( 0 ) ) != NULL )
		{
			delete item->widget();
			delete item;
		}
		delete ui->mFormLayout;
	}

	ui->mFormLayout = new QFormLayout();
	ui->mFormLayout->setObjectName(QString::fromUtf8("mFormLayout"));

	ui->verticalLayout->addLayout(ui->mFormLayout);

	if( pNodeId.isNull() )
	{
		mContext.clear();
		mNode.clear();

		return;
	}

	mContext = pContext;

	mNode = mContext->findNode( pNodeId );

	if( mNode.isNull() )
	{
		return;
	}

	for( auto &p : mNode->enumInputPins() )
	{
		QLabel		*Label = new QLabel( p->name() );
		QWidget		*Editor = Q_NULLPTR;

		if( !p->inputTypes().isEmpty() )
		{
			const QMetaObject *EditorMetaObject = gApp->global().findEditorMetaObject( p->inputTypes().first() );

			if( EditorMetaObject )
			{
				QObject		*ClassInstance = EditorMetaObject->newInstance();

				if( ClassInstance )
				{
					fugio::InspectorWidgetInterface *Interface = qobject_cast<fugio::InspectorWidgetInterface *>( ClassInstance );

					if( Interface )
					{
						Editor = Interface->inspectorWidget( p->value().toString() );
					}
				}
			}
		}

		if( !Editor )
		{
			QLineEdit	*LineEdit = new QLineEdit( p->value().toString() );

			connect( LineEdit, &QLineEdit::textChanged, [=]( const QString &s )
					{
						p->setValue( s );
					});

			Editor = LineEdit;
		}


		ui->mFormLayout->addRow( Label, Editor );
	}
}
