#include <cassert>
#include <curl/curl.h>
#include <porter/porter.hpp>
#include <iostream>
#include <chrono>
#include <atomic>

using namespace porter;

static std::atomic_int a = 0;

void request_done(async_request& req) {
    a += 1;
    std::cout << req.status_code() << std::endl;
}

int main(int argc, char** argv) {
    global_guard _;
    client cl;

    cl.add_request(async_request{ "http://httpbin.org/delay/5", request_done });
    cl.add_request(async_request{ "http://httpbin.org/delay/4", request_done });

    while (cl.num_requests()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(32));
    }

    return 0;
}