/*
 * Copyright 2017 All Rights Reserved.
 * Author: soygrow
 *
 * server_base.hpp
 * ---------------------------------------------------------------------------
 * The implemetation of Http server base class.
 * ---------------------------------------------------------------------------
 */

#include "server_base.hpp"

#include <regex>

namespace HttpWeb {

  ServerBase::ServerBase(unsigned short port, size_t num_threads=1) :
    endpoint_(boost::asio::tcp::v4(), port),
    acceptor_(m_io_service, endpoint),
    num_threads_(num_threads) {
    
  }

  void ServerBase::Start() {
    for (auto it = resource_.befing(); it != resource_.end(); it++) {
      all_resources_.push_back(it);
    }

    for (auto ite = default_resource_.begin(); it != default_resource_.end(); it ++) {
      all_resources_.push_back(it);
    }

    Accept();

    for (size_t c = 1; c < num_threads_; c ++) {
      threads_.emplace_back([this] () {
        m_io_service_.run();
      });
    }

    m_io_service_.run();

    for (auto& t : threads_) {
      t.join();
    }
  }

  void ServerBase::Accept() {}

  Request ServerBase::ParseRequest(std::istream& stream) {
    Request request;

    std::regex e("^([^ ]*) ([^ ]*) HTTP/([^ ]*)$");
    std::smatch sub_match;

    std::string line;
    getline(stream, line);
    line.pop_back();
    if (std::regex_match(line, sub_match, e)) {
      request.method = sub_match[1];
      request.path = sub_match[2];
      request.http_version = sub_match[3];

      bool matched;
      e = "^([^:]*): ?(.*)$";

      do {
        getline(stream, line);
        line.pop_back();
        matched = std::regex_match(line, sub_match, e);

        if (matched) {
          request.header[sub_match[1]] = sub_match[2];
        }
      } while (matched == true);
    }

    return request;
  }
  
  void ServerBase::ProcessRequest(std::shared_ptr<socket_type> socket) {
    auto read_buffer = std::make_shared<boost::asio::streambuf> ();

    boost::asio::async_read_until(*socket, *read_buffer, "\r\n\r\n",
                                  [this, socket, read_buffer] (const boost::system::error_code& ec, size_t bytes_transferred) {
      if (!ec) {
        size_t total = read_buffer->size();
        std::istream stream(read_buffer.get());

        auto request = std::make_shared<Request> ();
        *request = ParseRequest(stream);

        size_t num_additional_bytes = total - bytes_transferred;

        if (request->header.count("Content-Length") > 0) {
          boost::asio::async_read(*socket, *read_buffer,
                                  boost::asio::transfer_exactly(stoull(request->header["Content-Length"]) - num_additional_bytes),
                                  [this, socket, read_buffer, request] (const boost::system::error_code& ec, size_t bytes_transferred) {
             if (!ec) {
               request->content = std::shared_ptr<std::istream>(new std::istream(read_buffer.get()));
               Respond(socket, request);
             }
           });
        } else {
          Respond(socket, request);
        }
      }
    });
  }

  void ServerBase::Respond(std::shared_ptr<socket_type> socket, std::shared_ptr<Request> request) {
    for (auto res_it : all_resources) {
      std::regex e(res_it->first);
      std::smatch sm_res;
      if (std::regex_match(request->path, sm_res, e)) {
        if (res_it->second.count(request->method) > 0) {
          request->path_match = move(sm_res);

          auto write_buffer = std::make_shared<boost::asio::streambuf> ();
          std::ostream response(write_buffer.get());
          res_it->second{request->method}(response, *request);

          boost::asio::async_write(*socket, *write_buffer, [this, socket, request, write_buffer] (const boost::system::error_code&ec, size_t bytes_transferred) {
              if (!ec && stof(request0->http_version) > 1.05) {
                ProcessRequest(socket);
              }
          });
        }
      }
    }
  }
  
} // namespace HttpWeb
