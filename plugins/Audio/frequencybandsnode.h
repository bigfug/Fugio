#ifndef FREQUENCYBANDSNODE_H
#define FREQUENCYBANDSNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>
#include <fugio/core/variant_interface.h>

class FrequencyBandsNode : public fugio::NodeControlBase
{
	Q_OBJECT

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Generates frequency bands from a FFT" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Frequency_Bands_(FFT)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE FrequencyBandsNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~FrequencyBandsNode( void ) {}

	// NodeControlInterface interface
public:
	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

private:
	static float getBandWidth(float timeSize, float sampleRate);
	static int freqToIndex(int timeSize, int sampleRate, int freq);

protected:
	QSharedPointer<fugio::PinInterface>				 mPinInputFFT;
	QSharedPointer<fugio::PinInterface>				 mPinInputCount;

	QSharedPointer<fugio::PinInterface>				 mPinOutputBands;
	fugio::VariantInterface							*mValOutputBands;
};


#endif // FREQUENCYBANDSNODE_H
