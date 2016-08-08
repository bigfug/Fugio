#ifndef SNIPPETSFORM_H
#define SNIPPETSFORM_H

#include <QWidget>
#include <QFileSystemModel>
#include <QDebug>

namespace Ui {
class SnippetsForm;
}

class SnippetsForm : public QWidget
{
	Q_OBJECT

public:
	explicit SnippetsForm(QWidget *parent = 0);

	~SnippetsForm();

	QString root( void ) const
	{
		return( mUsrRoot );
	}

private slots:
	void on_mSnippetSource_currentIndexChanged(int index);

	void on_mUser_customContextMenuRequested(const QPoint &pos);

	void userDeleteSelected( void );

	void userNewFolder( void );

private:
	Ui::SnippetsForm			*ui;

	QString						 mUsrRoot;

	QFileSystemModel			 mFileSystem;
	QFileSystemModel			 mUserSystem;

	QPoint						 mUserPoint;
};

#endif // SNIPPETSFORM_H
