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
#include <functional>

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
    ProtocolParser(void *p) { _serverPointer = p; }
    virtual bool ParseMsg(int fd) = 0;

protected:
    std::shared_ptr<IOBuf> _buf {nullptr};
    void * _serverPointer {nullptr};
};

#endif //ZEUS_PROTOCOLPARSER_H
