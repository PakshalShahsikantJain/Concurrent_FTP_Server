#include "helper.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Helper Function Definations
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Function To print Progress bar
//  
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void print_progress(int current, int total,char * task) {
    int width = 50; // Total width of the bar
    float progress = (float)current / total;
    int filled = (int)(progress * width);
    int i = 0;

    // \r moves cursor to start; [ is the bar start
    printf("\r%s: [",task); 
    
    for (int i = 0; i < filled; i++) 
    {
        printf("#");
    }
    
    for (int i = filled; i < width; i++)
    {
        printf(" ");
    }
    
    printf("] %d%%", (int)(progress * 100));
    
    // Force the terminal to show the output immediately
    fflush(stdout); 
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Create TCP Socket
//  
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

int createTcpSocket()
{
    int iRet = 0;

    iRet = socket(AF_INET,SOCK_STREAM,0);

    return iRet;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Connect To Server
//  
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

int connectToServer(int Sock,char * ip,int Port)
{
    int iRet = 0;
    struct sockaddr_in ServerAddr;

    memset(&ServerAddr,0,sizeof(ServerAddr));

    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_port = htons(Port);

    //Convert the IP Addres into binary format
    inet_pton(AF_INET,ip,&ServerAddr.sin_addr);

    iRet = connect(Sock,(struct sockaddr *)&ServerAddr,sizeof(ServerAddr));

    return iRet;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Send File Name To Server
//  
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

int sendFileNameToServer(int Sock,char * fileName)
{
    int iRet = 0;
    int length = 0;

    length = strlen(fileName);

    write(Sock,&length,sizeof(int));

    iRet = write(Sock,fileName,length);       //Send File Name To Server Which You Want To Read
 
    return iRet;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  
//  Read Header Sent From Server
//  
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int ReadLine(int Sock,char *line,int max)
{
    int i = 0;
    char ch = '\0';
    int n = 0;

    while(i < max - 1)
    {
        n = read(Sock,&ch,1);

        if(n <= 0)
        {
            break;
        }

        line[i++] = ch;

        if(ch == '\n')
        {
            break;
        }
    }//End of While

    line[i] = '\0';

    return i;
} // End of ReadLine

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Function Name: SendFileToServer
//  Parameter: Client Socket FD (int), File Name Which Client Want to Download From Server
//  Description: Sends File To Server as Upload Request
//  Return Value: Nothing
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void SendFileToServer(int ServerSocket,char * FileName)
{
    int fd = 0;
    struct stat sobj;
    char Buffer[1024];
    int BytesRead = 0;
    int sent = 0;
    char task[50] = {'\0'};
    char Header[64] = {'\0'};
    
    printf("Filename is: %s : %ld\n",FileName,strlen(FileName));

    fd = open(FileName,O_RDONLY);               //System Call To Open File In Read Mode Only

    //Unable to Open File
    if(fd < 0)                      
    {
        printf("Unable to open file\n");
        //Send Error Message to Client
        write(ServerSocket,"ERR\n",4);          //System Call to Send Error Message To Client           

        return;
    }

    stat(FileName,&sobj);                       //System Call To Get Statistical Information of File 

    //Header : "OK 1700"
    snprintf(Header,sizeof(Header),"%ld\n",(long)sobj.st_size);          //Library Function To Build Header          

    //Write Header to Client
    printf("Header is: %s\n",Header);
    write(ServerSocket,Header,strlen(Header));          //System Call to Send Header as Success Response To Client

    memset(Buffer,'\0',sizeof(Buffer));

    //Loop To Send actual File Conents
    while((BytesRead = read(fd,Buffer,sizeof(Buffer))) > 0)
    {
        //Send The Data to Client

        if(errno)
        {
            perror("");
            return;
        }
        
        write(ServerSocket,Buffer,BytesRead);           //System Call To Send File Contents to client
        
        if(errno)
        {
            perror("");
            return;
        }
        memset(Buffer,'\0',sizeof(Buffer));
        sent = sent + BytesRead;

        strcpy(task,"Uploading");
        print_progress(sent,sobj.st_size,task);
    }

    if(sent == sobj.st_size)
    {
        printf("\nUpload Complete\n");
    }
    close(fd);              //System Call TO Close File after Reading is Completed
}
