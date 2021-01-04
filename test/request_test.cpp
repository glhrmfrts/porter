#include <cassert>
#include <curl/curl.h>
#include <porter/porter.hpp>
#include <iostream>

using namespace porter;

int main(int argc, char** argv) {
    global_guard _;
    sync_request req{ "http://httpbin.org/status/200" };
    
    assert(req.perform() == CURLE_OK);
    
    assert(req.status_code() == 200);

    return 0;
}