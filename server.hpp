/*
 * Copyright 2017 All Rights Reserved.
 * Author: soygrow
 *
 * server.hpp
 * ---------------------------------------------------------------------------
 * The class of Http server.
 * ---------------------------------------------------------------------------
 */

#pragma once

#ifndef SERVER_HPP_
#define SERVER_HPP_

#include "server_base.hpp"

namespace HttpWeb {

  template<typename socket_type>
  class Server : public ServerBase<socket_type> {
  };

} // namespace HttpWeb

#endif
