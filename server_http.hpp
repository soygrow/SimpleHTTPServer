/*
 * Copyright 2017 All Rights Reserved.
 * Author: soygrow
 *
 * server_http.hpp
 * ---------------------------------------------------------------------------
 * The implemetation of Http server class.
 * ---------------------------------------------------------------------------
 */

#pragma once

#ifndef SERVER_HTTP_HPP_
#define SERVER_HTTP_HPP_

#include "server_base.hpp"

namespace HttpWeb {

  typedef boost::asio::ip::tcp::socket HTTP;

  template<>
  class Server<HTTP> : public ServerBase<HTTP> {
   public:
    Server(unsigned short port, size_t num_threads=1) :
      ServerBase<HTTP>::ServerBase(port, num_threads) {};

   private:
    void Accept() {
      auto socket = std::make_shared<HTTP>(m_io_service_);

      acceptor_.async_accept(*socket, [this, socket] (const boost::system::error_code& ec) {
        Accept();
        if (!ec) {
          ProcessRequest(socket);
        }
      });
    }
    
  }; // class HttpWeb
  
} // namespace HttpWeb

#endif
