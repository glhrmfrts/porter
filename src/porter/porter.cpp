#include <curl/curl.h>
#include "porter/porter.hpp"

namespace porter {

std::string str_error(int code) {
    return curl_easy_strerror((CURLcode)code);
}

}