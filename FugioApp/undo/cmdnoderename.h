#ifndef CMDNODERENAME_H
#define CMDNODERENAME_H

#include <QUndoCommand>
#include <QUuid>

#include <fugio/global_interface.h>
#include <fugio/context_interface.h>
#include <fugio/node_interface.h>

class CmdNodeRename : public QUndoCommand
{
public:
	explicit CmdNodeRename( QSharedPointer<fugio::NodeInterface> pNode, const QString &pNewVal )
		: mNode( pNode ), mValOld( pNode->name() ), mValNew( pNewVal )
	{
		setText( QObject::tr( "Rename Node" ) );
	}

	virtual ~CmdNodeRename( void )
	{

	}

	virtual void undo( void )
	{
		mNode->setName( mValOld );
	}

	virtual void redo( void )
	{
		mNode->setName( mValNew );
	}

private:
	QSharedPointer<fugio::NodeInterface>			 mNode;
	QString									 mValOld, mValNew;
};

#endif // CMDNODERENAME_H
