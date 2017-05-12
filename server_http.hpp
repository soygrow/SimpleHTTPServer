/*
 * Copyright 2017 All Rights Reserved.
 * Author: soygrow
 *
 * server_http.hpp
 * ---------------------------------------------------------------------------
 * The implemetation of Http server class.
 * ---------------------------------------------------------------------------
 */

#pragam once

#ifndef SERVER_HTTP_HPP_
#define SERVER_HTTP_HPP_

#include "server_base.hpp"

namespace HttpWeb {

  typedef boost::asio::ip::tcp::socket HTTP;

  template<>
  class Server<HTTP> : public ServerBase<HTTP> {
   public:
    Server(unsigned short port, size_t num_threads) {};

   private:
    void accept();
  }
  
} // namespace HttpWeb

#endif
