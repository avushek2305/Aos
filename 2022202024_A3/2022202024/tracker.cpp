#include<iostream>
#include <bits/stdc++.h>
#include<fstream>
#include<stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

using namespace std;

struct file_data
{
	string clientid;
    string filepath;
	string hash;
	string ip;
	string port;
	string gid;
		
};
fstream file;

    int PORT;
    
    int general_socket_descriptor;
    

    struct sockaddr_in address;
    int address_length;
    char buffer[1024] = { 0 };
    string filename = "../Data/2.png";
    string msg;
    map<string,string> Authenticate;
    map<string,string> GidToAdmin;
    map<string,vector<string>> GidToUsers;
    map<string,vector<string>> adminTOReq;
    map<string,string> isLogin; 
    map<string,vector<file_data>> GidToFile;
    int UserId = 0;

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
            
            cout<<"[LOG] : Connected to Client.\n";
            return new_socket_descriptor;
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

        bool findin(vector<string> &arr, string x){
            for(auto i: arr){
                if(i==x)
                return true;
            }
            return false;
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
        pair<string,int> read_msg(int new_socket_descriptor){
            char buffer[1024] = { 0 };
            int valread = read(new_socket_descriptor, buffer, 1024);
            return {buffer,valread};
           
        }
        vector<string> splitstring(string str,char delim)
            {
                std::vector<string> vec;
                int i=0;
                string s="";
                while(i<str.length())
                {
                    if(str[i]==delim)
                    {
                        vec.push_back(s);
                        s="";
                        i++;
                    }
                    else
                    {
                        s=s+str[i];
                        i++;
                    }
                }
                vec.push_back(s);
                return vec;
            }

        void *threads_code(void* n_s_d){
            int new_socket_descriptor = *(int *)n_s_d;
            while(1)
             {  
                vector<string> task;
                pair<string,int>  val = read_msg(new_socket_descriptor);
                string message = val.first;
                if(val.second <= 0 )
                {break;}
                task = splitstring(message,'/');
                
                cout<<message;
                if(task[0] == "create_user" ){
                    if(task.size() != 3)
                    setMsg("3");
                    else{
                        
                        if(Authenticate.find(task[1]) == Authenticate.end())
                        {Authenticate[task[1]] = task[2];

                        setMsg("0");
                        }
                        else
                        {setMsg("1");}
                    }

                }
                else if(task[0] == "login" ){
                    if(task.size() != 4   )
                    setMsg("3");
                    else if(Authenticate.find(task[1]) == Authenticate.end() ){
                        
                        setMsg("2");
                    }
                    else if(Authenticate[task[1]] == task[2] && isLogin.find(task[1]) == isLogin.end()){
                        isLogin[task[1]] = task[3];
                        setMsg("0");
                    }
                    else{
                        setMsg("1");
                    }

                }
                else if(task[0] == "create_group"){
                    if(task.size() != 3)
                    setMsg("3");
                    else if(GidToAdmin.find(task[1]) == GidToAdmin.end()){
                            GidToAdmin[task[1]] = task[2];
                            GidToUsers[task[1]].push_back(task[2]);
                            adminTOReq.insert({task[1],{}});
                            setMsg("0");
                    }
                    else{
                        setMsg("1");
                    } 

                }

                else if(task[0] == "join_group"){
                    if(task.size() != 3)
                    setMsg("3");
                    else if(GidToAdmin.find(task[1]) != GidToAdmin.end() && task[2] != GidToAdmin[task[1]] && findin(adminTOReq[task[1]], task[2]) == false ){
                            adminTOReq[task[1]].push_back(task[2]);
                            setMsg("0");
                    }
                    else{
                        setMsg("1");
                    }

                }

                else if(task[0] == "leave_group"){
                    if(task.size() != 3)
                    setMsg("3");
                    else if(GidToAdmin.find(task[1]) != GidToAdmin.end()){
                            if(GidToAdmin[task[1]] == task[2]){
                                setMsg("4"); // admin assign
                                }
                                
                            
                            else if(findin(GidToUsers[task[1]],task[2]) != 0 )
                           { remove(GidToUsers[task[1]].begin(),GidToUsers[task[1]].end(),task[2]);
                           GidToUsers[task[1]].pop_back();
                            setMsg("0");}
                            else{
                                 setMsg("5");
                            }
                    }
                    else{
                        setMsg("1");
                    }
                }

                else if(task[0]=="accept_request"){
                    if(task.size() != 4)
                    setMsg("3");
                    else if(GidToAdmin.find(task[1]) != GidToAdmin.end() && GidToAdmin[task[1]] == task[3]){
                            if(findin(adminTOReq[task[1]], task[2])){
                                remove(adminTOReq[task[1]].begin(),adminTOReq[task[1]].end(),task[2]); 
                                adminTOReq[task[1]].pop_back();
                                GidToUsers[task[1]].push_back(task[2]);
                            setMsg("0");
                            }
                            else{
                                setMsg("2");
                            }
                            


                    }
                    else
                    {setMsg("1");}
                }
                else if(task[0] == "list_requests"){
                    if(task.size() != 2){
                        setMsg("3");
                    }
                    else if(GidToAdmin.find(task[1]) != GidToAdmin.end()){
                        cout<<"size :"<<adminTOReq[task[1]].size();
                        for(auto i: adminTOReq[task[1]]){
                            cout<<i<<" ";
                        
                        }
                        cout<<endl;
                        setMsg("0");
                    }
                }
                else if(task[0] == "list_groups"){
                    for(auto i : GidToAdmin){
                        cout<<i.first<<" ";
                    }
                    cout<<endl;
                        setMsg("0");
                }
                else if(task[0] == "logout"){
                    
                                isLogin.erase(task[1]);
                                setMsg("1");
                }

                else if(task[0] == "upload_file"){
                    if(isLogin.find(task[3]) != isLogin.end()){
                        if(findin(GidToUsers[task[2]],task[3]) != false){
                            file_data f_data;
                            f_data.filepath = task[1];
                            f_data.clientid = task[3];
                            f_data.gid =task[2];
                            f_data.port = isLogin[task[3]]; 
                            GidToFile[task[2]].push_back(f_data); 
                            setMsg("0");
                        }
                        else{
                            setMsg("2");
                        }
                    }
                    else{
                        setMsg("1");
                    }
                }
                else if(task[0] == "list_files"){
                    if(findin(GidToUsers[task[1]],task[3]) != false){
                            vector<file_data> temp= GidToFile[task[1]];
                            for(file_data i: temp){
                                cout<<i.filepath<<" ";
                            }
                            setMsg("0");
                        }
                        else{
                            setMsg("2");
                        }


                }
                else{
                    setMsg("Bakwass Hai Ye");
                }

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
                
                

                if( pthread_create( &c ,0 ,  threads_code , (void*)&new_socket_descriptor) < 0)
                {
                    perror("could not create thread");
                    
                }
            
            }


}






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
        // return 1;
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