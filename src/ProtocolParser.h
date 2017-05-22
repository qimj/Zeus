//
// Created by comoon on 5/19/17.
//

#ifndef ZEUS_PROTOCOLPARSER_H
#define ZEUS_PROTOCOLPARSER_H

#include "IOBuf.h"
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
    typedef std::function<void(const _REQ&)> FnOnMsg;

    void OnMsg(const FnOnMsg & fnOnMsg) { _fnOnMsg = fnOnMsg; }
    bool ParseMsg(int fd) {

        while(true) {

            auto head = std::make_shared<Protocol::MSG>();
            int readlen = recv(fd, head.get(), sizeof(Protocol::MSG), 0);

            if(readlen != sizeof(Protocol::MSG)){
                if(readlen == -1 && errno != EAGAIN){
                    //error
                } else if(readlen == -1) {
                    return true;
                }else if(readlen == 0) {
                    return true;
                }
                return false;
            }

            if (head->magic != MAGIC)
                return false;

            //TODO check dataLen
            auto ioBuf = std::make_shared<IOBuf>((size_t)head->dataLen);
            readlen = recv(fd, ioBuf->Get().get(), head->dataLen, 0);
            ioBuf->Forward(head->dataLen);

            if(readlen != head->dataLen){
                if(readlen == -1 && errno != EAGAIN){
                    //error
                } else if(readlen == -1) {

                }else if(readlen == 0) {
                    //client closed
                }
                return false;
            }

            ioBuf->Print();
        }
    }

private:

    FnOnMsg _fnOnMsg;
};

#endif //ZEUS_PROTOCOLPARSER_H
