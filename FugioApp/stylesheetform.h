#ifndef STYLESHEETFORM_H
#define STYLESHEETFORM_H

#include <QWidget>

namespace Ui {
class StyleSheetForm;
}

class StyleSheetForm : public QWidget
{
	Q_OBJECT

public:
	explicit StyleSheetForm(QWidget *parent = 0);

	~StyleSheetForm();

	inline bool isChanged( void ) const
	{
		return( mChanged );
	}

public slots:
	void save( void )
	{
		on_mButtonStyleSave_clicked();
	}

	void stylesApply( void );

private slots:
	void on_mButtonStyleUpdate_clicked();

	void on_mButtonStyleLoad_clicked();

	void on_mButtonStyleSave_clicked();

	void stylesLoad( QString pFileName );
	void stylesSave( QString pFileName );

	void on_mStyleSheet_textChanged();

private:
	Ui::StyleSheetForm	*ui;
	QString				 mFileName;
	bool				 mChanged;
};

#endif // STYLESHEETFORM_H
