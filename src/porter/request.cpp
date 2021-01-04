#include <curl/curl.h>
#include "porter/request.hpp"

namespace porter {

base_request::base_request() {
    _handle = curl_easy_init();
}

base_request::base_request(const std::string& url) {
    _handle = curl_easy_init();
    set_url(url);
}

base_request::~base_request() {
    if (_handle) curl_easy_cleanup(_handle);
}

base_request& base_request::operator =(base_request&& other) noexcept {
    _handle = other._handle;
    other._handle = NULL;
    return *this;
}

CURLM* base_request::curl_handle() {
    return _handle;
}

void base_request::set_post_data(const char* data, std::size_t size) {
    curl_easy_setopt(_handle, CURLOPT_POSTFIELDS, data);
    curl_easy_setopt(_handle, CURLOPT_POSTFIELDSIZE, size);
}

void base_request::set_url(const std::string& url) {
    curl_easy_setopt(_handle, CURLOPT_URL, url.data());
}

long base_request::status_code() {
    long http_code = 0;
    curl_easy_getinfo(_handle, CURLINFO_RESPONSE_CODE, &http_code);
    return http_code;
}


async_request::async_request() : base_request{} {
}

async_request::async_request(const std::string& url, async_request::done_callback_func func) : base_request{ url } {
    set_done_callback(func);
}

async_request& async_request::operator =(async_request&& other) noexcept {
    base_request::operator=(std::move(other));
    _done_callback = std::move(other._done_callback);
    return *this;
}

async_request::done_callback_func async_request::done_callback() {
    return _done_callback;
}

void async_request::set_done_callback(done_callback_func func) {
    _done_callback = func;
}


int sync_request::perform() {
    return curl_easy_perform(_handle);
}


}