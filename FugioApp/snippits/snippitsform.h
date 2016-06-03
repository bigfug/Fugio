#ifndef SNIPPITSFORM_H
#define SNIPPITSFORM_H

#include <QWidget>
#include <QFileSystemModel>

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
		return( mSnippitRoot );
	}

private slots:
	void on_mSnippits_doubleClicked(const QModelIndex &index);

private:
	Ui::SnippitsForm			*ui;

	QString						 mSnippitRoot;

	QFileSystemModel			 mFileSystem;
};

#endif // SNIPPITSFORM_H
