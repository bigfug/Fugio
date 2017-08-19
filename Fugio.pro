TEMPLATE = subdirs

SUBDIRS += \
    FugioLib \
    FugioApp \
    plugins

FugioApp.depends = FugioLib plugins
