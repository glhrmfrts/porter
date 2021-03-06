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
    curl_slist_free_all(_headers);
    if (_handle) curl_easy_cleanup(_handle);
}

base_request& base_request::operator =(base_request&& other) noexcept {
    _post_data = std::move(other._post_data);
    _response = std::move(other._response);
    _write_callback = std::move(other._write_callback);
    _handle = other._handle;
    _headers = other._headers;
    _prepared = other._prepared;
    other._handle = NULL;
    other._headers = NULL;
    curl_easy_setopt(_handle, CURLOPT_WRITEFUNCTION, raw_write_callback);
    curl_easy_setopt(_handle, CURLOPT_WRITEDATA, static_cast<void*>(this));
    return *this;
}

void base_request::add_header(const std::string& name, const std::string& value) {
    const size_t len = name.size() + value.size() + 10;
    char* header = (char*)malloc(len);
    snprintf(header, len, "%s: %s", name.c_str(), value.c_str());
    _headers = curl_slist_append(_headers, header);
    free(header);
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

void base_request::set_method(method m) {
    curl_easy_setopt(_handle, CURLOPT_CUSTOMREQUEST, method_name(m));
}

void base_request::set_data(const char* data, std::size_t size) {
    std::copy(data, data + size, std::back_inserter(_post_data));
    curl_easy_setopt(_handle, CURLOPT_POSTFIELDS, _post_data.data());
    curl_easy_setopt(_handle, CURLOPT_POSTFIELDSIZE, _post_data.size());
}

void base_request::set_url(const std::string& url) {
    curl_easy_setopt(_handle, CURLOPT_URL, url.data());
}

void base_request::set_ssl_verify_peer(bool verify) {
    curl_easy_setopt(_handle, CURLOPT_SSL_VERIFYPEER, verify);
}

void base_request::set_timeout(const std::chrono::milliseconds &ms) {
    curl_easy_setopt(_handle, CURLOPT_TIMEOUT_MS, (long)ms.count());
    curl_easy_setopt(_handle, CURLOPT_NOSIGNAL, 1);
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

void base_request::_prepare() {
    if (_prepared) return;
    
    curl_easy_setopt(_handle, CURLOPT_HTTPHEADER, _headers);
    _prepared = true;
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
    _prepare();
    return curl_easy_perform(_handle);
}


}