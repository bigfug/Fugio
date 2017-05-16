#include "isfnode.h"

#include <QJsonDocument>
#include <QJsonArray>

#include <QByteArray>

#include <fugio/core/uuid.h>
#include <fugio/colour/uuid.h>
#include <fugio/opengl/uuid.h>
#include <fugio/audio/uuid.h>

#include <fugio/colour/colour_interface.h>
#include <fugio/choice_interface.h>

#include "isfplugin.h"

ISFNode::ISFNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mVAO( 0 ), mBuffer( 0 ), mProgram( 0 ), mVertexShader( 0 )
{
	FUGID( PIN_INPUT_SOURCE, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_OUTPUT_RENDER, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );

	mPinInputSource = pinInput( "Source", PIN_INPUT_SOURCE );

	mValOutputRender = pinOutput<fugio::RenderInterface *>( "Render", mPinOutputRender, PID_RENDER, PIN_OUTPUT_RENDER );
}

bool ISFNode::deinitialise()
{
	if( ISFPlugin::hasContextStatic() )
	{
		if( mVAO )
		{
			glDeleteVertexArrays( 1, &mVAO );
		}

		if( mBuffer )
		{
			glDeleteBuffers( 1, &mBuffer );
		}
	}

	mVAO = 0;

	mBuffer = 0;

	return( NodeControlBase::deinitialise() );
}

void ISFNode::inputsUpdated( qint64 pTimeStamp )
{
	if( pTimeStamp && mPinInputSource->isUpdated( pTimeStamp ) )
	{
		QByteArray		Source = variant( mPinInputSource ).toByteArray();

		int			CommentStart = Source.indexOf( "/*" );
		int			CommentEnd   = Source.indexOf( "*/", CommentStart );

		if( CommentStart != -1 && CommentEnd != -1 )
		{
			QByteArray		Comment = Source.mid( CommentStart + 2, CommentEnd - CommentStart - 2 );

			QJsonParseError	JERR;

			QJsonDocument	JSON = QJsonDocument::fromJson( Comment, &JERR );

			QList<QSharedPointer<fugio::PinInterface>>	InputPins = mNode->enumInputPins();

			InputPins.removeAll( mPinInputSource );

			if( !JSON.isNull() )
			{
				const QJsonObject	JOBJ = JSON.object();

				if( JOBJ.contains( "ISFVSN" ) && JOBJ.value( "ISFVSN" ).toString().split( '.' ).first() == "2" )
				{
					mShaderSource = Source.mid( CommentEnd + 2 );

					QJsonArray	Inputs = JOBJ.value( "INPUTS" ).toArray();

					for( const QJsonValue v : Inputs )
					{
						const QJsonObject	Input = v.toObject();
						QString				Name  = Input.value( "NAME" ).toString();
						QString				Type  = Input.value( "TYPE" ).toString();

						if( !Name.isEmpty() && !Type.isEmpty() )
						{
							QSharedPointer<fugio::PinInterface>	InputPin = mNode->findInputPinByName( Name );

							if( InputPin )
							{
								InputPins.removeAll( InputPin );

								continue;
							}

							InputPin = pinInput( Name, QUuid::createUuid() );

							if( !InputPin )
							{
								continue;
							}

							if( Type == "event" )
							{
							}
							else if( Type == "bool" )
							{
								InputPin->setValue( Input.value( "DEFAULT" ).toBool() );
							}
							else if( Type == "long" )
							{
								QJsonArray	Labels = Input.value( "LABELS" ).toArray();
								QJsonArray	Values = Input.value( "VALUES" ).toArray();

								QStringList		LabLst;
								QVariantList	ValLst;

								for( QJsonValueRef L : Labels )
								{
									LabLst << L.toString();
								}

								for( QJsonValueRef V : Values )
								{
									ValLst << V.toInt();
								}

								QSharedPointer<fugio::PinControlInterface>	PinInf = mNode->context()->createPinControl( PID_CHOICE, InputPin );

								if( PinInf )
								{
									InputPin->setControl( PinInf );

									fugio::ChoiceInterface	*ChoiceInf = qobject_cast<fugio::ChoiceInterface *>( PinInf->qobject() );

									if( ChoiceInf )
									{
										ChoiceInf->setChoices( LabLst );

										InputPin->setSetting( "VALUES", ValLst );
									}
								}

								int		DefVal = Input.value( "DEFAULT" ).toInt();

								if( DefVal < LabLst.size() )
								{
									InputPin->setValue( LabLst.at( DefVal ) );
								}
							}
							else if( Type == "float" )
							{
								InputPin->setValue( Input.value( "DEFAULT" ).toDouble() );
							}
							else if( Type == "point2D" )
							{
								InputPin->registerPinInputType( PID_POINT );

								InputPin->setValue( QPointF() );
							}
							else if( Type == "image" )
							{
								InputPin->registerPinInputType( PID_OPENGL_TEXTURE );
							}
							else if( Type == "color" )
							{
								InputPin->registerPinInputType( PID_COLOUR );

								QColor		C;

								QJsonArray	DefaultColour = Input.value( "DEFAULT" ).toArray();

								if( !DefaultColour.isEmpty() )
								{
									if( DefaultColour.size() > 0 ) C.setRedF( DefaultColour.at( 0 ).toDouble() );
									if( DefaultColour.size() > 1 ) C.setGreenF( DefaultColour.at( 1 ).toDouble() );
									if( DefaultColour.size() > 2 ) C.setBlueF( DefaultColour.at( 2 ).toDouble() );
									if( DefaultColour.size() > 3 ) C.setAlphaF( DefaultColour.at( 3 ).toDouble() );
								}

								InputPin->setValue( C );
							}
							else if( Type == "audio" )
							{
								InputPin->registerPinInputType( PID_AUDIO );
							}
							else if( Type == "audioFFT" )
							{
								InputPin->registerPinInputType( PID_FFT );
							}
						}
					}

					for( QSharedPointer<fugio::PinInterface> P : InputPins )
					{
						mNode->removePin( P );
					}
				}
			}
			else
			{
				qWarning() << JERR.errorString();
			}
		}
	}
}

