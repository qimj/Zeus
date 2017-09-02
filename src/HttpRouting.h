//
// Created by comoon on 8/27/17.
//

#ifndef ZEUS_HTTPROUTING_H
#define ZEUS_HTTPROUTING_H

#include "Log.h"
#include "HttpProtocol.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <functional>

class Trie {
public:

    struct Node {
        unsigned rule_index{};
        std::unordered_map<std::string, unsigned> children;
    };

    Trie() : _nodes(1){
    }

    void add(const std::string &url, unsigned rule_index) {
        unsigned idx{0};
        for(unsigned i = 0; i < url.size(); i ++) {
            char c = url[i];
            std::string piece(&c, 1);
            if (!_nodes[idx].children.count(piece)) {
                auto new_node_idx = new_node();
                _nodes[idx].children.emplace(piece, new_node_idx);
            }
            idx = _nodes[idx].children[piece];
        }

        if (_nodes[idx].rule_index)
            throw std::runtime_error("handler already exists for " + url);
        _nodes[idx].rule_index = rule_index;
    }

    unsigned find(const std::string& req_url, const Node* node = nullptr, unsigned pos = 0) const {

        unsigned found{};
        if (node == nullptr)
            node = head();
        if (pos == req_url.size())
            return node->rule_index;

        auto update_found = [&found](unsigned& ret) {
            if (ret && (!found || found > ret))
                found = ret;
        };

        for(auto& kv : node->children) {
            const std::string& fragment = kv.first;
            const Node* child = &_nodes[kv.second];

            if (req_url.compare(pos, fragment.size(), fragment) == 0) {
                auto ret = find(req_url, child, pos + fragment.size());
                update_found(ret);
            }
        }

        return found;
    }

    void debug_print() {
        debug_node_print(head(), 0);
    }

private:
    inline const Node* head() const {
        return &_nodes.front();
    }

    inline Node* head() {
        return &_nodes.front();
    }

    unsigned new_node() {
        _nodes.resize(_nodes.size()+1);
        return _nodes.size() - 1;
    }

    void debug_node_print(Node* n, int level) {
        for(auto& kv : n->children) {
            LOG_DEBUG << std::string(2 * level, ' ') << "(" << kv.second << ") " << kv.first << endl;
            debug_node_print(&_nodes[kv.second], level+1);
        }
    }

private:
    std::vector<Node> _nodes;
};

class HttpRule {
public:
    typedef std::function<void(const HttpRequest&, HttpResponse&)> HttpHandler;

    HttpRule(std::string rule) : _rule(std::move(rule)) {}
    ~HttpRule() {}

    HttpRule& methods(const char* m) {
        if(0 == strcmp("GET", m))
            _method = HttpRequest::Method::GET;
        else if(0 == strcmp("POST", m))
            _method = HttpRequest::Method::POST;
        else if(0 == strcmp("PUT", m))
            _method = HttpRequest::Method::PUT;
        else if(0 == strcmp("DELETE", m))
            _method = HttpRequest::Method::DELETE;
        else if(0 == strcmp("OPTIONS", m))
            _method = HttpRequest::Method::OPTIONS;
        else
            throw std::runtime_error("Unkonwn Http Method!");

        return *this;
    }

    void operator()(const HttpHandler& handler) { _handler = handler; }

    void handle(const HttpRequest& req, HttpResponse& rep) {
        if(_handler)
            if(req._method == _method)
                _handler(req, rep);
            else
                LOG_DEBUG << "URL : " << _rule << " Method not match!" << endl;
        else
            LOG_DEBUG << "NO handler for URL : " << _rule << endl;
    }

protected:
    std::string _rule;
    HttpRequest::Method _method;
    HttpHandler _handler{nullptr};
};
using rulePtr = std::unique_ptr<HttpRule>;


class Router {
public:
    Router() : _rules(1){
    }

    HttpRule& new_rule(const std::string& rule){
        auto ruleObj = std::make_unique<HttpRule>(rule);

        _rules.emplace_back(std::move(ruleObj));
        _trie.add(rule, _rules.size() - 1);

        return *_rules[_rules.size() - 1].get();
    }

    void debug_print() {
        _trie.debug_print();
    }

    void handler(const HttpRequest& req, HttpResponse& rep) {
        auto rule_index = _trie.find(req._url);

        if(!rule_index){
            LOG_DEBUG << "Unknown URL : " << req._url << endl;
            return;
        }

        _rules[rule_index]->handle(req, rep);
    }

private:
    std::vector<rulePtr> _rules;
    Trie _trie;
};

#endif //ZEUS_HTTPROUTING_H
