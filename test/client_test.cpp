#include <cassert>
#include <curl/curl.h>
#include <porter/client.hpp>
#include <iostream>
#include <chrono>
#include <atomic>

using namespace porter;

static std::atomic_int a = 0;

void request_done(request& req) {
    a += 1;
    std::cout << req.status_code() << std::endl;
}

int main(int argc, char** argv) {
    client cl;

    request req1{ "http://httpbin.org/delay/5", request_done };
    request req2{ "http://httpbin.org/delay/4", request_done };
    
    cl.add_request(std::move(req1));
    cl.add_request(std::move(req2));

    while (cl.num_requests()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(32));
    }

    return 0;
}