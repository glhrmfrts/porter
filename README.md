# porter

Asynchronous HTTP client for C++(17) using libcurl.

### Usage

Basic synchronous request (curl_easy):

```c++
#include <porter/porter.hpp>

porter::global_guard _;
porter::sync_request req{ "http://httpbin.org/status/200" };
req.perform();
assert(req.status_code() == 200);
```

Asynchronous client (curl_multi):

```c++
#include <porter/porter.hpp>

void request_done(const porter::async_request& req, int result_code) {
    std::cout << req.status_code() << std::endl;
}

porter::global_guard _;
porter::client cl;
cl.add_request(porter::async_request{ "http://httpbin.org/status/200", request_done });
cl.add_request(porter::async_request{ "http://httpbin.org/status/400", request_done });
cl.add_request(porter::async_request{ "http://httpbin.org/status/500", request_done });

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