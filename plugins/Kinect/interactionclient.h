#ifndef INTERACTIONCLIENT_H
#define INTERACTIONCLIENT_H

#include <QObject>
#include <QDebug>

#if 0 //def KINECT_SUPPORTED

#include <NuiApi.h>
#include <KinectInteraction.h>

class InteractionClient : public INuiInteractionClient
{
public:
	InteractionClient( void ) {}
	virtual ~InteractionClient( void ) {}

	STDMETHOD(GetInteractionInfoAtLocation)( THIS_ DWORD pSkeletonTrackingId, NUI_HAND_TYPE pHandType, FLOAT x, FLOAT y, _Out_ NUI_INTERACTION_INFO *pInteractionInfo )
	{
		qDebug() << pSkeletonTrackingId << pHandType << x << y;

		Q_UNUSED( pSkeletonTrackingId )
		Q_UNUSED( pHandType )
		Q_UNUSED( x )
		Q_UNUSED( y )

		if( pInteractionInfo )
		{
			pInteractionInfo->IsPressTarget         = true;
			pInteractionInfo->PressTargetControlId  = 0;
			pInteractionInfo->PressAttractionPointX = 0.f;
			pInteractionInfo->PressAttractionPointY = 0.f;
			pInteractionInfo->IsGripTarget          = false;

			return S_OK;
		}

		return E_POINTER;

		//return S_OK;

	}

	STDMETHODIMP_(ULONG)    AddRef()                                    { return 2;     }
	STDMETHODIMP_(ULONG)    Release()                                   { return 1;     }

	STDMETHODIMP            QueryInterface(REFIID riid, void **ppv)
	{
		Q_UNUSED( riid )
		Q_UNUSED( ppv )

		return S_OK;
	}

};

#endif // KINECT_SUPPORTED

#endif // INTERACTIONCLIENT_H
