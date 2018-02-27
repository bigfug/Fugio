#ifndef OPENGL_INCLUDES_H
#define OPENGL_INCLUDES_H

#include <QOpenGLContext>
#include <QOpenGLFunctions>

#if defined( QT_DEBUG )
#define OPENGL_DEBUG_ENABLE
#endif

#if defined( OPENGL_DEBUG_ENABLE )
#define OPENGL_PLUGIN_DEBUG 	OpenGLPlugin::instance()->checkErrors( __FILE__, __LINE__ );
#define OPENGL_DEBUG(x) 	OpenGLPlugin::instance()->checkErrors( x, __FILE__, __LINE__ );
#else
#define OPENGL_PLUGIN_DEBUG
#define OPENGL_DEBUG(x)
#endif

#endif // OPENGL_INCLUDES_H
