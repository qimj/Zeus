//
// Created by comoon on 8/20/17.
//

#ifndef ZEUS_TCPPROTOCOLPARSER_H
#define ZEUS_TCPPROTOCOLPARSER_H

#include "ProtocolParser.h"

class TcpProtocolParser : public ProtocolParser{
public:

    typedef std::shared_ptr<Protocol::MSG> _REQ;
    typedef std::shared_ptr<Protocol::MSG> _RSP;

    TcpProtocolParser(void *p) : ProtocolParser(p) {}
    
    virtual bool ParseMsg(int fd) final {

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
                _buf.reset(new ConstIOBuf((size_t) _head.dataLen));

            uint64_t remainLen = _head.dataLen - (_recvedLen - sizeof(Protocol::MSG));
            int readlen = ::recv(fd, _buf->getTail(), remainLen, 0);

            LOG_DEBUG << "Body len : " << _head.dataLen << " remain : " << remainLen - readlen << endl;

            if (readlen != remainLen) {
                throw_system_error_on(-1 == readlen && errno != EAGAIN, "recv");

                if (readlen == 0)
                    return false;

                if(readlen != -1) {
                    _recvedLen += readlen;
                    static_pointer_cast<ConstIOBuf>(_buf)->Forward(readlen);
                }
                return true;
            } else {
                static_pointer_cast<ConstIOBuf>(_buf)->Forward(_head.dataLen);

                //TODO OnMessgae

                _buf = nullptr;
                _recvedLen = 0;
            }
        }
    }

private:
    Protocol::MSG _head;
    size_t _recvedLen {0};
};

#endif //ZEUS_TCPPROTOCOLPARSER_H
