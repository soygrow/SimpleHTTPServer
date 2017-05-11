# SimpleHTTPServer

服务器开发中 Web 服务是一个基本的代码单元，将服务端的请求和响应部分的逻辑抽象出来形成框架，能够做到最高级别的框架级代码复用。本次项目将综合使用 C++11 及 Boost 中的 Asio 实现 HTTP 和 HTTPS 的服务器框架。

可以参考实验楼相关文档:https://www.shiyanlou.com/courses/568#sign-modal

### 项目涉及的知识点

1. C++基本知识
> * 面向对象
> * 模板
> * 命名空间
> * 常用 IO 库

2. C++11 相关
> * lambda expression
> * std::shared_ptr
> * std::make_shared
> * std::unordered_map
> * std::regex
> * std::smatch
> * std::regex_match
> * std::function
> * std::thread

3. Boost Asio 相关
> * boost::asio::io_service
> * boost::asio::ip::tcp::socket
> * boost::asio::ip::tcp::v4()
> * boost::asio::ip::tcp::endpoint
> * boost::asio::ip::tcp::acceptor
> * boost::asio::streambuf
> * boost::asio::async_read
> * boost::asio::async_read_until
> * boost::asio::async_write
> * boost::asio::transfer_exactly
> * boost::asio::ssl::stream
> * boost::asio::ssl::stream_base::server
> * boost::asio::ssl::context
> * boost::asio::ssl::context::sslv23
> * boost::asio::ssl::context::pem
> * boost::system::error_code