void ISFNode::render( qint64 pTimeStamp, QUuid pSourcePinId )
{
	if( !ISFPlugin::hasContextStatic() )
	{
		return;
	}

	if( !mVAO )
	{
		glGenVertexArrays( 1, &mVAO );
	}

	if( mVAO )
	{
		glBindVertexArray( mVAO );
	}

	if( !mBuffer )
	{
		GLfloat		Verticies[][ 2 ] = {
			{ -1, -1 },
			{  1, -1 },
			{  1,  1 },
			{ -1,  1 }
		};

		glGenBuffers( 1, &mBuffer );
		glBindBuffer( GL_ARRAY_BUFFER, mBuffer );
		glBufferData( GL_ARRAY_BUFFER, sizeof( Verticies ), Verticies, GL_STATIC_DRAW );
	}

	if( !mProgram )
	{
		if( !mVertexShader )
		{
			mVertexShader = glCreateShader( GL_VERTEX_SHADER );

			QString		VS = "attribute highp vec2 vertex;\n"
																			"void main(void)\n"
																			"{\n"
																			"   gl_Position = vec4( vertex, 0, 1 );\n"
																			"}";

			glShaderSource( mVertexShader, 1, VS.toLatin1().constData(), VS.size() );)
		}
		GLuint
	}

	if( mProgram )
	{
		glUseProgram( mProgram );
	}

//	if( !mShaderProgram.isLinked() )
//	{
//		mShaderProgram.removeAllShaders();

//		mShaderProgram.addShaderFromSourceCode( QOpenGLShader::Vertex,
//												"attribute highp vec2 vertex;\n"
//												"void main(void)\n"
//												"{\n"
//												"   gl_Position = vec4( vertex, 0, 1 );\n"
//												"}" );

//		mShaderProgram.addShaderFromSourceCode( QOpenGLShader::Fragment,
//												 "void main(void)\n"
//												 "{\n"
//												 "   gl_FragColor = vec4(1);\n"
//												 "}");

//		mShaderProgram.link();
//	}

//	if( !mShaderProgram.isLinked() )
//	{
//		return;
//	}

//	if( !mShaderProgram.bind() )
//	{
//		return;
//	}

//	int vertexLocation = mShaderProgram.attributeLocation( "vertex" );

//	static GLfloat const triangleVertices[] = {
//		-1.0f,  -1.0f,
//		 1.0f,  -1.0f,
//		-1.0f,   1.0f,
//	};

//	mShaderProgram.enableAttributeArray( vertexLocation );

//	mShaderProgram.setAttributeArray( vertexLocation, triangleVertices, 2 );

	glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

//	mShaderProgram.disableAttributeArray( vertexLocation );

//	mShaderProgram.release();

	if( mVAO )
	{
		glBindVertexArray( 0 );
	}

	glUseProgram( 0 );
}

