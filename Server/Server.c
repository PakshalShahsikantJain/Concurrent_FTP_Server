////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Author: Pakshal Shashikant Jain 
//  Date: 03/03/2026
//  Project: Concurrent FTP Server
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Server Application 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Required Headers 
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "helper.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Commandline Argument Application
//  1st Argument: Port Number
//  ./server    9000
//  argv[0]     argv[1]
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc,char *argv[])
{
    int ServerSocket = 0;
    int ClientSocket = 0;
    int Port = 0;
    int iRet = 0;
    char command[20] = {'\0'};
    char data[50] = {'\0'};
    char FileName[50] = {'\0'};
    char ack[20] = {'\0'};
    int len = 0;
    pid_t pid = 0;

    struct sockaddr_in ServerAddr;
    struct sockaddr_in ClientAddr;

    socklen_t AddrLen = sizeof(ClientAddr);

    if((argc < 2) || (argc > 2))
    {
        printf("Unable to Proceed as Invalid Number of arguments\n");
        printf("Please Provide The Port Number\n");
        return -1;
    }   

    //Port Number of Server
    Port = atoi(argv[1]);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //  Step 1: Create TCP Socket
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ServerSocket = socket(AF_INET,SOCK_STREAM,0);

    if(ServerSocket < 0)
    {
        printf("Unable to Create Server Socket\n");
        return -1;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //  Step 2: Bind Socket To IP and Port
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    memset(&ServerAddr,0,sizeof(ServerAddr));

    //Initialize the Structure

    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_addr.s_addr = INADDR_ANY;
    ServerAddr.sin_port = htons(Port);

    iRet = bind(ServerSocket,(struct sockaddr *)&ServerAddr,sizeof(ServerAddr));

    if(iRet == -1)
    {
        printf("Unable to bind\n");
        close(ServerSocket);
        return -1;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //  Step 3: listen  for client connections
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    iRet = listen(ServerSocket,11);

    if(iRet == -1)
    {
        printf("Server unable to listen\n");
        close(ServerSocket);
        return -1;
    }

    printf("Server is running on port: %d\n",Port);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //  Loop which accepts client request continously
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //Loop to accept Multiple Cleint requests
    while(1)
    {
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //  Step 4: Accept the Client request
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        memset(&ClientAddr,0,sizeof(ClientAddr));

        printf("Server is Waiting for Client Request\n");
        
        ClientSocket = accept(ServerSocket,(struct sockaddr *)&ClientAddr,&AddrLen);

        if(ClientSocket < 0)
        {
            printf("Unable to accept client Request\n");

            continue;   // Used for while 
        }

        printf("Client Gets Connected: %s\n",inet_ntoa(ClientAddr.sin_addr));

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //  Step 5: Create New Process to handle Client Request
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        pid = fork();

        if(pid < 0)
        {
            printf("Unable to create a new process for client request\n");
            close(ClientSocket);

            continue;
        }

        //New Process Gets Created for client
        if(pid == 0)
        {
            printf("Client New Process is created for client request\n");

            close(ServerSocket);

            read(ClientSocket,&len,sizeof(int));
            printf("Size of data: %d\n",len);
            iRet = read(ClientSocket,data,len); 

            printf("%s\n",data);
            
            // printf("Command Entered By Client: %s\n",data);
            if(strcmp(data,"-ls") == 0)     //Condition To Handle Coomand -ls entered By Client
            {
                printf("Command Entered: %s\n",data);
                SendListOfFiles(ClientSocket);              //Call To User Defined Function Which Sends List of File To Client

                close(ClientSocket);
                printf("List of Files Sent Successfully\n");
                exit(0);
            }
            else if(strcmp(data,"-stat") == 0)     //Condition To Handle Coomand -stat entered By Client
            {
                printf("Command Entered: %s\n",data);
    
                read(ClientSocket,&len,sizeof(int));
                iRet = read(ClientSocket,FileName,len);

                FileName[strcspn(FileName,"\r\n")] = '\0';
                printf("File Name Send with Stat Command is: %s\n",FileName);
                
                SendStatOfFile(ClientSocket,FileName);              //Call To User Defined Function Which Sends List of File To Client

                close(ClientSocket);
                printf("Statistics of Files Sent Successfully\n");
                exit(0);
            }
            else if(strcmp(data,"-cat") == 0)     //Condition To Handle Coomand -cat entered By Client
            {
                printf("Command Entered: %s\n",data);
    
                read(ClientSocket,&len,sizeof(int));
                iRet = read(ClientSocket,FileName,len);

                FileName[strcspn(FileName,"\r\n")] = '\0';
                printf("File Name Send with Cat Command is: %s\n",FileName);
                
                SendFileToClient(ClientSocket,FileName);              //Call To User Defined Function Which Sends Data of File To Client

                close(ClientSocket);
                printf("Data of File Sent Successfully\n");
                exit(0);
            }
            else if(strcmp(data,"-upload") == 0)            //Condition to handle Command -upload entered by client
            {
                printf("Command Entered: %s\n",data);
    
                read(ClientSocket,&len,sizeof(int));
                iRet = read(ClientSocket,FileName,len);

                FileName[strcspn(FileName,"\r\n")] = '\0';
                printf("File Name Send with upload Command is: %s\n",FileName);
                
                ReceiveFileFromClient(ClientSocket,FileName);              //Call To User Defined Function Which Receuveds Data of File From Client

                write(ClientSocket,"1",1);

                close(ClientSocket);

                printf("File Received Successfully\n");
                exit(0);
            }
            else 
            {
                strcpy(FileName,data);              
                
                printf("Requested File By Client: %s\n",FileName);

                FileName[strcspn(FileName,"\r\n")] = '\0';

                SendFileToClient(ClientSocket,FileName);       //Call To Handle -cat Command or To Send File Content To User when user request to download file

                iRet = read(ClientSocket,ack,1);
                
                printf("Value of iRet: %d\n",iRet);
        
                if(iRet < -1)
                {
                    perror("Error Occured: ");
                    
                    return -1;
                }

                if(iRet > 0)
                {
                    printf("File Transfer Done & Client Disconnected\n");

                    close(ClientSocket);            //Close Client Socket Once Request is Completed                    
                    exit(0);  //Kill the Child process
                }
                else 
                {
                    close(ClientSocket);            //Close Client Socket Once Request is Completed
                    exit(0);  //Kill the Child process
                }
            }
        }   //End of if (fork)
        else    //Parent Process    (Server) 
        {
            close(ClientSocket);
        }   //End of else

    }   //End of While


    close(ServerSocket);  //Close Server Socket

    return 0;
}  // End of main