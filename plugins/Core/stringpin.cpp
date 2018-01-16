#include "stringpin.h"
#include <QSettings>

#include <fugio/core/uuid.h>

StringPin::StringPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin ), VariantHelper( QMetaType::QString, PID_STRING )
{
}
