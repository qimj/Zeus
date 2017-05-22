#include <iostream>
#include "src/TcpServer.h"

using namespace std;

int main() {
    
    TcpServer sv;
    sv.listen(5660, "127.0.0.1");

    sv.onNewConnect([](const std::shared_ptr<Connection>& con){
        cout << "a new connection is coming!" << endl;
    });

    sv.start();
    
    return 0;
}