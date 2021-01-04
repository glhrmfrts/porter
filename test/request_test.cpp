#include <cassert>
#include <curl/curl.h>
#include <porter/request.hpp>
#include <iostream>

using namespace porter;

int main(int argc, char** argv) {
    request req{ "http://httpbin.org/status/200" };
    
    assert(req.perform() == CURLE_OK);
    
    assert(req.status_code() == 200);

    return 0;
}