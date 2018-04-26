#include "freeframelibrary.h"

FreeframeLibrary::FreeframeLibrary( const QString &pLibraryName )
	: mPluginLibrary( pLibraryName ), mMaxInputFrames( 1 ), mFlags( NONE ), mMainFunc( nullptr )
{
	mMainFunc = nullptr;
}

FreeframeLibrary::~FreeframeLibrary()
{
}

bool FreeframeLibrary::initialise()
{
	if( isInitialised() )
	{
		return( true );
	}

	if( !mPluginLibrary.load() )
	{
		return( false );
	}

	mMainFunc = (FF_Main_FuncPtr)mPluginLibrary.resolve( "plugMain" );

	if( !mMainFunc )
	{
		return( false );
	}

	//-------------------------------------------------------------------------

	FFMixed		PMU;

#if defined( __FFGL_H__ )
	if( testCapability( FF_CAP_SETTIME ) )
	{
		mFlags |= CAP_SETTIME;
	}

	if( testCapability( FF_CAP_PROCESSOPENGL ) )
	{
		mFlags |= FFGL;
	}
#endif

	if( testCapability( FF_CAP_16BITVIDEO ) )
	{
		mFlags |= CAP_16BIT;
	}

	if( testCapability( FF_CAP_24BITVIDEO ) )
	{
		mFlags |= CAP_24BIT;
	}

	if( testCapability( FF_CAP_32BITVIDEO ) )
	{
		mFlags |= CAP_32BIT;
	}

	if( testCapability( FF_CAP_PROCESSFRAMECOPY ) )
	{
		mFlags |= PROCESSFRAMECOPY;
	}

	PMU.UIntValue = FF_CAP_MINIMUMINPUTFRAMES;

	PMU = mMainFunc( FF_GETPLUGINCAPS, PMU, 0 );

	if( PMU.UIntValue != FF_FALSE )
	{
		mMinInputFrames = PMU.UIntValue;
	}
	else
	{
		mMinInputFrames = 1;
	}

	if( !mMinInputFrames )
	{
		mFlags |= SOURCE;
	}

	PMU.UIntValue = FF_CAP_MAXIMUMINPUTFRAMES;

	PMU = mMainFunc( FF_GETPLUGINCAPS, PMU, 0 );

	if( PMU.UIntValue != FF_FALSE )
	{
		mMaxInputFrames = PMU.UIntValue;
	}
	else
	{
		mMaxInputFrames = 1;
	}

	//-------------------------------------------------------------------------

	PMU.UIntValue = 0;

	PMU = mMainFunc( FF_GETNUMPARAMETERS, PMU, 0 );

	if( PMU.UIntValue != FF_FALSE )
	{
		const int	PrmCnt = PMU.UIntValue;

		for( int i = 0 ; i < PrmCnt ; i++ )
		{
			PMU.UIntValue = i;

			PMU = mMainFunc( FF_GETPARAMETERNAME, PMU, 0 );

			ParamEntry	PrmEnt;

			if( PMU.UIntValue != FF_FAIL )
			{
				PrmEnt.mName = QString::fromLatin1( QByteArray( (const char *)PMU.PointerValue, 16 ) );
			}

			if( PrmEnt.mName.isEmpty() )
			{
				PrmEnt.mName = QString( "Param %1" ).arg( i );
			}

			PMU.UIntValue = i;

			PMU = mMainFunc( FF_GETPARAMETERTYPE, PMU, 0 );

			if( PMU.UIntValue != FF_FAIL )
			{
				PrmEnt.mType = PMU.UIntValue;
			}

			PMU.UIntValue = i;

			PMU = mMainFunc( FF_GETPARAMETERDEFAULT, PMU, 0 );

			switch( PrmEnt.mType )
			{
				case FF_TYPE_BOOLEAN:
					PrmEnt.mDefault = QVariant::fromValue( PMU.UIntValue == FF_TRUE );
					break;

				case FF_TYPE_EVENT:
					PrmEnt.mDefault = QVariant::fromValue( false );
					break;

				case FF_TYPE_RED:
				case FF_TYPE_GREEN:
				case FF_TYPE_BLUE:
				case FF_TYPE_XPOS:
				case FF_TYPE_YPOS:
				case FF_TYPE_STANDARD:
					PrmEnt.mDefault = QVariant::fromValue( PMU.FloatValue );
					break;

				case FF_TYPE_TEXT:
					PrmEnt.mDefault = QString( QByteArray( (const char *)PMU.PointerValue ) );
					break;
			}

			mParams << PrmEnt;
		}
	}

	//-------------------------------------------------------------------------

	PMU.UIntValue = 0;

	PMU = mMainFunc( FF_INITIALISE, PMU, 0 );

	if( PMU.UIntValue != FF_SUCCESS )
	{
		return( false );
	}

	mFlags |= INITIALISED;

	return( true );
}

void FreeframeLibrary::deinitialise()
{
	if( mPluginLibrary.isLoaded() )
	{
		if( isInitialised() )
		{
			FFMixed		PMU;

			PMU.UIntValue = 0;

			PMU = mMainFunc( FF_DEINITIALISE, PMU, 0 );

			mParams.clear();

			mMainFunc = nullptr;

			mFlags &= ~INITIALISED;
		}

		mPluginLibrary.unload();
	}
}

bool FreeframeLibrary::testCapability(FFUInt32 pType) const
{
	FFMixed		PMU;

	PMU.UIntValue = pType;

	PMU = mMainFunc( FF_GETPLUGINCAPS, PMU, 0 );

	return( PMU.UIntValue == FF_SUPPORTED );
}

bool FreeframeLibrary::testCapability( FF_Main_FuncPtr pFuncPtr, FFUInt32 pType )
{
	FFMixed		PMU;

	PMU.UIntValue = pType;

	PMU = pFuncPtr( FF_GETPLUGINCAPS, PMU, 0 );

	return( PMU.UIntValue == FF_SUPPORTED );
}

