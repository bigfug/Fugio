#ifndef COLOURBUTTON_H
#define COLOURBUTTON_H

#include <QPushButton>
#include <QColor>

class ColourButton : public QPushButton
{
	Q_OBJECT

public:
	explicit ColourButton( QWidget *parent = 0 );
	
	const QColor &colour( void ) const
	{
		return( mColour );
	}

signals:
	void colourChanged( const QColor &pColour ) const;

public slots:
	void setColour( const QColor &pColour );

private slots:
	void buttonClicked( void );

private:
	QColor			mColour;
};

#endif // COLOURBUTTON_H
