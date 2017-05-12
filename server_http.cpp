/*
 * Copyright 2017 All Rights Reserved.
 * Author: soygrow
 *
 * server_http.cpp
 * ---------------------------------------------------------------------------
 * The implemetation of Http server class.
 * ---------------------------------------------------------------------------
 */

#include "server_http.hpp"

namespace HttpWeb {

  void Server::Accept() {
    auto socket = std::make_shared<HTTP>(m_io_service_);

    acceptor_.async_accept(*socket, [this, socket] (const boost::system::error_code& ec) {
      Accept();
      if (!ec) {
        ProcessRequest(socket);
      }
    });
  }
  
} // namespace HttpWeb
