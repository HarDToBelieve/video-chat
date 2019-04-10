#ifndef CUSTOMSOCKETSERVER_H
#define CUSTOMSOCKETSERVER_H

#include "rtc_base/physical_socket_server.h"

class CustomSocketServer : public rtc::PhysicalSocketServer {
 public:
  explicit CustomSocketServer() {}
  virtual ~CustomSocketServer() {}
};

#endif // CUSTOMSOCKETSERVER_H
