#ifndef porter_method_hpp
#define porter_method_hpp

namespace porter {

enum class method {
    GET,
    POST,
    PUT,
    PATCH,
    DEL,
    HEAD,
    OPTIONS,
};

const char* method_name(method m);

}

#endif