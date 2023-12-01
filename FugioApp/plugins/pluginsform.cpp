#include "pluginsform.h"
#include "ui_pluginsform.h"

PluginsForm::PluginsForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PluginsForm)
{
    ui->setupUi(this);
}

PluginsForm::~PluginsForm()
{
    delete ui;
}
