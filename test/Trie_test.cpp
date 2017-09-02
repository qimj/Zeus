//
// Created by comoon on 8/27/17.
//

#include <iostream>
#include <gtest/gtest.h>
#include "src/HttpRouting.h"

using namespace std;

TEST(Trie, test){

    array<HttpRequest, 5> reqs;
    HttpResponse rep;
    reqs[0]._url = "a/b/c/d"; reqs[0]._method = HttpRequest::Method::POST;
    reqs[1]._url = "a/c";     reqs[1]._method = HttpRequest::Method::GET;
    reqs[2]._url = "c/d";     reqs[2]._method = HttpRequest::Method::PUT;
    reqs[3]._url = "b/c/e";   reqs[3]._method = HttpRequest::Method::POST;
    reqs[4]._url = "e/d/f";   reqs[4]._method = HttpRequest::Method::POST;

    Router router;
    router.new_rule("a/b/c/d").methods("POST")(
            [](const HttpRequest& req, HttpResponse&){
                LOG_DEBUG << "it's " << req._url << endl;
            }
    );

    router.new_rule("a/c").methods("GET")(
            [](const HttpRequest& req, HttpResponse&){
                LOG_DEBUG << "it's " << req._url << endl;
            }
    );

    router.new_rule("c/d").methods("PUT")(
            [](const HttpRequest& req, HttpResponse&){
                LOG_DEBUG << "it's " << req._url << endl;
            }
    );

    router.new_rule("b/c/e").methods("OPTIONS");

    router.new_rule("e/d/f").methods("DELETE")(
            [](const HttpRequest& req, HttpResponse&){
                LOG_DEBUG << "it's " << req._url << endl;
            }
    );

//    router.debug_print();
    router.handler(reqs[0], rep);
    router.handler(reqs[1], rep);
    router.handler(reqs[2], rep);
    router.handler(reqs[3], rep);
    router.handler(reqs[4], rep);
}