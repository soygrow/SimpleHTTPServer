/*
 * Copyright 2017 All Rights Reserved.
 * Author: soygrow
 *
 * main_http.cpp
 * ---------------------------------------------------------------------------
 * The main function of Http server class.
 * ---------------------------------------------------------------------------
 */

#include "server_http.hpp"

namespace HttpWeb {

  int main() {
    Server<HTTP> server(12345, 4);
    StartServer<Server<HTTP>> (server);

    return 0
  }
  
} // namespace HttpWeb
