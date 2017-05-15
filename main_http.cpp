/*
 * Copyright 2017 All Rights Reserved.
 * Author: soygrow
 *
 * main_http.cpp
 * ---------------------------------------------------------------------------
 * The main function of Http server class.
 * ---------------------------------------------------------------------------
 */

#include "handler.hpp"
#include "server_base.hpp"
#include "server_http.hpp"

#include <fstream>

namespace HttpWeb {
  
  int main(int argc, char** argv) {
    Server<HTTP> server(12345, 4);
    StartServer<Server<HTTP>> (server);

    return 0;
  }
  
} // namespace HttpWeb
