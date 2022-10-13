#include<iostream>
#include<fstream>
#include<stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

using namespace std;

class Client_socket{
    fstream file;

    int PORT;
    
    int general_socket_descriptor;
    int new_socket_descriptor;
    struct sockaddr_in address;
    int address_length;
    string msg;
    string filename = "../Data/out1.png";
    char buffer[1024] = { 0 };
    public:
    Client_socket(int port){
            create_socket();
            PORT = port;

            address.sin_family = AF_INET;
            address.sin_port = htons( PORT );
            address_length = sizeof(address);
            if(inet_pton(AF_INET, "127.0.0.1", &address.sin_addr)<=0) { 
                cout<<"[ERROR] : Invalid address\n";
            }

            create_connection();
            
           
        }

        void setMsg(string s){
        msg = s;
    }

        

        void create_socket(){
            if ((general_socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) { 
                perror("[ERROR] : Socket failed.\n");
                exit(EXIT_FAILURE);
            }
            cout<<"[LOG] : Socket Created Successfully.\n";
        }

        void create_connection(){
            if (connect(general_socket_descriptor, (struct sockaddr *)&address, sizeof(address)) < 0) { 
                perror("[ERROR] : connection attempt failed.\n");
                exit(EXIT_FAILURE);
            }
            cout<<"[LOG] : Connection Successfull.\n";
        }

        void receive_file(){
            size_t datasize = 1;
            char text[256];
            FILE* fd = fopen(filename.c_str(), "wb");
            while (datasize > 0)
            {
                datasize = recv(general_socket_descriptor, text, sizeof(text), 0);
                fwrite(&text, 1, datasize, fd);
            }
            fclose(fd);
            cout<<"[LOG] : File Saved.\n";}
        

        void read_msg(){
            
           
            int valread = read(general_socket_descriptor, buffer, 1024);
             printf("%s\n", buffer);
             
        }
        void accept_connection(){
            if ((new_socket_descriptor = accept(general_socket_descriptor, (struct sockaddr *)&address, (socklen_t*)&address_length))<0) { 
                perror("[ERROR] : Accept");
                exit(EXIT_FAILURE);
            }
            cout<<"[LOG] : Connected to Client.\n";
        }
       
        void transmit_file(){
            char buffer[256];
            FILE *fd = fopen(filename.c_str(), "rb");
            size_t rret, wret;
            int bytes_read;
            while (!feof(fd)) {
                if ((bytes_read = fread(&buffer, 1, sizeof(buffer), fd)) > 0)
                    send(general_socket_descriptor, buffer, bytes_read, 0);
                else
                    break;
            }
                fclose(fd);
        }
    

        void transmit_msg(){
            
            
            send(general_socket_descriptor , msg.c_str(), msg.length() , 0);
            printf("message sent\n");
        }
    
    
    
    };




int main(){
    cout<<"Enter your port no.: ";
    int curr_port;
    cin>>curr_port; 

    Client_socket S(curr_port);
    // S.setMsg("Hello server i'm c1");
    while(1)
   { 
    string x;
    cin>>x;
    S.setMsg(x);
    cout<<"temp :"<<x<<endl;
    S.transmit_msg();
   
    S.read_msg();}
    return 0;
}
