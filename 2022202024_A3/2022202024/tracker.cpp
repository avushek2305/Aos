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
fstream file;

    int PORT;
    
    int general_socket_descriptor;
    

    struct sockaddr_in address;
    int address_length;
    char buffer[1024] = { 0 };
    string filename = "../Data/2.png";
    string msg;

void setMsg(string s){
    msg = s;
}

void create_socket(){
            if ((general_socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) == 0) { 
                perror("[ERROR] : Socket failed");
                exit(EXIT_FAILURE);
            }
            cout<<"[LOG] : Socket Created Successfully.\n";
        }

        void bind_socket(){
            if (bind(general_socket_descriptor, (struct sockaddr *)&address, sizeof(address))<0) {
                perror("[ERROR] : Bind failed");
                exit(EXIT_FAILURE);
            }
            cout<<"[LOG] : Bind Successful.\n";
        }

        void set_listen_set(){
            if (listen(general_socket_descriptor, 3) < 0) {
                perror("[ERROR] : Listen");
                exit(EXIT_FAILURE);
            }
            cout<<"[LOG] : Socket in Listen State (Max Connection Queue: 3)\n";
        }

        int accept_connection(){
            int new_socket_descriptor;
            if ((new_socket_descriptor = accept(general_socket_descriptor, (struct sockaddr *)&address, (socklen_t*)&address_length))<0) { 
                perror("[ERROR] : Accept");
                exit(EXIT_FAILURE);
            }
            return new_socket_descriptor;;
            cout<<"[LOG] : Connected to Client.\n";
        }

        void transmit_file(int new_socket_descriptor){
            char buffer[256];
            FILE *fd = fopen(filename.c_str(), "rb");
            size_t rret, wret;
            int bytes_read;
            while (!feof(fd)) {
                if ((bytes_read = fread(&buffer, 1, sizeof(buffer), fd)) > 0)
                    send(new_socket_descriptor, buffer, bytes_read, 0);
                else
                    break;
            }
                fclose(fd);
        }


        void transmit_msg(int new_socket_descriptor){
            send(new_socket_descriptor , msg.c_str() , msg.length() , 0);
            printf("message sent\n");
        }

        void receive_file(int new_socket_descriptor){
            size_t datasize = 1;
            char text[256];
            FILE* fd = fopen(filename.c_str(), "wb");
            while (datasize > 0)
            {
                datasize = recv(new_socket_descriptor, text, sizeof(text), 0);
                fwrite(&text, 1, datasize, fd);
            }
            fclose(fd);
            cout<<"[LOG] : File Saved.\n";
        }
        void read_msg(int new_socket_descriptor){
           
            int valread = read(new_socket_descriptor, buffer, 1024);
             printf("%s\n", buffer);
           
        }
        void *threads_code(void* n_s_d){
            int new_socket_descriptor = *(int *)n_s_d;
            while(1)
             {
                read_msg(new_socket_descriptor);

                setMsg("msg is being sent by us !");
                transmit_msg(new_socket_descriptor);
                }
        }
void *Server_code(void* port){
    create_socket();
            PORT = *(int *)port;

            address.sin_family = AF_INET; 
            address.sin_addr.s_addr = INADDR_ANY; 
            address.sin_port = htons( PORT );
            address_length = sizeof(address);

            bind_socket();
            set_listen_set();
            while(1){

                int new_socket_descriptor = accept_connection();

                pthread_t c;
                pthread_attr_t t_attr2;
                int res2 = pthread_attr_init(&t_attr2);
                if (res2 != 0) {
                    perror("Attribute creation failed");
                    exit(EXIT_FAILURE);
                }
                

                if( pthread_create( &c ,&t_attr2 ,  threads_code , (void*)&new_socket_descriptor) < 0)
                {
                    perror("could not create thread");
                    
                }
               
            }


}


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
            while(true)
            {
            int valread = read(general_socket_descriptor, buffer, 1024);
             printf("%s\n", buffer);
             if(valread <= 0){
                break;
             }
             }
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
            send(general_socket_descriptor , msg.c_str() , msg.length() , 0);
            printf("message sent\n");
        }
    
    
    
    };





    int main(){
    cout<<"Enter your port no.: ";
    int curr_port;
    cin>>curr_port; 

    pthread_t S;
    pthread_attr_t thread_attr2;
    int res2 = pthread_attr_init(&thread_attr2);
    if (res2 != 0) {
        perror("Attribute creation failed");
        exit(EXIT_FAILURE);
    }
    

    if( pthread_create( &S , NULL ,  Server_code , (void*)&curr_port) < 0)
    {
        perror("could not create thread");
        return 1;
    }
    cout<<"yha phuch"<<endl;
   string c ;
while(1)
{cout<<"enter c:"<<endl;
cin>>c;
if(c=="quit")
break;
}
    return 0;
}