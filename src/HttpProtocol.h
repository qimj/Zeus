//
// Created by comoon on 8/22/17.
//

#ifndef ZEUS_HTTPPROTOCOL_H
#define ZEUS_HTTPPROTOCOL_H

#include <map>

using std::map;
using std::string;

struct HttpRequest {
    enum class Method {
        GET,
        POST,
        PUT,
        DELETE,
        OPTIONS,
        OTHER
    };

    map<string, string> _heads;
    map<string, string> _queries;
    Method _method;
    string _url;
    string _body;
};

struct HttpResponse {
    string _body;
    map<string, string> _heads;
};

#endif //ZEUS_HTTPPROTOCOL_H
