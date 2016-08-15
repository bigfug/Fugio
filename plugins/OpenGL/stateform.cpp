#include "stateform.h"
#include "ui_openglstateform.h"

#include <QCheckBox>

#include <fugio/opengl/state_interface.h>

#include "statepin.h"

#define INSERT_FACTOR(x)		mBlendingFactors.insert(#x,x)
#define INSERT_EQUATION(x)		mBlendingEquations.insert(#x,x)
#define INSERT_DEPTHFUNC(x)		mDepthFunc.insert(#x,x)
#define INSERT_POLYGON_MODE(x)	mPolygonModes.insert(#x,x)

StateForm::StateForm( fugio::OpenGLStateInterface *pState, QWidget *parent)  :
	QDialog(parent),
	ui(new Ui::OpenGLStateForm), mState( pState )
{
	ui->setupUi(this);

	int		i = 0;

	for( QMap<QString,int>::const_iterator it = StatePin::mapFlags().begin() ; it != StatePin::mapFlags().end() ; it++, i++ )
	{
		int	col = i % 3;
		int row = i / 3;

		QCheckBox	*CB = new QCheckBox( it.key(), this );

		CB->setChecked( pState->tstFlag( it.value() ) );

		ui->mFlagGrid->addWidget( CB, row, col );

		mFlagMap.insert( CB, it.value() );
	}

	INSERT_FACTOR( GL_ZERO );
	INSERT_FACTOR( GL_ONE );
	INSERT_FACTOR( GL_SRC_COLOR );
	INSERT_FACTOR( GL_ONE_MINUS_SRC_COLOR );
	INSERT_FACTOR( GL_DST_COLOR );
	INSERT_FACTOR( GL_ONE_MINUS_DST_COLOR );
	INSERT_FACTOR( GL_SRC_ALPHA );
	INSERT_FACTOR( GL_ONE_MINUS_SRC_ALPHA );
	INSERT_FACTOR( GL_DST_ALPHA );
	INSERT_FACTOR( GL_ONE_MINUS_DST_ALPHA );
	INSERT_FACTOR( GL_CONSTANT_COLOR );
	INSERT_FACTOR( GL_ONE_MINUS_CONSTANT_COLOR );
	INSERT_FACTOR( GL_CONSTANT_ALPHA );
	INSERT_FACTOR( GL_ONE_MINUS_CONSTANT_ALPHA );
	INSERT_FACTOR( GL_CONSTANT_ALPHA );
	INSERT_FACTOR( GL_ONE_MINUS_CONSTANT_ALPHA );
	INSERT_FACTOR( GL_SRC_ALPHA_SATURATE );
#if defined( GL_SRC1_COLOR )
	INSERT_FACTOR( GL_SRC1_COLOR );
#endif
#if defined( GL_ONE_MINUS_SRC1_COLOR )
	INSERT_FACTOR( GL_ONE_MINUS_SRC1_COLOR );
#endif
#if defined( GL_SRC1_ALPHA )
	INSERT_FACTOR( GL_SRC1_ALPHA );
#endif
#if defined( GL_ONE_MINUS_SRC1_ALPHA )
	INSERT_FACTOR( GL_ONE_MINUS_SRC1_ALPHA );
#endif

	INSERT_EQUATION( GL_FUNC_ADD );
	INSERT_EQUATION( GL_FUNC_SUBTRACT );
	INSERT_EQUATION( GL_FUNC_REVERSE_SUBTRACT );
#if defined( GL_MIN )
	INSERT_EQUATION( GL_MIN );
	INSERT_EQUATION( GL_MAX );
#endif
	INSERT_DEPTHFUNC( GL_NEVER );
	INSERT_DEPTHFUNC( GL_ALWAYS );
	INSERT_DEPTHFUNC( GL_LESS );
	INSERT_DEPTHFUNC( GL_LEQUAL );
	INSERT_DEPTHFUNC( GL_EQUAL );
	INSERT_DEPTHFUNC( GL_GEQUAL );
	INSERT_DEPTHFUNC( GL_GREATER );
	INSERT_DEPTHFUNC( GL_NOTEQUAL );

#if defined( GL_FILL )
	INSERT_POLYGON_MODE( GL_FILL );
	INSERT_POLYGON_MODE( GL_LINE );
	INSERT_POLYGON_MODE( GL_POINT );
#endif

	ui->mBlendSrc->addItems( mBlendingFactors.keys() );
	ui->mBlendDst->addItems( mBlendingFactors.keys() );

	ui->mBlendEquation->addItems( mBlendingEquations.keys() );

	ui->mDepthFunc->addItems( mDepthFunc.keys() );

	ui->mPolygonMode->addItems( mPolygonModes.keys() );

	ui->mBlendSrc->setCurrentText( mBlendingFactors.key( mState->blendRgbSrc() ) );
	ui->mBlendDst->setCurrentText( mBlendingFactors.key( mState->blendRgbDst() ) );

	ui->mBlendEquation->setCurrentText( mBlendingEquations.key( mState->blendRgbEquation() ) );

	ui->mDepthFunc->setCurrentText( mDepthFunc.key( mState->depthFunc() ) );

	ui->mPolygonMode->setCurrentText( mPolygonModes.key( mState->polygonMode() ) );
}

StateForm::~StateForm()
{
	delete ui;
}

void StateForm::updateState()
{
	for( QMap<QCheckBox *,int>::iterator it = mFlagMap.begin() ; it != mFlagMap.end() ; it++ )
	{
		mState->setFlag( it.value(), it.key()->isChecked() );
	}

	mState->setBlendRgbSrc( mBlendingFactors.value( ui->mBlendSrc->currentText() ) );
	mState->setBlendAlphaSrc( mBlendingFactors.value( ui->mBlendSrc->currentText() ) );

	mState->setBlendRgbDst( mBlendingFactors.value( ui->mBlendDst->currentText() ) );
	mState->setBlendAlphaDst( mBlendingFactors.value( ui->mBlendDst->currentText() ) );

	mState->setBlendRgbEquation( mBlendingEquations.value( ui->mBlendEquation->currentText() ) );
	mState->setBlendAlphaEquation( mBlendingEquations.value( ui->mBlendEquation->currentText() ) );

	mState->setDepthFunction( mDepthFunc.value( ui->mDepthFunc->currentText() ) );

	mState->setPolygonMode( mPolygonModes.value( ui->mPolygonMode->currentText() ) );
}
