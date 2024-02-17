#ifndef PLUGINSFORM_H
#define PLUGINSFORM_H

#include <QWidget>

namespace Ui {
class PluginsForm;
}

#include <pluginmanager.h>

class PluginsForm : public QWidget
{
    Q_OBJECT

public:
    explicit PluginsForm(QWidget *parent = nullptr);
    ~PluginsForm();

private slots:
	void on_mSourceList_itemSelectionChanged();

	void on_mButtonSourceUpdateAll_clicked();

	void on_mButtonSourceRemove_clicked();

	void on_mButtonSourceUpdate_clicked();

	void rebuildPluginInformation( void );

	void on_mButtonSourceAddFile_clicked();

	void on_mButtonSourceAddUrl_clicked();

	void on_mButtonApply_clicked();

private:
    Ui::PluginsForm *ui;
	PluginCache		 mPluginCache;
};

#endif // PLUGINSFORM_H
