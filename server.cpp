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

    //server infinite loop
    while(true)
    {
        struct sockaddr_in from;
        socklen_t from_len = sizeof(from);

        int connected_sock = accept(listen_sock, (struct sockaddr*)&from, &from_len);
        if(connected_sock < 0)
        {
            cout<<"Accept error"<<endl;
            continue;
        }
        //new thread for client
        pthread_t client_thread;
        int* client_sock_ptr = new int(connected_sock);
        if(pthread_create(&client_thread, NULL, handle_client, (void*)client_sock_ptr)!=0)
        {
            cout <<"Failed to create thread"<<endl;
            close(connected_sock);
            delete client_sock_ptr;
            continue;
        }
        //cleanup thread
        pthread_detach(client_thread);
    }
    close(listen_sock);
    return SUCCESS;
}

void* handle_client(void* client_sock)
{
    int connected_sock = *(int*)client_sock;
    delete(int*)client_sock;

    char buffer[MAX_BUFFER];
    int bytes_read;

    //read command
    string command;
    while((bytes_read = read(connected_sock, buffer, MAX_BUFFER -1))>0)
    {
        buffer[bytes_read] = '\0';
        command += buffer;
        if(command.back()=='\n')
            break;
    }

    if(bytes_read<=0){
        close(connected_sock);
        return NULL;
    }
    //do command
    istringstream iss(command);
    string cmd;
    iss>>cmd;
    string response;
    pthread_mutex_lock(&acc_mutex);
    if(cmd=="get")
    {
        response = to_string(accumulator)+"\n";
    }
    else if(cmd=="add")
    {
        int value;
        if(iss>>value)
        {
            accumulator+=value;
            response = to_string(accumulator)+"\n";
        }
    }
    else if(cmd=="clear")
    {
        accumulator = 0;
        response = "";
    }
    pthread_mutex_unlock(&acc_mutex);

    //response
    if(!response.empty())
    {
        int needed = response.length();
        const char* cbuff = response.c_str();
        while(needed>0)
        {
            int n = write(connected_sock, cbuff, needed);
            if(n<=0)
                break;
            needed -=n;
            cbuff +=n;
        }
    }

    //connection
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    getpeername(connected_sock, (struct sockaddr*)&client_addr, &addr_len);
    cout<<"Handled command '"<<cmd<<"'from " <<inet_ntoa(client_addr.sin_addr) <<endl;

    close(connected_sock);
    return NULL;
}