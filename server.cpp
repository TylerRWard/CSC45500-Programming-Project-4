#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include <string>
#include <sstream>
using namespace std;

//error codes
#define SUCCESS 0
#define USAGE_ERROR 1
#define SOCK_ERROR 2
#define BIND_ERROR 3
#define LISTEN_ERROR 4

#define MAX_WAITING 25
#define MAX_BUFFER 256

static int accumulator = 0;
static pthread_mutex_t acc_mutex = PTHREAD_MUTEX_INITIALIZER;

int do_server(unsigned int on_port);
void* handle_client(void* client_sock);

int main(int argc, char *argv[]){
    if(argc!= 2){
        cout<<"Usage: "<<argv[0] <<" <port number>" <<endl;
        exit(USAGE_ERROR);
    }
    return do_server(atoi(argv[1]));
}

int do_server(unsigned int on_port){
    int listen_sock;
    struct sockaddr_in local_addr;

    listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(listen_sock < 0){
        cout<<"Count not creating listening socket!"<<endl;
        return SOCK_ERROR;
    }
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;
    local_addr.sin_port = htons(on_port);

    if (::bind(listen_sock, (sockaddr*)&local_addr, sizeof(local_addr))!=0){
        cout<< "Binding failed!"<<endl;
        close(listen_sock);
        return BIND_ERROR;
    }
    if(listen(listen_sock, MAX_WAITING)!=0){
        cout<<"Listen error"<<endl;
        return LISTEN_ERROR;
    }
    cout<<"Server listening on port "<<on_port<<endl;
    while(true)
    {
        struct sockaddr_in from;
        socklen_t from_len = sizeof(from);
    }
}