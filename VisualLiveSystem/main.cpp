#include <QApplication>
#include <QFile>
#include <QTextStream>
#include "mainwindow.h"
#include "core.h"

#include <iostream>
#include <map>

int main(int argc, char *argv[])
{

    std::map<int, std::string> apiMap;
    apiMap[RtAudio::MACOSX_CORE] = "OS-X Core Audio";
    apiMap[RtAudio::WINDOWS_ASIO] = "Windows ASIO";
    apiMap[RtAudio::WINDOWS_DS] = "Windows Direct Sound";
    apiMap[RtAudio::WINDOWS_WASAPI] = "Windows WASAPI";
    apiMap[RtAudio::UNIX_JACK] = "Jack Client";
    apiMap[RtAudio::LINUX_ALSA] = "Linux ALSA";
    apiMap[RtAudio::LINUX_PULSE] = "Linux PulseAudio";
    apiMap[RtAudio::LINUX_OSS] = "Linux OSS";
    apiMap[RtAudio::RTAUDIO_DUMMY] = "RtAudio Dummy";
    std::vector< RtAudio::Api > apis;
    RtAudio :: getCompiledApi( apis );

    std::cout << "\nRtAudio Version " << RtAudio::getVersion() << std::endl;

    std::cout << "\nCompiled APIs:\n";
    for ( unsigned int i=0; i<apis.size(); i++ )
      std::cout << "  " << apiMap[ apis[i] ] << std::endl;


    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("Razor 1911");
    QCoreApplication::setOrganizationDomain("http://razor1911.com/");
    QCoreApplication::setApplicationName("Visual Live System");

    QFile f(":qdarkstyle/style.qss");
    f.open(QFile::ReadOnly | QFile::Text);
    QTextStream ts(&f);
    a.setStyleSheet(ts.readAll());

    MainWindow w;
    w.showMaximized();
    
    return a.exec();
}
