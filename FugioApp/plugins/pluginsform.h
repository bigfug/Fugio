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
	void on_mButtonSourceAdd_clicked();

private:
    Ui::PluginsForm *ui;
	PluginCache		 mPluginCache;
};

#endif // PLUGINSFORM_H
