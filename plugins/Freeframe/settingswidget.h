#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>

namespace Ui {
class SettingsWidget;
}

class QListWidgetItem;

class SettingsWidget : public QWidget
{
	Q_OBJECT

public:
	explicit SettingsWidget(QWidget *parent = 0);
	~SettingsWidget();

	void setPluginPaths( QStringList pPaths );

	QStringList pluginPaths( void ) const;

private slots:
	void on_mButtonAdd_clicked();

	void on_mButtonRemove_clicked();

	void on_mPathList_itemClicked( QListWidgetItem *item );

private:
	Ui::SettingsWidget *ui;
};

#endif // SETTINGSWIDGET_H
