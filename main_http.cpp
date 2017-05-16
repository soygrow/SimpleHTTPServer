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
  
int main(int argc, char** argv) {
  HttpWeb::Server<HttpWeb::HTTP> server(12345, 4);
  HttpWeb::StartServer<HttpWeb::Server<HttpWeb::HTTP>> (server);

  return 0;
}
  
