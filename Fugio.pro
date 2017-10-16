TEMPLATE = subdirs

SUBDIRS += \
    FugioLib \
    FugioApp \
	FugioShow \
    plugins

FugioApp.depends = FugioLib plugins
FugioShow.depends = FugioLib plugins
