#include <QtGui>
#include <iostream>
#include "texteditor.h"
#include "highlighter.h"

TextEditor::TextEditor(QWidget *parent) : QPlainTextEdit(parent)
{
    lineNumberArea = new LineNumberArea(this);

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();

    QFont font("consolas",9);
    const int tabStop = 4;  // 4 characters
    QFontMetrics metrics(font);
    setTabStopWidth(tabStop * metrics.width(' '));
    setFont(font);
    setLineWrapMode(QPlainTextEdit::NoWrap);

    m_highlighter = new Highlighter(document());

    m_autoUpdate = false;
    m_timer = new QTimer();
    m_timer->setSingleShot(true);
    m_timer->setInterval(1000);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(updateShader()));
    connect(this, SIGNAL(textChanged()), this, SLOT(resetTimer()));
}

void TextEditor::open(QString filename, int id)
{
    if(id < 0)
        m_highlighter->defineXMLRule();

    m_id = id;
    m_filename = filename;

    QFile f(m_filename);
    f.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream flux(&f);
    setPlainText( flux.readAll() );
    f.close();


    m_autoUpdate = false;
}

void TextEditor::save()
{
    QFile f(m_filename);
    f.setPermissions(QFile::WriteUser);
    f.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream flux(&f);
    flux.setCodec("UTF-8");
    flux << toPlainText();
    f.close();
}

int TextEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 5 + fontMetrics().width(QLatin1Char('9')) * digits;

    return space;
}



void TextEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}



void TextEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}



void TextEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}



void TextEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(Qt::darkGray).darker(300);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}



void TextEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
   // painter.fillRect(event->rect(), Qt::black);


    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::darkGray);
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}

void TextEditor::resetTimer()
{
    m_timer->start();
    m_autoUpdate = true;
}

void TextEditor::updateShader()
{
    if( m_autoUpdate )
    {
        emit shaderUpdate( this->toPlainText(), m_id);
        m_autoUpdate = false;
    }
}

