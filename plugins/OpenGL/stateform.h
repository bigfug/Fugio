#ifndef STATEFORM_H
#define STATEFORM_H

#include <QDialog>
#include <QMap>

#include <fugio/global.h>

namespace Ui {
	class OpenGLStateForm;
}

FUGIO_NAMESPACE_BEGIN
class OpenGLStateInterface;
FUGIO_NAMESPACE_END

class QCheckBox;

class StateForm : public QDialog
{
	Q_OBJECT

public:
	explicit StateForm( fugio::OpenGLStateInterface *pState, QWidget *parent = 0 );

	virtual ~StateForm();

	void updateState( void );

private:
	Ui::OpenGLStateForm			*ui;

	fugio::OpenGLStateInterface		*mState;

	QMap<QCheckBox *,int>		 mFlagMap;
	QMap<QString,int>			 mBlendingFactors;
	QMap<QString,int>			 mBlendingEquations;
	QMap<QString,int>			 mDepthFunc;
	QMap<QString,int>			 mPolygonModes;
};

#endif // STATEFORM_H
