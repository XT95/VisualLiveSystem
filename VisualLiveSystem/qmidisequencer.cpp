#include <iostream>
#include <porttime.h>
#include "qmidisequencer.h"

#include <stdio.h>

#define INPUT_BUFFER_SIZE 4096
#define DRIVER_INFO NULL
#define TIME_PROC ((int32_t (*)(void *)) Pt_Time)
#define TIME_INFO NULL
#define TIME_START Pt_Start(1, 0, 0) /* timer started w/millisecond accuracy */

QMidiSequencer::QMidiSequencer(QString clientName, int device, QObject *parent) :
    QThread(parent)
{;
    TIME_START;
    Pm_OpenInput(&m_midi,
                 device,
                 DRIVER_INFO,
                 INPUT_BUFFER_SIZE,
                 TIME_PROC,
                 TIME_INFO);
    //Pm_SetFilter(m_midi, PM_FILT_ACTIVE | PM_FILT_CLOCK | PM_FILT_SYSEX);
}
QMidiSequencer::~QMidiSequencer()
{
    Pm_Close(m_midi);
}

void QMidiSequencer::run()
{
    while(true)
    {
        if (Pm_Poll(m_midi) == TRUE)
        {
            int size = Pm_Read(m_midi,m_buffer, 256);
            for(int i=0; i<size; i++)
            {
                int type = Pm_MessageStatus(m_buffer[i].message);
                int chan = Pm_MessageData1(m_buffer[i].message);
                int value = Pm_MessageData2(m_buffer[i].message);
                //std::cout << "midi : " << QString::number(type).toStdString() << " | " << QString::number(chan).toStdString() << " | " << QString::number(value).toStdString() << std::endl;
                /*qDebug(QString::number(type).toStdString().c_str());
                qDebug(QString::number(chan).toStdString().c_str());
                qDebug(QString::number(value).toStdString().c_str());
                qDebug("\n");*/
                if( chan == 74 || chan == 10 || chan == 7 )
                    emit controllerValueChanged(type*chan,value);
                else if( (chan == 72 || chan == 73) )
                   emit noteOn(type,type+chan,value);
               /*else if( type == 144 && value == 0 )
                    emit noteOff(type, chan);*/
            }
        }
        this->msleep(10);
    }
}
