#ifndef NEWSHADER_H
#define NEWSHADER_H

#include <QWidget>
#include <QtXml>
#include <QtGui>

namespace Ui {
class newShader;
}

class NewShader : public QWidget
{
    Q_OBJECT
    
public:
    explicit NewShader(QWidget *parent = 0);
    ~NewShader();
signals:
    void reload();
public slots:
    void show();
    void ok();
    void cancel();
private slots:
    void setImageChan1();
    void setImageChan2();
    void setImageChan3();
    void setImageChan4();
    void setPassChan1();
    void setPassChan2();
    void setPassChan3();
    void setPassChan4();

private:
    bool writeDomChannel(QLineEdit *name, QLineEdit *value, QDomNode *node, QDomDocument &dom, int id);
    Ui::newShader *ui;
};

#endif // NEWSHADER_H
