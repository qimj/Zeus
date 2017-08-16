//
// Created by comoon on 5/19/17.
//

#ifndef ZEUS_PROTOCOLPARSER_H
#define ZEUS_PROTOCOLPARSER_H

#include "Log.h"
#include "IOBuf.h"
#include "Connection.h"

#include <cstdint>
#include <memory>

#define MAGIC 0xFF9527FF

struct Protocol {
    struct MSG {
        uint32_t magic;
        uint32_t protocolId;
        uint32_t dataLen;
        char data[0];
    }__attribute__((packed));

    static_assert(sizeof(MSG)==12,"MSG size = 12");
};

class ProtocolParser {
public:

    typedef std::shared_ptr<Protocol::MSG> _REQ;
    typedef std::shared_ptr<Protocol::MSG> _RSP;
    typedef std::function<void(const std::shared_ptr<IOBuf> &&)> FnOnMsg;

    void OnMsg(const FnOnMsg & fnOnMsg) { _fnOnMsg = fnOnMsg; }
    bool ParseMsg(int fd) {

        while(true) {

            //even not recved a full head
            if(_recvedLen < sizeof(Protocol::MSG)) {

                uint64_t remainLen = sizeof(Protocol::MSG) - _recvedLen;
                int readlen = ::recv(fd, &_head +_recvedLen, remainLen, 0);

                LOG_DEBUG << "Head recv : " << readlen << " remain : " << remainLen - readlen << endl;
                if(-1 != readlen)
                    _recvedLen += readlen;

                if (readlen != remainLen) {
                    throw_system_error_on(-1 == readlen && errno != EAGAIN, "recv");

                    if (readlen == 0)
                        return false;

                    return true;
                }

                //head check things
                if (_head.magic != MAGIC)
                    return false;
            }

            if(nullptr == _buf)
                _buf.reset(new IOBuf((size_t) _head.dataLen));

            uint64_t remainLen = _head.dataLen - (_recvedLen - sizeof(Protocol::MSG));
            int readlen = ::recv(fd, _buf->get(), remainLen, 0);

            LOG_DEBUG << "Body len : " << _head.dataLen << " remain : " << remainLen - readlen << endl;

            if (readlen != remainLen) {
                throw_system_error_on(-1 == readlen && errno != EAGAIN, "recv");

                if (readlen == 0)
                    return false;

                if(readlen != -1) {
                    _recvedLen += readlen;
                    _buf->Forward(readlen);
                }
                return true;
            } else {
                _buf->Forward(_head.dataLen);
                if(_fnOnMsg)
                    _fnOnMsg(std::move(_buf));

                _buf = nullptr;
                _recvedLen = 0;
            }
        }
    }

private:

    FnOnMsg _fnOnMsg;
    size_t _recvedLen {0};
    std::shared_ptr<IOBuf> _buf {nullptr};
    Protocol::MSG _head;
};

#endif //ZEUS_PROTOCOLPARSER_H
