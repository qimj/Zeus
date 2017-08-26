//
// Created by comoon on 8/21/17.
//

#ifndef ZEUS_HTTPPROTOCOLPARSER_H
#define ZEUS_HTTPPROTOCOLPARSER_H

#include "Util.h"
#include "HttpProtocol.h"
#include "ProtocolParser.h"
#include "http_parser/http_parser.h"

class HttpProtocolParser : public ProtocolParser{
public:

    typedef std::shared_ptr<HttpRequest>  _REQ;
    typedef std::shared_ptr<HttpResponse> _RSP;

    HttpProtocolParser(){
        http_parser_init(&_httpParser, HTTP_REQUEST);
        _httpParser.data = this;
        _httpSettings.on_url = on_url;
        _httpSettings.on_body=on_body;
        _httpSettings.on_header_field=on_header_field;
        _httpSettings.on_header_value=on_header_value;
        _httpSettings.on_status=on_status;
        _httpSettings.on_headers_complete=on_headers_complete;
        _httpSettings.on_message_begin=on_message_begin;
        _httpSettings.on_message_complete=on_message_complete;

        _httpReq = std::make_shared<HttpRequest>();
        _buf = std::make_shared<DynamicIOBuf>();
    }

    virtual bool ParseMsg(int fd) final {

        int readlen {1};
        size_t nparsed{0};
        size_t toParseSize{0};
        while(readlen > 0) {

            readlen = ::recv(fd, _buf->getHead(), DYNAMIC_BUF_BLOCK_SIZE, 0);

            throw_system_error_on(-1 == readlen && errno != EAGAIN, "recv");
            if(readlen == 0)
                return false;

            if(readlen != -1)
                toParseSize += readlen;

            if(readlen == DYNAMIC_BUF_BLOCK_SIZE)
                static_pointer_cast<DynamicIOBuf>(_buf)->TryToFit(DYNAMIC_BUF_BLOCK_SIZE);
            else
                break;
        }

        nparsed = http_parser_execute(&_httpParser, &_httpSettings, _buf->getHead(), toParseSize);
        if (_httpParser.upgrade) {
            LOG_DEBUG<< "http recv a new protocol" << endl;
        } else if (nparsed != toParseSize) {
            LOG_DEBUG << "Handle error. Usually just close the connection." << endl;
            return false;
        }

        return true;
    }

private:
    static int on_url(http_parser* parser, const char *at, size_t length) {
        assert(parser && parser->data);

        http_parser_url u;
        int ret=http_parser_parse_url(at, length, 0, &u);
        if(ret) {
            LOG_ERROR << "http url parse error" << endl;
            return 0;
        }

        auto _this = static_cast<HttpProtocolParser*>(parser->data);
        if(u.field_set & (1 << UF_PATH)){
            _this->_httpReq->_url = string(at + u.field_data[UF_PATH].off, u.field_data[UF_PATH].len);
        }

        if(u.field_set & (1 << UF_QUERY)){
            string realUrl = URLDecode(at + u.field_data[UF_QUERY].off,u.field_data[UF_QUERY].len);
            _this->_httpReq->_queries.insert({"", realUrl});  //TODO query things
        }

        return 0;
    }

    static int on_body(http_parser* parser, const char *at, size_t length) {
        assert(parser && parser->data);
        auto _this = static_cast<HttpProtocolParser*>(parser->data);
        _this->_httpReq->_body.append(at, length);
        return 0;
    }

    static int on_header_field(http_parser* parser, const char *at, size_t length) {
        assert(parser && parser->data);
        auto _this = static_cast<HttpProtocolParser*>(parser->data);
        _this->_currentHead.assign(at ,length);
        return 0;
    }

    static int on_header_value(http_parser* parser, const char *at, size_t length) {
        assert(parser && parser->data);
        auto _this = static_cast<HttpProtocolParser*>(parser->data);
        if(!_this->_currentHead.empty())
            _this->_httpReq->_heads.insert({_this->_currentHead, string(at, length)});

        return 0;
    }

    static int on_status(http_parser* parser, const char *at, size_t length) {
        assert(parser && parser->data);
        return 0;
    }

    static int on_headers_complete(http_parser* parser) {
        assert(parser && parser->data);
        auto _this = static_cast<HttpProtocolParser*>(parser->data);
        _this->_currentHead.clear();
        return 0;
    }

    static int on_message_begin(http_parser* parser) {
        assert(parser && parser->data);
        return 0;
    }

    static int on_message_complete(http_parser* parser) {
        assert(parser && parser->data);

        HttpRequest::Method method = HttpRequest::Method::OTHER;
        switch (parser->method) {
            case HTTP_GET:
                method = HttpRequest::Method::GET;
                break;
            case HTTP_POST:
                method = HttpRequest::Method::POST;
                break;
            case HTTP_PUT:
                method = HttpRequest::Method::PUT;
                break;
            case HTTP_DELETE:
                method = HttpRequest::Method::DELETE;
                break;
            case HTTP_OPTIONS:
                method = HttpRequest::Method::OPTIONS;
                break;
            default:
                method = HttpRequest::Method::OTHER;
                break;
        }

        auto _this = static_cast<HttpProtocolParser*>(parser->data);
        _this->_httpReq->_method = method;
        assert(_this->_fnOnMsg);
//        _this->_fnOnMsg(_this->_httpReq);   //TODO type does not match

        LOG_DEBUG << "url : " << _this->_httpReq->_url << endl;
        LOG_DEBUG << "body : " << _this->_httpReq->_body << endl;
        for(auto const &i : _this->_httpReq->_heads)
            LOG_DEBUG << i.first << ":" << i.second << endl;

        return 0;
    }

private:
    http_parser _httpParser;
    http_parser_settings _httpSettings;
    _REQ _httpReq;
    string _currentHead;
};

#endif //ZEUS_HTTPPROTOCOLPARSER_H
