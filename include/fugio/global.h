#ifndef FUGIOGLOBAL_H
#define FUGIOGLOBAL_H

#include <QtCore/qglobal.h>

#if defined(FUGIOLIB_LIBRARY)
#  define FUGIOLIBSHARED_EXPORT Q_DECL_EXPORT
#else
#  define FUGIOLIBSHARED_EXPORT Q_DECL_IMPORT
#endif

#define FUGIO_NAMESPACE_BEGIN	namespace fugio {
#define FUGIO_NAMESPACE_END		}

#endif // FUGIOGLOBAL_H
