#include <netdb.h>
#include <unistd.h>
#include <iostream>
#include <string>
using namespace std;

#define SUCCESS 0
#define USAGE_ERROR 1
#define CREATE_ERROR 2
#define HOSTNAME_ERROR 3
#define SOCK_ERROR 4
#define READ_ERROR 5

#define MAX_BUFFER 256


int do_client(const char* server, unsigned int port);
int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        cout<<"Usage: "<<argv[0]<<" <server> <port numnber>"<<endl;
        exit(USAGE_ERROR);
    }

    return do_client(argv[1], atoi(argv[2]));
}

int do_client(const char* server, unsigned int port)
{
    struct hostent *server_entry = gethostbyname(server);
    if(!server_entry)
    {
        cout<< "Failed gethostbyname(), server="<<server<<endl;
        exit(HOSTNAME_ERROR);
    }
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock<0)
    {
        cout<<"Failed socket()"<<endl;
        exit(CREATE_ERROR);
    }
    struct sockaddr_in server_info;
    server_info.sin_family = AF_INET;
    server_info.sin_addr = *(struct in_addr*)server_entry->h_addr_list[0];
    server_info.sin_port = htons(port);

    if(connect(sock, (struct sockaddr*)&server_info, sizeof(server_info))<0)
    {
        cout<<"Failed connect()"<<endl;
        close(sock);
        exit(SOCK_ERROR);
    }
    //read commands
    string command;
    cout<<"Enter command (get, clear, add <intval>): ";
    getline(cin, command);
    command+="\n";

    //send command to server
    int needed = command.length();
    const char* cbuff = command.c_str();
    while(needed>0)
    {
        int n = write(sock, cbuff, needed);
        if(n<=0)
        {
            cout<<"write() error"<<endl;
            close(sock);
            exit(SOCK_ERROR);

        }
        needed -=n;
        cbuff +=n;
    }


    //responce "get" "add"
    if(command.find("get")==0||command.find("add")==0)
    {
        char buffer[MAX_BUFFER];
        int n = read(sock,buffer,MAX_BUFFER-1);
        if(n<0)
        {
            cout<<"read() error"<<endl;
            close(sock);
            exit(READ_ERROR);

        }
        if(n>0)
        {
            buffer[n]='\0';
            cout<<buffer;
        }
    }
    close(sock);
    return SUCCESS;
    
}