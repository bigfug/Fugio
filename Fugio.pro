TEMPLATE = subdirs

SUBDIRS += \
    FugioLib \
    FugioApp \
    plugins \
    FugioTime

FugioApp.depends = FugioLib plugins

