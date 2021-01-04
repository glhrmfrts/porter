#include <curl/curl.h>
#include "porter/request.hpp"

namespace porter {

request::request() {
    _handle = curl_easy_init();
}

request::request(const std::string& url) {
    _handle = curl_easy_init();
    set_url(url);
}

request::request(const std::string& url, request::done_callback_func func) {
    _handle = curl_easy_init();
    set_url(url);
    set_done_callback(func);
}

request::~request() {
    if (_handle) curl_easy_cleanup(_handle);
}

request& request::operator =(request&& other) noexcept {
    _handle = other._handle;
    _done_callback = other._done_callback;
    other._handle = NULL;
    other._done_callback = {};
    return *this;
}

CURLM* request::curl_handle() {
    return _handle;
}

request::done_callback_func request::done_callback() {
    return _done_callback;
}

int request::perform() {
    return curl_easy_perform(_handle);
}

void request::set_done_callback(done_callback_func func) {
    _done_callback = func;
}

void request::set_post_data(const char* data, std::size_t size) {
    curl_easy_setopt(_handle, CURLOPT_POSTFIELDS, data);
    curl_easy_setopt(_handle, CURLOPT_POSTFIELDSIZE, size);
}

void request::set_url(const std::string& url) {
    curl_easy_setopt(_handle, CURLOPT_URL, url.data());
}

long request::status_code() {
    long http_code = 0;
    curl_easy_getinfo(_handle, CURLINFO_RESPONSE_CODE, &http_code);
    return http_code;
}

}