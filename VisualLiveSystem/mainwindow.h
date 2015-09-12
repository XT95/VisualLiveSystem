#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class ChannelWidget;
class MasterWidget;
class VisualManager;
class ProjectorWidget;
class SoundManager;
class MidiWindow;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private:
    Ui::MainWindow  *ui;
    ChannelWidget   *m_chan[2];
    VisualManager   *m_visualManager;
    MasterWidget    *m_master;
    ProjectorWidget *m_proj;
    SoundManager    *m_soundManager;
    MidiWindow      *m_midiManager;
};

#endif // MAINWINDOW_H
