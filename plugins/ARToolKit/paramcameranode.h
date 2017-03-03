#ifndef PARAMCAMERANODE_H
#define PARAMCAMERANODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

#include <fugio/artoolkit/param_interface.h>

#include <fugio/math/matrix_interface.h>

#if defined( ARTOOLKIT_SUPPORTED )
#include <AR/ar.h>
#include <AR/gsub_lite.h>
#include <AR/video.h>
#endif

class ParamCameraNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "arParamCamera" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE ParamCameraNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~ParamCameraNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

private:
#if defined( ARTOOLKIT_SUPPORTED )
	static void arglCameraFrustumRHf(const ARParam *cparam, const float focalmin, const float focalmax, float m_projection[16])
	{
		ARdouble    icpara[3][4];
		ARdouble    trans[3][4];
		ARdouble    p[3][3], q[4][4];
		int         width, height;
		int         i, j;

		width  = cparam->xsize;
		height = cparam->ysize;

		if (arParamDecompMat(cparam->mat, icpara, trans) < 0) {
			ARLOGe("arglCameraFrustum(): arParamDecompMat() indicated parameter error.\n");
			return;
		}
		for (i = 0; i < 4; i++) {
			icpara[1][i] = (height - 1)*(icpara[2][i]) - icpara[1][i];
		}

		for(i = 0; i < 3; i++) {
			for(j = 0; j < 3; j++) {
				p[i][j] = icpara[i][j] / icpara[2][2];
			}
		}
		q[0][0] = (2.0 * p[0][0] / (width - 1));
		q[0][1] = (2.0 * p[0][1] / (width - 1));
		q[0][2] = -((2.0 * p[0][2] / (width - 1))  - 1.0);
		q[0][3] = 0.0;

		q[1][0] = 0.0;
		q[1][1] = -(2.0 * p[1][1] / (height - 1));
		q[1][2] = -((2.0 * p[1][2] / (height - 1)) - 1.0);
		q[1][3] = 0.0;

		q[2][0] = 0.0;
		q[2][1] = 0.0;
		q[2][2] = (focalmax + focalmin)/(focalmin - focalmax);
		q[2][3] = 2.0 * focalmax * focalmin / (focalmin - focalmax);

		q[3][0] = 0.0;
		q[3][1] = 0.0;
		q[3][2] = -1.0;
		q[3][3] = 0.0;

		for (i = 0; i < 4; i++) { // Row.
			// First 3 columns of the current row.
			for (j = 0; j < 3; j++) { // Column.
				m_projection[i + j*4] = q[i][0] * trans[0][j] +
										q[i][1] * trans[1][j] +
										q[i][2] * trans[2][j];
			}
			// Fourth column of the current row.
			m_projection[i + 3*4] = q[i][0] * trans[0][3] +
									q[i][1] * trans[1][3] +
									q[i][2] * trans[2][3] +
									q[i][3];
		}
	}
#endif

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputParam;
	QSharedPointer<fugio::PinInterface>			 mPinInputSize;
	QSharedPointer<fugio::PinInterface>			 mPinInputNear;
	QSharedPointer<fugio::PinInterface>			 mPinInputFar;

	QSharedPointer<fugio::PinInterface>			 mPinOutputMatrix;
	fugio::VariantInterface						*mValOutputMatrix;
};

#endif // PARAMCAMERANODE_H
