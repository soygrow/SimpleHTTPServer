# SimpleHTTPServer
标签： HTTP服务器
----------
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

### 程序介绍

- 首先我们看下主函数的功能
```cpp
int main(int argc, char** argv) {
  HttpWeb::Server<HttpWeb::HTTP> server(12345, 4);
  HttpWeb::StartServer<HttpWeb::Server<HttpWeb::HTTP>> (server);

  return 0;
}
```
    1. 主要就两行代码，首先构造了HttpWeb::Server<HttpWeb::HTTP>对象
    2. 调用StartServer，对server_base中的source进行赋值，定义相应的操作

 - 再看下StartServer中实现的功能
```cpp
void StartServer(SERVER_TYPE& server) {
    server.resource_["^/string/?$"]["POST"] = [](std::ostream& response, Request& request) {
        ......
    };

    server.resource_["^/info/?$"]["GET"] = [](std::ostream& response, Request& request) {
        ......
    };

    server.resource_["^/match/([0-9a-zA-Z]+)/?$"]["GET"] = [](std::ostream& response, Request& request) {
        ......
    };

    server.default_resource_["^/?(.*)$"]["GET"] = [](std::ostream& response, Request& request) {
        ......
    };

    server.Start();
  }
```
    1. 可以看到主要定义了一些具体的操作，比如当请求info、match等请求过来后的具体操作，如果resource_中没有定义相应的操作，那么转到default_resource_中进行返回
    2. 这里可以看到定义了四个请求方式：string，info，match，默认
    3. 定义了几个具体的操作后，开始启动服务器，server.Start()

 - 看下Start函数中的操作
```cpp
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
    // when using push_bach
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
```
    1. Start函数中做的操作也简单，将具体的操作resource和default_resource放入到all_resource中，这样便于后面进行遍历查找相应的请求
    2. 然后调用了Accept函数，该函数在具体的实例中进行实现，而不再server_base中实现，具体工作是等待client的连接，并接受client的请求
    3. 紧接着启动了多个线程，并等待线程结束
    
 - 下面看下Accept实现的功能
```cpp
void Accept() {
    auto socket = std::make_shared<HTTP>(m_io_service_);

    acceptor_.async_accept(*socket, [this, socket] (const boost::system::error_code& ec) {
        Accept();
        if (!ec) {
            ProcessRequest(socket);
        }
    });
}
```
    1. 首先获得m_io_service_的指针
    2. 然后调用async_accept接受client的请求，如果没有出现错误，则调用ProcessRequest处理接收到的请求
    3. ProcessRequest主要功能是读取client的请求数据，然后调用Respond进行相应请求，具体可以参看相关代码
    
### 总结
1. 整理下来，代码逻辑还是比较清晰的
2. 首先创建了server对象，并初始化asio相关参数
3. 定义了四个请求方式，有string，info，match以及默认的方式，然后启动服务器，调用Start
4. 调用Accept进行接受client的请求，并启动几个线程
5. 当client的请求过来后，分析client请求的URL，并对相应的请求进行处理

### 注意事项
1. BUILD文件是我使用bazel进行编译时需要的文件，如果你不是使用bazel进行编译，而是使用makefile进行编译，那么可以忽略该文件
2. 如果你的gcc版本是4.9.0之前的版本，那么必须升级你的gcc，不然regex库不支持ECMAScript的正则语法，换句话说，4.9以前的版本对C++11标准库支持的并不好，所以需要升级你的gcc，相应的升级方法可以点击[此处][1]中提到的方法。


  [1]: https://www.shiyanlou.com/courses/568/labs/1984/document
