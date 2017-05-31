#include <iostream>
#include "src/TcpServer.h"

using namespace std;

int main() {

    auto mainLoop = std::make_shared<EventLoop>();
    
    TcpServer sv(mainLoop);
    sv.listen(5660, "127.0.0.1");

    sv.onNewConnect([](const std::shared_ptr<Connection>& con){
        cout << "a new connection is coming!" << endl;
    });

//    mainLoop->add_timer(Timer(10000, 3000, [](){
//        std::cout << "1" << std::endl;
//    }, true));

    mainLoop->loop_forever();
    return 0;
}