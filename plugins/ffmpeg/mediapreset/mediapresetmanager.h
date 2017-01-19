#ifndef MEDIAPRESETMANAGER_H
#define MEDIAPRESETMANAGER_H

#include <QObject>
#include <QStringList>
#include <QMap>

#include "interfacemediapreset.h"

class MediaPresetManager : public QObject
{
	Q_OBJECT

public:
	explicit MediaPresetManager( QObject *pParent = 0 );

	const QStringList &presetNames( void ) const
	{
		return( mPresetNames );
	}

	typedef MediaPresetInterface *(*PresetFunction)( void );

	void registerPreset( const QString &pPresetName, PresetFunction pPresetFunction );

	MediaPresetInterface *preset( const QString &pPresetName ) const;

	static MediaPresetManager *instance( void );

private:
	QStringList							 mPresetNames;
	QMap<QString,PresetFunction>		 mPresetFunctions;
};

#endif // MEDIAPRESETMANAGER_H
