#include "mainwindow.h"
#include <QApplication>
#include "rtc_base/ssl_adapter.h"
#include "api/peer_connection_interface.h"
#include <iostream>

#include "mywidget.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MyWidget w;


    if (!rtc::InitializeSSL()) {
        std::cerr << "Unable to initialize SSL" << std::endl;
        return -1;
    }
    w.show();

    return app.exec();
}
