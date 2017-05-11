/*
 * Copyright 2017 All Rights Reserved.
 * Author: soygrow
 *
 * server_base.hpp
 * ---------------------------------------------------------------------------
 * The base class of Http server.
 * ---------------------------------------------------------------------------
 */

#pragma once

#ifndef SERVER_BASE_HPP_
#define SERVER_BASE_HPP_

#include <map>
#include <string>

namespace HttpWeb {
  struct Request {
    std::string method;
    std::string path;
    std::string http_version;
    std::shared_ptr<std::istream> content;
    std::unordered_map<std::string, std::string> header;
    std::smatch path_match;
  };
  
  typedef std::map<std::string, std::unordered_map<std::string,
                                                   std::function<void(std::ostream, Request&)>>> ResourceType;

  template <typename socket_type>
  class ServerBase {
   public:
    ResourceType resource_;
    ResourceType default_resource_;

    ServerBase(unsigned short port, size_t num_threads=1);
    void Start();

   protected:
    boost::asio::io_service m_io_service_;
    boost::asio::ip::tcp::endpoint end_point_;
    boost::asio::ip::tcp::acceptor acceptor_;

    size_t num_threads_;
    std::vector<std::thread> threads_;

    std::vector<ResourceType::iterator> all_resources_;

    virtual void Accept();
    void ProcessRequest(std::shared_ptr<socket_type> socket) const; 
  };
  
} // namespace HttpWeb

#endif
