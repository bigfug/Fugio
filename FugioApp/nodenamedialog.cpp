#include "nodenamedialog.h"
#include "ui_nodenamedialog.h"

#include <QDebug>
#include <QSettings>
#include <QMetaClassInfo>

#include <fugio/fugio.h>
#include <fugio/global_interface.h>

#include "app.h"

#include <fugio/utils.h>

const static int NODE_HISTORY_COUNT = 10;

NodeNameDialog::NodeNameDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::NodeNameDialog)
{
	ui->setupUi(this);

	mNodeList.clear();

	QSettings				 Settings;

	int		HistoryCount = Settings.beginReadArray( "node-history" );

	for( int i = 0 ; i < qMin( NODE_HISTORY_COUNT, HistoryCount ) ; i++ )
	{
		Settings.setArrayIndex( i );

		QUuid NodeUuid = fugio::utils::string2uuid( Settings.value( "uuid" ).toString() );

		mNodeList << NodeUuid;
	}

	Settings.endArray();

	while( mNodeList.size() < NODE_HISTORY_COUNT )
	{
		mNodeList << QUuid();
	}

	setListHistory();
}

NodeNameDialog::~NodeNameDialog()
{
	QSettings				 Settings;

	Settings.beginWriteArray( "node-history" );

	for( int i = 0 ; i < NODE_HISTORY_COUNT ; i++ )
	{
		Settings.setArrayIndex( i );

		Settings.setValue( "uuid", fugio::utils::uuid2string( mNodeList.at( i ) ) );
	}

	Settings.endArray();

	delete ui;
}

void NodeNameDialog::on_lineEdit_textChanged( const QString &arg1 )
{
	if( arg1.isEmpty() )
	{
		setListHistory();
	}
	else
	{
		setListCompare( arg1 );
	}
}

void NodeNameDialog::setListHistory()
{
	ui->listWidget->clear();

	for( int i = 0 ; i < NODE_HISTORY_COUNT ; i++ )
	{
		QUuid		NodeUuid = mNodeList.at( i );

		if( !NodeUuid.isNull() && gApp->global().nodeMap().contains( NodeUuid ) )
		{
			const fugio::ClassEntry	&NodeData = gApp->global().nodeMap().value( NodeUuid );

			if( !NodeData.mFlags.testFlag( fugio::ClassEntry::Deprecated ) )
			{
				QListWidgetItem		*NodeItem = fillWidgetItem( NodeData.friendlyName(), NodeData.mUuid );

				if( NodeItem )
				{
					ui->listWidget->addItem( NodeItem );
				}
			}
		}
	}
}

void NodeNameDialog::setListCompare( const QString &pCmp )
{
	const GlobalPrivate::UuidClassEntryMap	&NodeList = gApp->global().nodeMap();

	ui->listWidget->clear();

	const QString					CmpLwr = pCmp.toLower();
	QPair<QString,QUuid>			Match;
	QMap<QString,QUuid>				MatchStart;
	QMap<QString,QUuid>				MatchContain;

	for( const fugio::ClassEntry &NodeEntry : NodeList.values() )
	{
		if( NodeEntry.mFlags.testFlag( fugio::ClassEntry::Deprecated ) )
		{
			continue;
		}

		const QString		LocalName = NodeEntry.friendlyName();
		const QString		LocalLower = LocalName.toLower();

		if( LocalLower == CmpLwr )
		{
			Match = QPair<QString,QUuid>( LocalName, NodeEntry.mUuid );
		}
		else if( LocalLower.startsWith( CmpLwr ) )
		{
			MatchStart.insert( LocalName, NodeEntry.mUuid );
		}
		else if( LocalLower.contains( CmpLwr ) )
		{
			MatchContain.insert( LocalName, NodeEntry.mUuid );
		}
	}

	if( !Match.second.isNull() )
	{
		QListWidgetItem		*NodeItem = fillWidgetItem( Match.first, Match.second );

		if( NodeItem )
		{
			ui->listWidget->addItem( NodeItem );
		}
	}

	if( !MatchStart.isEmpty() )
	{
		QStringList			NodeList = MatchStart.keys();

		std::sort( NodeList.begin(), NodeList.end() );

		for( const QString &S : NodeList )
		{
			QListWidgetItem		*NodeItem = fillWidgetItem( S, MatchStart.value( S ) );

			if( NodeItem )
			{
				ui->listWidget->addItem( NodeItem );
			}
		}
	}

	if( !MatchContain.isEmpty() )
	{
		QStringList			NodeList = MatchContain.keys();

		std::sort( NodeList.begin(), NodeList.end() );

		for( const QString &S : NodeList )
		{
			QListWidgetItem		*NodeItem = fillWidgetItem( S, MatchContain.value( S ) );

			if( NodeItem )
			{
				ui->listWidget->addItem( NodeItem );
			}
		}
	}
}

void NodeNameDialog::addToHistory( const QUuid &pNodeUuid )
{
	mNodeList.removeAll( pNodeUuid );

	mNodeList.prepend( pNodeUuid );

	while( mNodeList.size() < NODE_HISTORY_COUNT )
	{
		mNodeList << QUuid();
	}

	while( mNodeList.size() > NODE_HISTORY_COUNT )
	{
		mNodeList.pop_back();
	}
}

QListWidgetItem *NodeNameDialog::fillWidgetItem( const QString &pName, const QUuid &pUuid )
{
	QListWidgetItem		*NodeItem = new QListWidgetItem( pName );

	if( NodeItem )
	{
		NodeItem->setData( Qt::UserRole, pUuid );

		const QMetaObject	*NodeMeta = gApp->global().findNodeMetaObject( pUuid );

		if( NodeMeta )
		{
			int		InfIdx;

			if( ( InfIdx = NodeMeta->indexOfClassInfo( "Description" ) ) != -1 )
			{
				QMetaClassInfo	InfInf = NodeMeta->classInfo( InfIdx );

				NodeItem->setToolTip( InfInf.value() );
			}
		}
	}

	return( NodeItem );
}

void NodeNameDialog::on_listWidget_doubleClicked( const QModelIndex &index )
{
	mNodeUuid = index.data( Qt::UserRole ).toUuid();

	if( !mNodeUuid.isNull() )
	{
		addToHistory( mNodeUuid );

		accept();
	}
}

void NodeNameDialog::on_lineEdit_returnPressed()
{
	if( ui->lineEdit->text().isEmpty() )
	{
		mNodeUuid = mNodeList.first();

		if( !mNodeUuid.isNull() )
		{
			accept();
		}
	}
	else if( ui->listWidget->count() > 0 )
	{
		mNodeUuid = ui->listWidget->item( 0 )->data( Qt::UserRole ).toUuid();

		addToHistory( mNodeUuid );

		accept();
	}
}

void NodeNameDialog::on_listWidget_itemActivated( QListWidgetItem *item )
{
	mNodeUuid = item->data( Qt::UserRole ).toUuid();

	if( !mNodeUuid.isNull() )
	{
		addToHistory( mNodeUuid );

		accept();
	}
}
