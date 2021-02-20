#include "USB_Packet_Analyzer.h"
#include "stdafx.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    USB_Packet_Analyzer w;
    w.show();
    return a.exec();
}
