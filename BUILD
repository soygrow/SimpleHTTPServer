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
          ],
  linkopts = ["-pthread -lboost_thread -lboost_system -lboost_regex"],
  
)

cc_library(
  name = "server_https",
  hdrs = ["server_https.hpp"],
  deps = [
          ":server_base",
         ],
)

cc_binary(
  name = "main_https",
  srcs = ["main_https.cpp"],
  deps = [
          ":server_https",
         ],
  copts = [
           "-I/usr/include/boost",
           "-L/usr/lib",
           "-I/usr/include/openssl",
          ],
  linkopts = ["-pthread -lboost_thread -lboost_system -lboost_regex -lssl -lcrypto"],
)