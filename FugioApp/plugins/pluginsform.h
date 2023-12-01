#ifndef PLUGINSFORM_H
#define PLUGINSFORM_H

#include <QWidget>

namespace Ui {
class PluginsForm;
}

class PluginsForm : public QWidget
{
    Q_OBJECT

public:
    explicit PluginsForm(QWidget *parent = nullptr);
    ~PluginsForm();

private:
    Ui::PluginsForm *ui;
};

#endif // PLUGINSFORM_H
