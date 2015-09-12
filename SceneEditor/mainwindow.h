#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class BottomWidget;
class ParamWidget;
class EditorWidget;
class OpenWidget;
class NewWidget;
class NewShader;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void newScene();
    void newShader();
    void open(QString);
    void save();
    void reloadEffect();
private:
    Ui::MainWindow *ui;
    BottomWidget *m_info;
    ParamWidget *m_param;
    EditorWidget *m_editor;
    OpenWidget *m_open;
    NewShader *m_newShader;
};

#endif // MAINWINDOW_H
