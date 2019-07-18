#include "nodelistform.h"
#include "ui_nodelistform.h"

#include <QMetaClassInfo>

#include "app.h"

NodeListForm::NodeListForm(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::NodeListForm)
{
	ui->setupUi(this);

	connect( gApp->global().qobject(), SIGNAL(nodeClassAdded(fugio::ClassEntry)), this, SLOT(addNodeClass(fugio::ClassEntry)) );
}

NodeListForm::~NodeListForm()
{
	delete ui;
}

void NodeListForm::setNodelistUpdate( bool pEnabled )
{
	ui->mNodeList->setUpdatesEnabled( pEnabled );
}

void NodeListForm::on_mNodeList_itemClicked( QListWidgetItem *pItem )
{
	QString		NodeDesc = QString( "<h1>%1</h1>" ).arg( pItem->text() );

	const QMetaObject		*NodeMeta = gApp->global().findNodeMetaObject( pItem->data( Qt::UserRole ).toUuid() );

	if( NodeMeta )
	{
		int		InfIdx;

		if( ( InfIdx = NodeMeta->indexOfClassInfo( "Description" ) ) != -1 )
		{
			QMetaClassInfo	InfInf = NodeMeta->classInfo( InfIdx );

			NodeDesc += QString( "<h3>Description</h3><p>%1</p>" ).arg( InfInf.value() );
		}

		if( ( InfIdx = NodeMeta->indexOfClassInfo( "Version" ) ) != -1 )
		{
			QMetaClassInfo	InfInf = NodeMeta->classInfo( InfIdx );

			NodeDesc += QString( "<p><strong>Version:</strong> %1</p>" ).arg( InfInf.value() );
		}

		if( ( InfIdx = NodeMeta->indexOfClassInfo( "URL" ) ) != -1 )
		{
			QMetaClassInfo	InfInf = NodeMeta->classInfo( InfIdx );

			NodeDesc += QString( "<p><strong>URL:</strong> <a href=\"%1?utm_source=fugio&utm_medium=node-info\" target=\"fugio\">%1</a></p>" ).arg( InfInf.value() );
		}

		if( ( InfIdx = NodeMeta->indexOfClassInfo( "Author" ) ) != -1 )
		{
			QMetaClassInfo	InfInf = NodeMeta->classInfo( InfIdx );

			NodeDesc += QString( "<p><strong>Author:</strong> %1</p>" ).arg( InfInf.value() );
		}

		if( ( InfIdx = NodeMeta->indexOfClassInfo( "Contact" ) ) != -1 )
		{
			QMetaClassInfo	InfInf = NodeMeta->classInfo( InfIdx );

			NodeDesc += QString( "<p><strong>Contact:</strong> <a href=\"%1?utm_source=fugio&utm_medium=node-info\" target=\"fugio-help\">%1</a></p>" ).arg( InfInf.value() );
		}
	}

	ui->mNodeInfo->setHtml( NodeDesc );
}


void NodeListForm::addNodeClass( const fugio::ClassEntry &pClassEntry )
{
	QString		LocalName = pClassEntry.mName;

	if( !pClassEntry.mFlags.testFlag( fugio::ClassEntry::Deprecated ) )
	{
		if( !pClassEntry.mGroup.isEmpty() )
		{
			if( !mNodeGroups.contains( pClassEntry.mGroup ) )
			{
				mNodeGroups.append( pClassEntry.mGroup );

				std::sort( mNodeGroups.begin(), mNodeGroups.end() );

				ui->mNodeGroup->clear();

				ui->mNodeGroup->addItem( "[ALL]" );

				ui->mNodeGroup->addItems( mNodeGroups );
			}

			LocalName = QString( "%1 (%2)" ).arg( LocalName ).arg( pClassEntry.mGroup );
		}
	}

	QListWidgetItem			*NodeItem = new QListWidgetItem( LocalName );

	if( NodeItem == 0 )
	{
		return;
	}

	NodeItem->setData( Qt::UserRole, pClassEntry.mUuid );

	int		DscIdx = pClassEntry.mMetaObject->indexOfClassInfo( "DESC" );

	if( DscIdx != -1 )
	{
		QMetaClassInfo	DscInf = pClassEntry.mMetaObject->classInfo( DscIdx );

		NodeItem->setToolTip( LocalName + "\n\n" + DscInf.value() );
	}

	ui->mNodeList->addItem( NodeItem );
}

void NodeListForm::on_mNodeList_itemSelectionChanged()
{
	on_mNodeList_itemClicked( ui->mNodeList->currentItem() );
}


void NodeListForm::on_mNodeGroup_currentIndexChanged( const QString &pGroup )
{
	for( int i = 0 ; i < ui->mNodeList->count() ; i++ )
	{
		QListWidgetItem		*I = ui->mNodeList->item( i );

		if( !I )
		{
			continue;
		}

		QUuid				 UUID = I->data( Qt::UserRole ).toUuid();

		if( UUID.isNull() )
		{
			continue;
		}

		const fugio::ClassEntry	CLASS = gApp->global().nodeMap().value( UUID );

		if( pGroup == ui->mNodeGroup->itemText( 0 ) )
		{
			I->setHidden( false );
		}
		else if( CLASS.mGroup == pGroup )
		{
			I->setHidden( false );
		}
		else
		{
			I->setHidden( true );
		}

		if( !I->isHidden() && !ui->mNodeSearch->text().isEmpty() )
		{
			if( !I->text().contains(  ui->mNodeSearch->text(), Qt::CaseInsensitive ) )
			{
				I->setHidden( true );
			}
		}
	}
}

void NodeListForm::on_mNodeSearch_textChanged( const QString & )
{
	on_mNodeGroup_currentIndexChanged( ui->mNodeGroup->currentText() );
}
