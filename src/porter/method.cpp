#include "porter/method.hpp"

namespace porter {

const char* method_name(method m) {
    switch (m) {
    case method::GET:
        return "GET";
    case method::POST:
        return "POST";
    case method::PUT:
        return "PUT";
    case method::PATCH:
        return "PATCH";
    case method::DEL:
        return "DELETE";
    case method::HEAD:
        return "HEAD";
    case method::OPTIONS:
        return "OPTIONS";
    }
    return nullptr;
}

}