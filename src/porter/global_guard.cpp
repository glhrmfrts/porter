#include <curl/curl.h>
#include "porter/global_guard.hpp"

namespace porter {

global_guard::global_guard() {
    curl_global_init(CURL_GLOBAL_ALL);
}

global_guard::~global_guard() {
    curl_global_cleanup();
}

}