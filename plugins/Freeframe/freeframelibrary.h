#ifndef FREEFRAMELIBRARY_H
#define FREEFRAMELIBRARY_H

#include <QObject>
#include <QLibrary>
#include <QVariant>

#if !defined( QT_OPENGL_ES )
#include "FFGL.h"
#else
#include "FreeFrame.h"
#endif

class FreeframeLibrary
{
public:
	FreeframeLibrary( const QString &pLibraryName );

	virtual ~FreeframeLibrary();

	inline bool isInitialised( void ) const
	{
		return( mFlags.testFlag( INITIALISED ) );
	}

	bool initialise( void );

	void deinitialise( void );

	enum Flag
	{
		NONE				= 0,
		INITIALISED			= 1 << 0,
		PROCESSFRAMECOPY	= 1 << 1,
		FFGL				= 1 << 2,
		SOURCE				= 1 << 3,
		CAP_16BIT			= 1 << 4,
		CAP_24BIT			= 1 << 5,
		CAP_32BIT			= 1 << 6,
		CAP_SETTIME			= 1 << 7
	};

	Q_DECLARE_FLAGS( Flags, Flag )

	inline Flags flags( void ) const
	{
		return( mFlags );
	}

	inline bool isSource( void ) const
	{
		return( mFlags.testFlag( SOURCE ) );
	}

	inline bool isFFGL( void ) const
	{
		return( mFlags.testFlag( FFGL ) );
	}

	inline bool hasProcessFrameCopy( void ) const
	{
		return( mFlags.testFlag( PROCESSFRAMECOPY ) );
	}

	inline int minInputFrames( void ) const
	{
		return( mMinInputFrames );
	}

	inline int maxInputFrames( void ) const
	{
		return( mMaxInputFrames );
	}

	inline FF_Main_FuncPtr func( void )
	{
		return( mMainFunc );
	}

	bool testCapability( FFUInt32 pType ) const;

	static bool testCapability( FF_Main_FuncPtr pFuncPtr, FFUInt32 pType );

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
	int										 mMinInputFrames;
	int										 mMaxInputFrames;
	Flags									 mFlags;
	QList<ParamEntry>						 mParams;
	FF_Main_FuncPtr							 mMainFunc;
};

Q_DECLARE_OPERATORS_FOR_FLAGS( FreeframeLibrary::Flags )

#endif // FREEFRAMELIBRARY_H
