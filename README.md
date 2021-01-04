# porter

Asynchronous HTTP client for C++(17) using libcurl.

### Usage

Basic synchronous request (curl_easy):

```c++
porter::request req{ "http://httpbin.org/status/200" };
req.perform();
assert(req.status_code() == 200);
```

Asynchronous client (curl_multi):

```c++
void request_done(porter::request& req) {
    std::cout << req.status_code() << std::endl;
}

porter::client cl;
cl.add_request(porter::request{ "http://httpbin.org/status/200", request_done });
cl.add_request(porter::request{ "http://httpbin.org/status/400", request_done });
cl.add_request(porter::request{ "http://httpbin.org/status/500", request_done });

while (cl.num_requests() > 0) {
    // wait, do something, idk
}
```

### Building

Requirements:

- C++17 compiler
- CMake

```
$ mkdir -p build && cd build
$ cmake ..
$ make
```

### Credits

- libcurl authors.
- Guilherme Nemeth.

### License

porter is licensed under the MIT License, see LICENSE file for more information.