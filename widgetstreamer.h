#ifndef WIDGETSTREAMER_H

#define WIDGETSTREAMER_H

#include "mywidget.h"

class WidgetStreamer : public QObject
{
    Q_OBJECT
public:
    WidgetStreamer(QWidget *pRenderWidget, MyWidget *pStreamWidget) {}
    ~WidgetStreamer() {}
private slots:
    void onTimeout();
private:
    QWidget *m_pRenderWidget;
    MyWidget *m_pStreamWidget;
};

#endif // WIDGETSTREAMER_H


