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

private slots:
	void on_mSnippetSource_currentIndexChanged(int index);

	void on_mUser_customContextMenuRequested(const QPoint &pos);

	void userDeleteSelected( void );

	void userNewFolder( void );

	void userSnippetsDirectoryChanged( const QString &pDirectory );

private:
	Ui::SnippetsForm			*ui;

	QFileSystemModel			 mFileSystem;
	QFileSystemModel			 mUserSystem;

	QPoint						 mUserPoint;
};

#endif // SNIPPETSFORM_H
