#include<iostream>
#include<fstream>
#include<stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <bits/stdc++.h>

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
    string MSG;
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
        MSG = s;
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
        

        string read_msg(){
            char buffer[1024] = { 0 };
            int valread = read(general_socket_descriptor, buffer, 1024);
             return buffer;
           
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
    

        void transmit_msg(string res){
            send(general_socket_descriptor , res.c_str() , res.length() , 0);
            printf("message sent\n");
        }
    
    
    
    };





    int main(){
    cout<<"Enter your port no.: ";
    int port,talk_port;
    string curr_port = to_string(port);
    cin>>curr_port>>talk_port;
    Client_socket S(talk_port); 
    map<string,vector<string>> GidTofiles;

    pthread_t x;
    pthread_attr_t thread_attr2;
    int res2 = pthread_attr_init(&thread_attr2);
    if (res2 != 0) {
        perror("Attribute creation failed");
        exit(EXIT_FAILURE);
    }
    

    if( pthread_create( &x , NULL ,  Server_code , (void*)&port) < 0)
    {
        perror("could not create thread");
        // return 1;
    }
    
 
   string ops;
   string flag;
   string usrName, pword,messg;
   bool cu = 1;
   bool lu = 1;
 while(1)
   { 
    
     cout<<"$|> ";
    cin>>ops;
    

    if(ops == "login" && lu){
       
       string usrN, pw,messg;
        cin>>usrN>>pw;
        string to_send = ops + "/" + usrN + "/"+ pw + "/" + curr_port ;
        S.transmit_msg(to_send);
        flag = S.read_msg();
        if(flag == "0"){
            usrName = usrN;
            lu = 0;
        }
        cout<<flag<<endl;
    }
    if(lu == 1){
         cout<<"[ERROR] : login required!"<<endl;
    }
    if(ops == "create_user" && cu){
        string usrN, pw,messg;
        cin>>usrN>>pw;
        string to_send = ops + "/" + usrN + "/"+ pw;
        S.transmit_msg(to_send);
        messg = S.read_msg();
        if(messg == "0"){
            cu = 0;
        }
        cout<<messg<<endl;
    }
    if(ops == "create_group" && flag == "0" ){
        string gID,messg;
        cin>>gID;
        string to_send = ops + "/" + gID +"/"+ usrName ;
        S.transmit_msg(to_send);
        messg = S.read_msg();
        cout<<messg<<endl;
    }
    if(ops == "join_group" && flag == "0" ){
       string gID,messg;
        cin>>gID;
        string to_send = ops + "/" + gID +"/"+ usrName ;
        S.transmit_msg(to_send);
        messg = S.read_msg();
        cout<<messg<<endl;
    }
    if(ops == "leave_group" && flag == "0" ){
       string gID,messg;
        cin>>gID;
        string to_send = ops + "/" + gID +"/"+ usrName ;
        S.transmit_msg(to_send);
        messg = S.read_msg();
        cout<<messg<<endl;
    }
    if(ops == "list_requests" && flag == "0" ){
       string gID,messg;
        cin>>gID;
        string to_send = ops + "/" + gID ;
        S.transmit_msg(to_send);
        messg = S.read_msg();
        cout<<messg<<endl;
    }
    if(ops == "accept_request" && flag == "0" ){
        string gID,messg,uid;
        cin>>gID>>uid;
        string to_send = ops + "/" + gID + "/" + uid + "/" + usrName;
        S.transmit_msg(to_send);
        messg = S.read_msg();
        cout<<messg<<endl;
    }
    if(ops == "list_groups" && flag == "0"){
       
        string to_send = ops ,messg;
        S.transmit_msg(to_send);
        messg = S.read_msg();
        cout<<ops<<endl;
    }
    if(ops == "list_files" && flag == "0"){
       string gid,messg;
        cin>>gid;
        string to_send = ops + "/" + gid + "/" + usrName;
        S.transmit_msg(to_send);
        messg = S.read_msg();
        cout<<messg<<endl;
    }
    if(ops == "upload_file" && flag == "0"){
       string filepath, gid;
        cin>>filepath>>gid;
        string to_send = ops +"/" + filepath + "/" + gid + "/"+ usrName;
        S.transmit_msg(to_send);
        messg = S.read_msg();
        if(messg == "0"){
            GidTofiles[gid].push_back(filepath);
        }
        
        cout<<messg<<endl;
    }
    if(ops == "download_file"){
       string usrName, pword;
        cin>>usrName>>pword;
        string to_send = ops + "/" + usrName + "/";
        S.transmit_msg(to_send);
        cout<<ops<<endl;
    }
    if(ops == "logout" && flag == "0"){
        string to_send = ops + "/" + usrName;
        S.transmit_msg(to_send);
        string messg = S.read_msg();
        if(messg == "1")
       {flag = "1";
       lu = 1;}

    }
    if(ops == "show_downloads"){
       string usrName, pword;
        cin>>usrName>>pword;
        string to_send = ops + "/" + usrName + "/"+ pword;
        S.transmit_msg(to_send);
        cout<<ops<<endl;
    }
    if(ops == "stop_share"){
       string usrName, pword;
        cin>>usrName>>pword;
        string to_send = ops + "/" + usrName + "/"+ pword;
        S.transmit_msg(to_send);
        cout<<ops<<endl;
    }
    if(flag != "0"){
        cout<<"[ERROR] : login required!"<<endl;
    }
   
   }

}