#include <curl/curl.h>
#include "porter/request.hpp"

namespace porter {

namespace {
size_t raw_write_callback(void* ptr, size_t size, size_t nmemb, void* ud) {
    auto req = static_cast<base_request*>(ud);
    if (!req->curl_handle()) return 0;

    const size_t actual_size = size * nmemb;
    auto data = static_cast<char*>(ptr);
    std::copy(data, data + actual_size, std::back_inserter(req->response()));

    if (bool(req->write_callback())) {
        req->write_callback()(data, actual_size);
    }

    return actual_size;
}
}


base_request::base_request() {
    _handle = _create_handle();
}

base_request::base_request(const std::string& url) {
    _handle = _create_handle();
    set_url(url);
}

base_request::~base_request() {
    if (_handle) curl_easy_cleanup(_handle);
}

base_request& base_request::operator =(base_request&& other) noexcept {
    _response = std::move(other._response);
    _write_callback = std::move(other._write_callback);
    _handle = other._handle;
    other._handle = NULL;
    curl_easy_setopt(_handle, CURLOPT_WRITEFUNCTION, raw_write_callback);
    curl_easy_setopt(_handle, CURLOPT_WRITEDATA, static_cast<void*>(this));
    return *this;
}

CURLM* base_request::curl_handle() {
    return _handle;
}

std::vector<char>& base_request::response() {
    return _response;
}

const std::vector<char>& base_request::response() const {
    return _response;
}

void base_request::set_post_data(const char* data, std::size_t size) {
    curl_easy_setopt(_handle, CURLOPT_POSTFIELDS, data);
    curl_easy_setopt(_handle, CURLOPT_POSTFIELDSIZE, size);
}

void base_request::set_url(const std::string& url) {
    curl_easy_setopt(_handle, CURLOPT_URL, url.data());
}

long base_request::status_code() const {
    long http_code = 0;
    curl_easy_getinfo(_handle, CURLINFO_RESPONSE_CODE, &http_code);
    return http_code;
}

base_request::write_callback_func base_request::write_callback() const {
    return _write_callback;
}

CURLM* base_request::_create_handle() {
    CURLM* handle = curl_easy_init();
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, raw_write_callback);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, static_cast<void*>(this));
    return handle;
}


async_request::async_request() : base_request{}, _result_code{ 0 } {
}

async_request::async_request(const std::string& url, async_request::done_callback_func func) : base_request{ url }, _result_code{ 0 } {
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