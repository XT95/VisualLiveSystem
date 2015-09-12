#ifndef EDITORWIDGET_H
#define EDITORWIDGET_H

#include <QDockWidget>

namespace Ui {
class EditorWidget;
}

class EditorWidget : public QDockWidget
{
    Q_OBJECT
    
public:
    explicit EditorWidget(QWidget *parent = 0);
    ~EditorWidget();

    void save();
public slots:
    void loadScene( QString filename );

signals:
    void recompil(QString,int);
    void nameCCChanged(int,QString);
private slots:
    void shaderUpdate(QString, int);
private:
    Ui::EditorWidget *ui;
};

#endif // EDITORWIDGET_H
