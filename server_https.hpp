/*
 * Copyright 2017 All Rights Reserved.
 * Author: soygrow
 *
 * server_https.hpp
 * ---------------------------------------------------------------------------
 * The implemetation of Https server class.
 * ---------------------------------------------------------------------------
 */

#pragma once

#ifndef SERVER_HTTP_HPP_
#define SERVER_HTTP_HPP_

#include "server_base.hpp"

#include <boost/asio/ssl.hpp>

namespace HttpWeb {

  typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> HTTPS;

  template<>
  class Server<HTTPS> : public ServerBase<HTTPS> {
  public:
    Server(unsigned short port, size_t num_threads,
           const std::string& cert_file, const std::string& private_key_file) :
      ServerBase(port, num_threads),
      context_(boost::asio::ssl::context::sslv23) {
      context_.use_certificate_chain_file(cert_file);
      context_.use_private_key_file(private_key_file, boost::asio::ssl::context::pem);
    }

  private:
    boost::asio::ssl::context context_;

    void Accept() {
      auto socket = std::make_shared<HTTPS>(m_io_service_, context_);

      acceptor_.async_accept((*socket).lowest_layer(),
                             [this, socket](const boost::system::error_code&ec) {
        Accept();
        if (!ec) {
          (*socket).async_handshake(boost::asio::ssl::stream_base::server,
                                    [this, socket](const boost::system::error_code&ec) {
            if (!ec) {
              ProcessRequest(socket);
            }
          });
        }
      });
    }
  };
  
} // HttpWeb

#endif
