#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <QPlainTextEdit>
#include <QObject>

class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;

class LineNumberArea;
class Highlighter;

class TextEditor : public QPlainTextEdit
{
     Q_OBJECT

 public:
     TextEditor(QWidget *parent = 0);

     void lineNumberAreaPaintEvent(QPaintEvent *event);
     int lineNumberAreaWidth();
     void open( QString filename, int id );
     void save();
signals:
    void shaderUpdate(QString,int);
    void reload();
 protected:
     void resizeEvent(QResizeEvent *event);

 private slots:
     void updateLineNumberAreaWidth(int newBlockCount);
     void highlightCurrentLine();
     void updateLineNumberArea(const QRect &, int);
     void resetTimer();
     void updateShader();

 private:
     QString colorSyntax(QString in);
     QWidget *lineNumberArea;
     QString m_filename;
     Highlighter *m_highlighter;
     int m_id;
     QTimer *m_timer;
     bool m_autoUpdate;
 };


 class LineNumberArea : public QWidget
 {
 public:
     LineNumberArea(TextEditor *editor) : QWidget(editor) {
         codeEditor = editor;
     }

     QSize sizeHint() const {
         return QSize(codeEditor->lineNumberAreaWidth(), 0);
     }

 protected:
     void paintEvent(QPaintEvent *event) {
         codeEditor->lineNumberAreaPaintEvent(event);
     }

 private:
     TextEditor *codeEditor;
 };


#endif // TEXTEDITOR_H
