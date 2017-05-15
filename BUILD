# The BUILD rules for SimpleHttpServer

package(default_visibility = ["//visibility:public"])

cc_library(
  name = "server_base",
  hdrs = ["server_base.hpp"],
)

cc_library(
  name = "server_http",
  hdrs = ["server_http.hpp"],
  deps = [
          ":server_base",
         ],
)

cc_binary(
  name = "main_http",
  srcs = ["main_http.cpp"],
  deps = [
          ":server_http",
         ],
  copts = [
          "-I/usr/include/boost",
          "-L/usr/lib",
          "-std=c++11",
         ],
  linkopts = ["-pthread -lboost_thread -lboost_system"],
  
)