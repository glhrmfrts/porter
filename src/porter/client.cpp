#include <algorithm>
#include <curl/curl.h>
#include "porter/client.hpp"

namespace porter {

client::client() {
    _running = true;
    _multi_handle = curl_multi_init();
    _client_thread = std::thread{ &client::_client_loop, this };
}

client::~client() {
    _running = false;
    if (_client_thread.joinable()) {
        _client_thread.join();
    }
    curl_multi_cleanup(_multi_handle);
}

void client::add_request(async_request&& req) {
    std::lock_guard<std::mutex> lock{ _async_requests_mutex };
    curl_multi_add_handle(_multi_handle, req.curl_handle());

    auto ptr = new async_request{ std::move(req) };
    ptr->_prepare();
    _async_requests.push_back(std::unique_ptr<async_request>{ ptr });
}

std::size_t client::num_requests() {
    std::lock_guard<std::mutex> lock{ _async_requests_mutex };
    return _async_requests.size();
}

void client::_client_loop() {
    int still_running = 0; /* keep number of running handles */ 
    int i;

    CURLMsg *msg; /* for picking up messages with the transfer status */ 
    int msgs_left; /* how many messages are left */ 

    CURLM* mhandle = (_multi_handle);

    /* we start some action by calling perform right away */ 
    curl_multi_perform(mhandle, &still_running);

    while (_running || still_running) {
        struct timeval timeout;
        int rc; /* select() return code */ 
        CURLMcode mc; /* curl_multi_fdset() return code */ 
    
        fd_set fdread;
        fd_set fdwrite;
        fd_set fdexcep;
        int maxfd = -1;
    
        long curl_timeo = -1;
    
        FD_ZERO(&fdread);
        FD_ZERO(&fdwrite);
        FD_ZERO(&fdexcep);
    
        /* set a suitable timeout to play around with */ 
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
    
        curl_multi_timeout(mhandle, &curl_timeo);
        if(curl_timeo >= 0) {
            timeout.tv_sec = curl_timeo / 1000;
            if(timeout.tv_sec > 1)
                timeout.tv_sec = 1;
            else
                timeout.tv_usec = (curl_timeo % 1000) * 1000;
        }
    
        /* get file descriptors from the transfers */ 
        mc = curl_multi_fdset(mhandle, &fdread, &fdwrite, &fdexcep, &maxfd);
    
        if(mc != CURLM_OK) {
            fprintf(stderr, "curl_multi_fdset() failed, code %d.\n", mc);
            break;
        }

        /* On success the value of maxfd is guaranteed to be >= -1. We call
        select(maxfd + 1, ...); specially in case of (maxfd == -1) there are
        no fds ready yet so we call select(0, ...) --or Sleep() on Windows--
        to sleep 100ms, which is the minimum suggested value in the
        curl_multi_fdset() doc. */ 
    
        if(maxfd == -1) {
#ifdef _WIN32
            Sleep(100);
            rc = 0;
#else
            /* Portable sleep for platforms other than Windows. */ 
            struct timeval wait = { 0, 100 * 1000 }; /* 100ms */ 
            rc = select(0, NULL, NULL, NULL, &wait);
#endif
        }
        else {
            /* Note that on some platforms 'timeout' may be modified by select().
                If you need access to the original value save a copy beforehand. */ 
            rc = select(maxfd + 1, &fdread, &fdwrite, &fdexcep, &timeout);
        }

        switch(rc) {
        case -1:
            /* select error */ 
            break;
        case 0: /* timeout */ 
        default: /* action */ 
            curl_multi_perform(mhandle, &still_running);
            break;
        }        

        /* See how the transfers went */ 
        while((msg = curl_multi_info_read(mhandle, &msgs_left))) {
            if(msg->msg == CURLMSG_DONE) {
                std::unique_ptr<async_request> req;
            
                /* Find out which handle this message is about */
                {
                    std::lock_guard<std::mutex> lock{ _async_requests_mutex };
                    auto remove_it = std::remove_if(_async_requests.begin(), _async_requests.end(), [msg, &req](std::unique_ptr<async_request>& r) {
                        if (r->curl_handle() == msg->easy_handle) {
                            req = std::move(r);
                            return true;
                        }
                        return false;
                    });
                    _async_requests.erase(remove_it, _async_requests.end());
                    if (req) {
                        curl_multi_remove_handle(mhandle, req->curl_handle());
                        req->_result_code = msg->data.result;
                    }
                }

                if (req && bool(req->done_callback())) {
                    req->done_callback()(*req, req->_result_code);
                }
            }
        }
    }
}

}