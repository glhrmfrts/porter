#ifndef porter_request_hpp
#define porter_request_hpp

#include <functional>
#include <string>

typedef void CURLM;

namespace porter {

class base_request {
public:
    explicit base_request();

    explicit base_request(const std::string& url);

    virtual ~base_request();

    /// Copying not allowed
    base_request(const base_request&) = delete;

    base_request(base_request&& other) noexcept { *this = std::move(other); }

    /// Copying not allowed
    base_request& operator =(const base_request&) = delete;

    base_request& operator =(base_request&& other) noexcept;

    CURLM* curl_handle();

    void set_post_data(const char* data, std::size_t size);

    void set_url(const std::string& url);

    long status_code();

protected:
    CURLM* _handle;
};

class async_request : public base_request {
public:
    using done_callback_func = std::function<void(async_request&)>;

    explicit async_request();

    explicit async_request(const std::string& url, done_callback_func func);

    async_request(async_request&& other) noexcept { *this = std::move(other); }

    async_request& operator =(async_request&& other) noexcept;

    done_callback_func done_callback();

    void set_done_callback(done_callback_func func);

private:
    done_callback_func _done_callback;
};

class sync_request : public base_request {
public:
    using base_request::base_request;

    int perform();
};

}

#endif