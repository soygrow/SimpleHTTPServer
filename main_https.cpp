/*
 * Copyright 2017 All Rights Reserved.
 * Author: soygrow
 *
 * main_https.cpp
 * ---------------------------------------------------------------------------
 * The main function of Https server class.
 * ---------------------------------------------------------------------------
 */

#include "handler.hpp"
#include "server_base.hpp"
#include "server_https.hpp"

#include <fstream>
#include <iostream>
  
int main(int argc, char** argv) {
  HttpWeb::Server<HttpWeb::HTTPS> server(12340, 4, "server.crt", "server.key");
  HttpWeb::StartServer<HttpWeb::Server<HttpWeb::HTTPS>> (server);

  return 0;
}
  
