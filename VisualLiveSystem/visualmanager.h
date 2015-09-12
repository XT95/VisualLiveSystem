#ifndef VISUALMANAGER_H
#define VISUALMANAGER_H

#include <QWidget>
#include <QListWidget>

namespace Ui {
class VisualManager;
}

class VisualManager : public QWidget
{
    Q_OBJECT
    
public:
    explicit VisualManager(QWidget *parent = 0);
    ~VisualManager();

signals:
    void newListChanA( QList<QListWidgetItem *> list );
    void newListChanB( QList<QListWidgetItem *> list );
    void newListFX( QList<QListWidgetItem *> list );


private slots:
    void delToA();
    void delToB();
    void delPost();
    void apply();
    void resolutionChanged();
    void getSceneList();
    void getPostFXList();

private:
    Ui::VisualManager *ui;
};

#endif // VISUALMANAGER_H
