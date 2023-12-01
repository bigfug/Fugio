#include <QtWidgets>

#include "codeeditor.h"
#include "linenumberarea.h"
#include <fugio/text/syntax_highlighter_instance_interface.h>

CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent)
{
	lineNumberArea = new LineNumberArea(this);

	connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
	connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
	connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

	updateLineNumberAreaWidth(0);
	highlightCurrentLine();
}

void CodeEditor::setSyntaxErrors(QList<fugio::SyntaxError> pSyntaxErrors)
{
	mSyntaxErrors = pSyntaxErrors;
}

int CodeEditor::lineNumberAreaWidth()
{
	int digits = 1;
	int max = qMax(1, blockCount());
	while (max >= 10) {
		max /= 10;
		++digits;
	}

	int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;

	return space;
}

void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
	setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
	if (dy)
		lineNumberArea->scroll(0, dy);
	else
		lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

	if (rect.contains(viewport()->rect()))
		updateLineNumberAreaWidth(0);
}

void CodeEditor::resizeEvent(QResizeEvent *e)
{
	QPlainTextEdit::resizeEvent(e);

	QRect cr = contentsRect();
	lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::highlightCurrentLine()
{
	//	QList<QTextEdit::ExtraSelection> extraSelections;

	//	if( !isReadOnly() )
	//	{
	//		QTextEdit::ExtraSelection selection;

	//		//QColor lineColor = QColor( Qt::yellow ).lighter(160);

	//		selection.format.setBackground( palette().highlight() );
	//		selection.format.setForeground( palette().highlightedText() );
	//		selection.format.setProperty( QTextFormat::FullWidthSelection, true );
	//		selection.cursor = textCursor();
	//		selection.cursor.clearSelection();
	//		extraSelections.append(selection);
	//	}

	//	setExtraSelections(extraSelections);
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
	QPainter painter( lineNumberArea );
	//painter.fillRect(event->rect(), Qt::lightGray);
	painter.fillRect( event->rect(), lineNumberArea->palette().base() );

	QTextBlock block = firstVisibleBlock();
	int blockNumber = block.blockNumber();
	int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
	int bottom = top + (int) blockBoundingRect(block).height();

	while (block.isValid() && top <= event->rect().bottom())
	{
		if (block.isVisible() && bottom >= event->rect().top())
		{
			QString number = QString::number( blockNumber + 1 );

			painter.setPen( lineNumberArea->palette().text().color() );

			for( const fugio::SyntaxError &SE : mSyntaxErrors )
			{
				if( SE.mLineStart <= blockNumber + 1 && SE.mLineEnd >= blockNumber + 1 )
				{
					QRectF	R( event->rect().left(), top, event->rect().width(), blockBoundingRect( block ).height() );

					painter.fillRect( R, Qt::red );

					painter.setPen( Qt::white );

					break;
				}
			}

			painter.drawText( 0, top, lineNumberArea->width(), fontMetrics().height(),
							  Qt::AlignRight, number );
		}

		block = block.next();
		top = bottom;
		bottom = top + (int) blockBoundingRect(block).height();
		++blockNumber;
	}
}


void CodeEditor::keyPressEvent(QKeyEvent *event)
{
	if( event->key() == Qt::Key_Tab )
	{
		QTextCursor		TxtCur = textCursor();

		if( TxtCur.hasSelection() )
		{
			int		spos = TxtCur.anchor();
			int		epos   = TxtCur.position();

			if( spos > epos )
			{
				std::swap( spos, epos );
			}

			TxtCur.setPosition( spos, QTextCursor::MoveAnchor);
			int sblock = TxtCur.block().blockNumber();

			TxtCur.setPosition(epos, QTextCursor::MoveAnchor);
			int eblock = TxtCur.block().blockNumber();

			TxtCur.setPosition(spos, QTextCursor::MoveAnchor);

			TxtCur.beginEditBlock();

			const int blockDifference = eblock - sblock;

			for( int i = 0; i < blockDifference; ++i)
			{
				TxtCur.movePosition( QTextCursor::StartOfBlock, QTextCursor::MoveAnchor );

				TxtCur.insertText( "\t" );

				TxtCur.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor);
			}

			TxtCur.endEditBlock();

			// Set our cursor's selection to span all of the involved lines.

			TxtCur.setPosition(spos, QTextCursor::MoveAnchor);
			TxtCur.movePosition(QTextCursor::StartOfBlock, QTextCursor::MoveAnchor);

			while(TxtCur.block().blockNumber() < eblock)
			{
				TxtCur.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor);
			}

			setTextCursor(TxtCur);

			return;
		}
	}

	if( event->key() == Qt::Key_Backtab )
	{
		QTextCursor		TxtCur = textCursor();

		if( TxtCur.hasSelection() )
		{
			int		spos = TxtCur.anchor();
			int		epos = TxtCur.position();

			if( spos > epos )
			{
				std::swap( spos, epos );
			}

			TxtCur.setPosition( spos, QTextCursor::MoveAnchor);
			int sblock = TxtCur.block().blockNumber();

			TxtCur.setPosition(epos, QTextCursor::MoveAnchor);
			int eblock = TxtCur.block().blockNumber();

			TxtCur.setPosition(spos, QTextCursor::MoveAnchor);

			TxtCur.beginEditBlock();

			const int blockDifference = eblock - sblock;

			for( int i = 0; i < blockDifference; ++i)
			{
				TxtCur.movePosition( QTextCursor::StartOfBlock, QTextCursor::MoveAnchor );

				TxtCur.movePosition( QTextCursor::NextCharacter, QTextCursor::KeepAnchor );

				if( TxtCur.selectedText() == "\t" || TxtCur.selectedText() == " " )
				{
					TxtCur.removeSelectedText();
				}

				TxtCur.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor);
			}

			TxtCur.endEditBlock();

			// Set our cursor's selection to span all of the involved lines.

			TxtCur.setPosition(spos, QTextCursor::MoveAnchor);
			TxtCur.movePosition(QTextCursor::StartOfBlock, QTextCursor::MoveAnchor);

			while(TxtCur.block().blockNumber() < eblock)
			{
				TxtCur.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor);
			}

			setTextCursor(TxtCur);

			return;
		}
	}

	QPlainTextEdit::keyPressEvent( event );
}
