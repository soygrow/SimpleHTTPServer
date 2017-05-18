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
#include <iostream>

#include <boost/asio.hpp>

namespace HttpWeb {
  struct Request {
    std::string method; // the request method, POST or GET
    std::string path; // the path of the request 
    std::string http_version; // the version of http
    std::shared_ptr<std::istream> content; 
    std::unordered_map<std::string, std::string> header; // the hash map
    std::smatch path_match; // deal the path using regex
  };

  // the defination of the resource type, the std::function is a lamda function
  // that is use deal with the request.
  typedef std::map<std::string, std::unordered_map<std::string, std::function<void(std::ostream&, Request&)> > > ResourceType;

  // socket_type is HTTP or HTTPS
  template <typename socket_type>
  class ServerBase {
   public:
    ResourceType resource_;
    // When the request source is not in the resource_, we can response the
    // default resource.
    ResourceType default_resource_;

    // end_point_ indicate that one of the two ends in tcp connection.
    // acceptor_ is used to construt the connections.
    // num_threads_ indicate that multi threads is used to deal with the request.
    ServerBase(unsigned short port, size_t num_threads=1) :
      end_point_(boost::asio::ip::tcp::v4(), port),
      acceptor_(m_io_service_, end_point_),
      num_threads_(num_threads) {
    
    }

    // Start the server.
    void Start() {
      for (auto it = resource_.begin(); it != resource_.end(); it++) {
        all_resources_.push_back(it);
      }

      for (auto it = default_resource_.begin(); it != default_resource_.end(); it ++) {
        all_resources_.push_back(it);
      }

      // Begin to socket connect
      Accept();

      // If num_threads_ is > 1, m_io_service_.run() can be the thread
      // pool which is used deal with multi requests.
      // emplace_back can avoid the extra copy or move operation required
      // when using push_back
      for (size_t c = 1; c < num_threads_; c ++) {
        threads_.emplace_back([this] () {
            m_io_service_.run();
          });
      }

      // the main thread
      m_io_service_.run();

      // wait thread end
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

    // For Http and Https request, only accept() is different. So we can
    // realize this function in specific class. In the class of HTTP and
    // HTTPS, this function is different.
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

    // Deal the request and give response.
    void ProcessRequest(std::shared_ptr<socket_type> socket) const {
      // For async_read_until create the read buffer
      auto read_buffer = std::make_shared<boost::asio::streambuf> ();

      // Start an asynchronous operation to read data into a streambuf until
      // a function object indicates a match.
      // This function is used to asynchronously read data into the specified
      // streambuf until a user-defined match condition function object, when
      // applied to the data contained in the streambuf, indicates a successful
      // match. The function call always returns immediately. The asynchronous
      // operation will continue until one of the following condition is true:
      // 1.The match condition function object returns a std::pair where the
      //   second element evaluates to true.
      // 2.An error occurred.
      boost::asio::async_read_until(*socket, *read_buffer, "\r\n\r\n",
                                    [this, socket, read_buffer] (const boost::system::error_code& ec, size_t bytes_transferred) {
        if (!ec) {
          // read_buffer->size() may be different with the bytes_transferred
          // In the document of async_read_until point that beside the delimiter
          // character streambuf may have other extra data after async_read_until
          // operation is end.
          // So the best operation is that parse the data from read_buffer.
          size_t total = read_buffer->size();
          std::istream stream(read_buffer.get());

          auto request = std::make_shared<Request> ();
          // Parse the stream into request.
          *request = ParseRequest(stream);

          //std::cout << "method : " << request->method << std::endl;
          //std::cout << "path : " << request->path << std::endl;
          //std::cout << "version : " << request->http_version << std::endl;

          size_t num_additional_bytes = total - bytes_transferred;

          // If Content-Length is not 0 in the request header, we should read
          // read the data. I test the three requests, the Content-Length is 0.
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
            // Respond the request.
            Respond(socket, request);
          }
        }
      });
    }

    // Respond the request
    void Respond(std::shared_ptr<socket_type> socket, std::shared_ptr<Request> request) const {
      // In all_resources_, the key is follow value:
      // 1. ^/info/?$    2. ^/match/([0-9a-zA-Z]+)/?$
      // 3. ^/string/?$  4. ^/?(.*)$
      // The last is default resource.
      for (auto res_it : all_resources_) {
        std::regex e(res_it->first);
        std::smatch sm_res;
        if (std::regex_match(request->path, sm_res, e)) {
          if (res_it->second.count(request->method) > 0) {
            request->path_match = move(sm_res);

            auto write_buffer = std::make_shared<boost::asio::streambuf> ();
            std::ostream response(write_buffer.get());

            // Excute the std::function in resource.
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
