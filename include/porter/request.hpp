#ifndef porter_request_hpp
#define porter_request_hpp

#include <functional>
#include <string>
#include <vector>
#include "porter/method.hpp"

typedef void CURLM;

struct curl_slist;

namespace porter {

class base_request {
public:
    using write_callback_func = std::function<void(const char*, std::size_t)>;

    explicit base_request();

    explicit base_request(const std::string& url);

    virtual ~base_request();

    /// Copying not allowed
    base_request(const base_request&) = delete;

    base_request(base_request&& other) noexcept { *this = std::move(other); }

    /// Copying not allowed
    base_request& operator =(const base_request&) = delete;

    base_request& operator =(base_request&& other) noexcept;

    void add_header(const std::string& name, const std::string& value);

    CURLM* curl_handle();

    std::vector<char>& response();

    const std::vector<char>& response() const;

    void set_method(method m);

    void set_data(const char* data, std::size_t size);

    void set_url(const std::string& url);

    void set_ssl_verify_peer(bool verify);

    void set_write_callback(write_callback_func func);

    long status_code() const;

    write_callback_func write_callback() const;

protected:
    friend class client;

    CURLM* _create_handle();

    void _prepare();

    std::vector<char> _post_data;
    std::vector<char> _response;
    CURLM* _handle;
    write_callback_func _write_callback;
    struct curl_slist *_headers = NULL;
    bool _prepared = false;
};

class async_request : public base_request {
public:
    using done_callback_func = std::function<void(const async_request&, int)>;

    explicit async_request();

    explicit async_request(const std::string& url, done_callback_func func);

    async_request(async_request&& other) noexcept { *this = std::move(other); }

    async_request& operator =(async_request&& other) noexcept;

    done_callback_func done_callback();

    void set_done_callback(done_callback_func func);

private:
    friend class client;

    done_callback_func _done_callback;
    int _result_code;
};

class sync_request : public base_request {
public:
    using base_request::base_request;

    int perform();
};

}

#endif