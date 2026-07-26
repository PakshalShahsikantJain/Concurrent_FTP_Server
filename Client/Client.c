////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Author: Pakshal Shashikant Jain 
//  Date: 03/03/2026
//  Project: Concurrent FTP Server
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Client Application 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Required Headers 
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include"helper.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Commandline Argument Application
//
//  1. To List All Files Present on Sever  (-ls):
//     
//      1st Argument:   IP Address
//      2nd Argument:   Port Number 
//      3rd Argument:   -ls
//      
//      ./client    127.0.0.0.1    9000        -ls
//      argv[0]     argv[1]        argv[2]     argv[3]
//
//      argc = 4
//
//  2. To Read File Contents Present on Server and Display it on Console (-cat):
//      
//      1st Argument:   IP Address
//      2nd Argument:   Port Number
//      3rd Argument:   -cat
//      4th Argument:   Target File Name
// 
//      ./client    127.0.0.1   9000        -cat        A.txt
//      argv[0]     argv[1]     argv[2]     argv[3]     argv[4]
//
//      argc = 5
//
//  3. To Read Files Statistical Information of file Present on Server and Display it on Console (-stat):
//      
//      1st Argument:   IP Address
//      2nd Argument:   Port Number
//      3rd Argument:   -stat
//      4th Argument:   Target File Name
// 
//      ./client    127.0.0.1   9000        -stat        A.txt
//      argv[0]     argv[1]     argv[2]     argv[3]     argv[4]
//
//      argc = 5
//
//  4. To Download File From Server
// 
//      1st Argument:   IP Address
//      2nd Argument:   Port Number
//      3rd Argument:   Target File Name
//      4th Argument:   New File Name
//      ./client    127.0.0.1   9000        Demo.txt    A.txt
//      argv[0]     argv[1]     argv[2]     argv[3]     argv[4]
//
//      argc = 5
//
//  5. To Upload File On Server
// 
//      1st Argument:   IP Address
//      2nd Argument:   Port Number
//      3rd Argument:   -upload
//      4th Argument:   File Name
//      ./client    127.0.0.1   9000        -upload    A.txt
//      argv[0]     argv[1]     argv[2]     argv[3]     argv[4]
//
//      argc = 5
//  
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Entry Point Function 
int main(int argc,char *argv[])
{
    int Sock = 0;
    int Port = 0;       //argv[2]
    int iRet = 0;

    char *ip = NULL;        //argv[1]
    char *FileName = NULL;  //argv[3]
    char *fileNameToUpload = NULL;
    char *OutFileName = NULL; //argv[4]
    char Buffer[1024] = {'\0'};
    char d_name[256] = {'\0'};
    char Stat[300] = {'\0'};
    char task[50] = {'\0'};
    long received = 0;
    long remaining = 0;
    int n = 0;  
    int toRead = 0;
    int outfd = 0;
    long FileSize = 0;
    int numberOfFiles = 0;
    char * command = NULL;
    int length = 0;
    char ack[50] = {'\0'};

    char Header[64] = {'\0'};

    if(argc < 2 || argc > 5)
    {
        printf("Unable to Proceed as invalid number of arguments\n");
        printf("For Usage Type: -u or --usage\n");
        printf("For Help Type: -h or --help\n");

        return -1;
    }

    if(argc == 2)
    {
        if((strcmp(argv[1],"-u") == 0)||(strcmp(argv[1],"--usage") == 0))
        {
            printf("To list all Files Pass: \n");
            printf("1st Argument  : IP Address\n");
            printf("2nd Argument  : Port Number\n");
            printf("3rd Arguement : -ls\n");
            printf("\n");
            
            printf("To Display File Data Pass: \n");
            printf("1st Argument  : IP Address\n");
            printf("2nd Argument  : Port Number\n");
            printf("3rd Arguement : -cat\n");
            printf("4th Argument  : Target File Name\n");
            printf("\n");
            
            printf("To Display Statistical Information of File Pass: \n");
            printf("1st Argument  : IP Address\n");
            printf("2nd Argument  : Port Number\n");
            printf("3rd Arguement : -stat\n");
            printf("4th Argument  : Target File Name\n");
            printf("\n");
            
            printf("To Download File Pass: \n");
            printf("1st Argument  : IP Address\n");
            printf("2nd Argument  : Port Number\n");
            printf("3rd Arguement : Target File Name\n");
            printf("4th Argument  : New File Name\n");
            printf("\n");

            printf("To Upload File Pass: \n");
            printf("1st Argument  : IP Address\n");
            printf("2nd Argument  : Port Number\n");
            printf("3rd Arguement : -upload\n");
            printf("4th Argument  : Target File Name\n");

            return 0;
        }
        else if((strcmp(argv[1],"-h") == 0)||(strcmp(argv[1],"--help") == 0))
        {
            //Options
            printf("Options:\n");
            printf("-ls        : List All Files Present on Server\n");
            printf("-cat       : Concatenate files and print data on the standard output\n");
            printf("-stat      : Fetch Statistical information of file and print data on the standard output\n");
            printf("-upload    : To Upload File on Server\n");
            printf("-u --usage : Display instruction on how to use this application\n");
            printf("-h --help  : Display help / instructions\n");

            //Arguments
            printf("Arguments:\n");
            printf("IP Address       : IP Address of Server where server is Hosted and Running e.g localhost(127.0.0.0.1)\n");
            printf("Port Number      : Port Number from where Server will receive/send data from/to Client\n");
            printf("Target File Name : Name of file which you have to download/upload data from/on Server\n");
            printf("New File Name    : Name of file you want to download file as\n");

            return 0;
        }
        else 
        {
            printf("Unable to Proceed as invalid argument passed\n");
            printf("For Usage Type: -u or --usage\n");
            printf("For Help Type: -h or --help\n");

            return -1;
        }
    }
    else if(argc == 4)                              // ./Client 127.0.0.1 9000 -ls
    {
        command = argv[3];
    }
    else 
    {
        
        if(strcmp(argv[3],"-cat") == 0)             // ./Client 127.0.0.1 9000 -cat [Target File Name]
        {
            command = argv[3];
            FileName = argv[4];

            printf("Command: %s\n",command);
            printf("File Name: %s\n",FileName);
        }
        else if(strcmp(argv[3],"-stat") == 0)             // ./Client 127.0.0.1 9000 -stat [Target File Name]
        {
            command = argv[3];
            FileName = argv[4];
        } 
        else if(strcmp(argv[3],"-upload") == 0)             // ./Client 127.0.0.1 9000 -upload [Target File Name]
        {
            command = argv[3];
            FileName = argv[4];
        } 
        else                                        // ./Client 127.0.0.1 9000 [File Name To Download]  [File Name To Save Downloaded FIle Into] 
        {
            FileName = argv[3];
            OutFileName = argv[4];
        }
    }

    //Store Command Line Argument into the variables
    ip = argv[1];
    Port = atoi(argv[2]);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //  Step 1: Create TCP Socket 
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Sock = createTcpSocket();

    if(Sock < 0)
    {
        printf("Unable to crete the client socket\n");

        return -1;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //  Step 2: Connect with Server 
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    iRet = connectToServer(Sock,ip,Port);

    if(iRet == -1)
    {
        printf("Unable to connect with Server\n");
        close(Sock);
        return -1;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 
    // Commands Handling Part
    //
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    if(command != NULL)         //Condition To Handle Command -ls / -cat / -stat / -upload
    {
        // printf("Command Entered: %s\n",command);
        
        if(strcmp(command,"-ls") == 0)      //Condition To Handle -ls Command
        {
            length = strlen(command);
            write(Sock,&length,sizeof(int));

            write(Sock,command,length);
            
            //Loop To Read List of File Sent From Server and Display it on Console
            while((iRet = read(Sock,d_name,sizeof(d_name))) != 0)
            {
                printf("%s",d_name);
                memset(d_name,'\0',sizeof(d_name));
            }

            close(Sock);        //Close Server Socket After Reading List of Files

            return 0;
        }
        else if(strcmp(command,"-cat") == 0)        //Condition To Handle -cat command 
        {
            printf("%s\n",command);
            length = strlen(command);

            write(Sock,&length,sizeof(int));   
            iRet = write(Sock,command,length);       //Send Command To Server Which You Want To Read

            if(iRet <= 0)
            {
                printf("Server Terminated abnormally\n");
                return -1;
            }

            length = strlen(FileName);
            write(Sock,&length,sizeof(int));

            iRet = write(Sock,FileName,length);       //Send File Name To Server Which You Want To Read

            if(iRet <= 0)
            {
                printf("Server Terminated abnormally\n");
                return -1;
            }
            else 
            {
                printf("%d\n",iRet);
            }

            memset(Buffer,'\0',sizeof(Buffer));
    
            //Loop To Read File Contents Sent From Server and Display it Consile
            while((iRet = read(Sock,Buffer,sizeof(Buffer))) != 0)
            {
                printf("%s",Buffer);
                memset(Buffer,'\0',sizeof(Buffer));
            }
            printf("\n");

            close(Sock);        //System Call To Close Client Socket

            return 0;
        }
        else if(strcmp(command,"-stat") == 0)        //Condition To Handle -sat command 
        {
            length = strlen(command);

            write(Sock,&length,sizeof(int));   
            iRet = write(Sock,command,length);       //Send Command To Server Which You Want To Read

            if(iRet <= 0)
            {
                printf("Server Terminated abnormally\n");
                return -1;
            }
            
            // printf("Command %s is Send Successfully To ServerW",command);
            
            // printf("File Name To Stat %s\n",FileName);


            length = strlen(FileName);

            write(Sock,&length,sizeof(int));            
            iRet = write(Sock,FileName,strlen(FileName));       //Send File Name To Server Which You Want To Read

            if(iRet <= 0)
            {
                printf("Server Terminated abnormally\n");
                return -1;
            }
            // else 
            // {
            //     printf("%d\n",iRet);
            // }

            memset(Stat,'\0',sizeof(Stat));
    
            //Loop To Read Statistics of Fil Sent From Server and Display it Console
            while((iRet = read(Sock,Stat,sizeof(Stat))) != 0)
            {
                printf("%s",Stat);
                memset(Stat,'\0',sizeof(Stat));
            }

            printf("\n");
            close(Sock);        //System Call To Close Client Socket

            return 0;
        }
        else if(strcmp(command,"-upload") == 0)        //Condition To Handle -upload command 
        {
            length = strlen(command);

            write(Sock,&length,sizeof(int));   
            iRet = write(Sock,command,length);       //Send Command To Server

            if(iRet <= 0)
            {
                printf("Server Terminated abnormally\n");
                return -1;
            }
            
            // printf("Command %s is Send Successfully To ServerW",command);
            
            // printf("File Name To Stat %s\n",FileName);

            fileNameToUpload = basename(FileName);

            length = strlen(fileNameToUpload);
            
            write(Sock,&length,sizeof(int));        

            iRet = write(Sock,fileNameToUpload,length);       //Send File Name To Server Which You Want To Upload

            if(iRet <= 0)
            {
                printf("Server Terminated abnormally\n");
                return -1;
            }

            SendFileToServer(Sock,FileName);        //Call To user define Function

            iRet = read(Sock,ack,1);
            
            printf("Value of iRet: %d\n",iRet);
    
            if(iRet < -1)
            {
                perror("Error Occured: ");
                
                return -1;
            }

            if(iRet > 0)
            {
                printf("File Transfer Done & Client Disconnected\n");

                close(Sock);            //Close Client Socket Once Request is Completed                    
            }
            else 
            {
                close(Sock);            //Close Client Socket Once Request is Completed
            }
            return 0;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // Logic To Download File From Server
    //
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //  Step 3: Send File Name 
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    printf("Target File Name is: %s\n",FileName);
    printf("Output File Name is:%s\n",OutFileName);

    iRet = sendFileNameToServer(Sock,FileName);

    if(iRet <= 0)
    {
        perror("Unable to Send File Name To Server: ");
        printf("\n");
        return -1;
    }
    else 
    {
        write(Sock,"\n",1);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //  Step 4: Read the header 
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    iRet = ReadLine(Sock,Header,sizeof(Header));

    if(iRet <= 0)
    {
        printf("Server Gets Disconnected abnormally\n");
        close(Sock);
        return -1;
    }

    printf("Header is: %s",Header);
    sscanf(Header,"OK %ld",&FileSize);
    // printf("Header is: %s",Header);
    printf("File Size is: %ld\n",FileSize);
    

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //  Step 5: Creat new File 
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //System Call To Open File in Write and Read Mode also Create File If Not Present 
    outfd = open(OutFileName,O_CREAT | O_WRONLY | O_TRUNC,0777);

    if(outfd < 0)
    {
        printf("Unable to create downloaded File:\n");
        return -1;
    }

    memset(Buffer,'\0',sizeof(Buffer));

    strcpy(task,"Downloading");
    //Loop To Read File Contents From Server and Write it into Output File
    while(received < FileSize)
    {
        remaining = FileSize - received;

        if(remaining > 1024)
        {
            toRead = 1024;
        }
        else 
        {
            toRead = remaining;
        }

        n = read(Sock,Buffer,toRead);

        if(n < 0) 
        {
            perror("Error Occured: ");
            
            return -1;
        }
        
        write(outfd,Buffer,n);

        memset(Buffer,'\0',sizeof(Buffer));

        received = received + n;
        print_progress(received,FileSize,task);

    } // End of While
 
    printf("\n");

    write(Sock,"1",1);

    close(outfd);  //System Call To Close File
    close(Sock);   //System Call To Close Server Socket

    if(received == FileSize)
    {
        printf("Download Complete\n");
        return 0;
    }
    else 
    {
        printf("Download Failed\n");
        return -1;
    }

    return 0;
}  // End of main