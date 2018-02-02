#ifndef FREEFRAMELIBRARY_H
#define FREEFRAMELIBRARY_H

#include <QObject>
#include <QLibrary>
#include <QVariant>

#include <FFGL.h>

class FreeframeLibrary
{
public:
	FreeframeLibrary( const QString &pLibraryName );

	virtual ~FreeframeLibrary();

	inline bool isInitialised( void ) const
	{
		return( mFlags.testFlag( FLAGS_INITIALISED ) );
	}

	bool initialise( void );

	void deinitialise( void );

	inline bool isSource( void ) const
	{
		return( mFlags.testFlag( FLAGS_SOURCE ) );
	}

	inline bool isFFGL( void ) const
	{
		return( mFlags.testFlag( FLAGS_FFGL ) );
	}

	inline bool hasProcessFrameCopy( void ) const
	{
		return( mFlags.testFlag( FLAGS_PROCESSFRAMECOPY ) );
	}

	inline int maxInputFrames( void ) const
	{
		return( mMaxInputFrames );
	}

	inline FF_Main_FuncPtr func( void )
	{
		return( mMainFunc );
	}

	enum Flag
	{
		FLAGS_NONE				= 0,
		FLAGS_INITIALISED		= 1 << 0,
		FLAGS_PROCESSFRAMECOPY	= 1 << 1,
		FLAGS_FFGL				= 1 << 2,
		FLAGS_SOURCE			= 1 << 3
	};

	Q_DECLARE_FLAGS( Flags, Flag )

	typedef struct ParamEntry
	{
		QString		mName;
		FFUInt32	mType;
		QVariant	mDefault;

		ParamEntry( void )
			: mType( FF_TYPE_STANDARD )
		{

		}
	} ParamEntry;

	const QList<ParamEntry> &params( void ) const
	{
		return( mParams );
	}

protected:
	QLibrary								 mPluginLibrary;
	int										 mMaxInputFrames;
	Flags									 mFlags;
	QList<ParamEntry>						 mParams;
	FF_Main_FuncPtr							 mMainFunc;
};

Q_DECLARE_OPERATORS_FOR_FLAGS( FreeframeLibrary::Flags )

#endif // FREEFRAMELIBRARY_H
