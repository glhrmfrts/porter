#ifndef porter_request_hpp
#define porter_request_hpp

#include <functional>
#include <string>

typedef void CURLM;

namespace porter {

class request {
public:
    using done_callback_func = std::function<void(request&)>;

    explicit request();

    explicit request(const std::string& url);

    explicit request(const std::string& url, request::done_callback_func func);

    ~request();

    /// Copying not allowed
    request(const request&) = delete;

    request(request&& other) noexcept { *this = std::move(other); }

    /// Copying not allowed
    request& operator =(const request&) = delete;

    request& operator =(request&& other) noexcept;

    CURLM* curl_handle();

    done_callback_func done_callback();

    void set_done_callback(done_callback_func func);

    void set_post_data(const char* data, std::size_t size);

    void set_url(const std::string& url);

    int perform();

    long status_code();

private:
    CURLM* _handle;
    done_callback_func _done_callback;
};

}

#endif