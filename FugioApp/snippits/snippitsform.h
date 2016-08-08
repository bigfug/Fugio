#ifndef SNIPPITSFORM_H
#define SNIPPITSFORM_H

#include <QWidget>
#include <QFileSystemModel>
#include <QDebug>

namespace Ui {
class SnippitsForm;
}

class SnippitsForm : public QWidget
{
	Q_OBJECT

public:
	explicit SnippitsForm(QWidget *parent = 0);
	~SnippitsForm();

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
	Ui::SnippitsForm			*ui;

	QString						 mUsrRoot;

	QFileSystemModel			 mFileSystem;
	QFileSystemModel			 mUserSystem;

	QPoint						 mUserPoint;
};

#endif // SNIPPITSFORM_H
