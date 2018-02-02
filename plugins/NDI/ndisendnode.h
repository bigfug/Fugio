#ifndef NDISENDNODE_H
#define NDISENDNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

#if defined( NDI_SUPPORTED )

#if defined( Q_OS_WIN )
#include <windows.h>
#else
#include <Processing.NDI.compat.h>
#endif

#include <Processing.NDI.Lib.h>
#include <Processing.NDI.Find.h>
#include <Processing.NDI.Recv.h>
#include <Processing.NDI.Send.h>
#endif

#include <fugio/image/image.h>
#include <fugio/audio/audio_instance_base.h>

class NDISendNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "NDI Sender" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/NDI_Send" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE NDISendNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~NDISendNode( void ) {}

	// NodeControlInterface interface
public:
//	virtual bool initialise() Q_DECL_OVERRIDE;
	virtual bool deinitialise() Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

protected slots:
	void contextFrameStart( qint64 pTimeStamp );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputName;
	QSharedPointer<fugio::PinInterface>			 mPinInputGroups;
	QSharedPointer<fugio::PinInterface>			 mPinInputFPS;
	QSharedPointer<fugio::PinInterface>			 mPinInputAspectRatio;
	QSharedPointer<fugio::PinInterface>			 mPinInputImage;
	QSharedPointer<fugio::PinInterface>			 mPinInputAudio;

	fugio::AudioInstanceBase					*mProducerInstance;

	qint64										 mSamplePosition;

	QVector<float>								 mAudioBuffer;

	QString										 mNDIName;
	QString										 mNDIGroups;

	QVector<quint8>								 mImageBuffer;
#if defined( NDI_SUPPORTED )
	NDIlib_send_instance_t						 mNDIInstance;
#endif
};

#endif // NDISENDNODE_H
