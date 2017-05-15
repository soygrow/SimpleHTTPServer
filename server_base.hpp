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
#include <unordered_map>
#include <string>
#include <thread>
#include <regex>

#include <boost/asio.hpp>

namespace HttpWeb {
  struct Request {
    std::string method;
    std::string path;
    std::string http_version;
    std::shared_ptr<std::istream> content;
    std::unordered_map<std::string, std::string> header;
    std::smatch path_match;
  };
  
  typedef std::map<std::string, std::unordered_map<std::string, std::function<void(std::ostream&, Request&)> > > ResourceType;

  template <typename socket_type>
  class ServerBase {
   public:
    ResourceType resource_;
    ResourceType default_resource_;

    ServerBase(unsigned short port, size_t num_threads=1) :
      end_point_(boost::asio::ip::tcp::v4(), port),
      acceptor_(m_io_service_, end_point_),
      num_threads_(num_threads) {
    
    }
    
    void Start() {
      for (auto it = resource_.begin(); it != resource_.end(); it++) {
        all_resources_.push_back(it);
      }

      for (auto it = default_resource_.begin(); it != default_resource_.end(); it ++) {
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

   protected:
    boost::asio::io_service m_io_service_;
    boost::asio::ip::tcp::endpoint end_point_;
    boost::asio::ip::tcp::acceptor acceptor_;

    size_t num_threads_;
    std::vector<std::thread> threads_;

    std::vector<ResourceType::iterator> all_resources_;

    virtual void Accept() {};
    
    Request ParseRequest(std::istream& stream) const {
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
    
    void ProcessRequest(std::shared_ptr<socket_type> socket) const {
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
    
    void Respond(std::shared_ptr<socket_type> socket, std::shared_ptr<Request> request) const {
      for (auto res_it : all_resources_) {
        std::regex e(res_it->first);
        std::smatch sm_res;
        if (std::regex_match(request->path, sm_res, e)) {
          if (res_it->second.count(request->method) > 0) {
            request->path_match = move(sm_res);

            auto write_buffer = std::make_shared<boost::asio::streambuf> ();
            std::ostream response(write_buffer.get());
            res_it->second[request->method](response, *request);

            boost::asio::async_write(*socket, *write_buffer, [this, socket, request, write_buffer] (const boost::system::error_code&ec, size_t bytes_transferred) {
              if (!ec && std::stof(request->http_version) > 1.05) {
                ProcessRequest(socket);
              }
            });
          }
        }
      }
    }

  }; // class ServerBase

  
  template<typename socket_type>
  class Server : public ServerBase<socket_type> {
    
  }; // class Server
  
} // namespace HttpWeb

#endif
