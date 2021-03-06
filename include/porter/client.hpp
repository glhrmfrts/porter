#ifndef porter_client_hpp
#define porter_client_hpp

#include <list>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>
#include "porter/request.hpp"

namespace porter {

class client {
public:
    explicit client();

    ~client();

    /// Copying not allowed
    client(const client&) = delete;

    /// Moving not allowed, use ptrs
    client(client&&) = delete;

    /// Copying not allowed
    client& operator =(const client&) = delete;

    /// Moving not allowed, use ptrs
    client& operator =(client&&) = delete;

    /// Adds a new async_request to this client
    void add_request(async_request&& req);

    /// Returns the number of current async_requests
    std::size_t num_requests();

private:
    void _client_loop();

    std::list<std::unique_ptr<async_request>> _async_requests;
    std::mutex _async_requests_mutex;
    std::thread _client_thread;
    std::atomic_bool _running;
    void* _multi_handle;
};

}

#endif