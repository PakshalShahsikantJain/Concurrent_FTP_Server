#include "header.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Helper Function Definations
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void getFilePermissions(mode_t m,char * perm) 
{

    if(S_ISDIR(m)) perm[0] = 'd';
    if(S_ISLNK(m)) perm[0] = 'l';

    if(m & S_IRUSR) perm[1] = 'r';
    if(m & S_IWUSR) perm[2] = 'w';
    if(m & S_IXUSR) perm[3] = 'x';

    if(m & S_IRGRP) perm[4] = 'r';
    if(m & S_IWGRP) perm[5] = 'w';
    if(m & S_IXGRP) perm[6] = 'x';

    if(m & S_IROTH) perm[7] = 'r';
    if(m & S_IWOTH) perm[8] = 'w';
    if(m & S_IXOTH) perm[9] = 'x';

    // perm[11] = '\0';
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Function Name: SendStatOfFile
//  Parameter: Client Socket FD (int)
//  Description: Sends Statistical Information of File Present on Server To Client
//  Return Value: Nothing
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void SendStatOfFile(int ClientSocket,char * fileName)
{
    int iRet = 0;
    char perm[11] = "----------";
    char fileType[30] = {'\0'};
    struct stat sobj;
    struct passwd *pwd;
    struct group *grp;

    iRet = stat(fileName,&sobj);

    char Buffer[310] = {'\0'};

    if(iRet == -1)
    {
        write(ClientSocket,"Unable To Get Statistical Information of File",strlen("Unable To Get Statistical Information of File"));
        return;
    }

    switch((sobj.st_mode & S_IFMT)) 
    {
        case S_IFREG : 
            strcpy(fileType,"regular file");
            break;
        case S_IFSOCK:
            strcpy(fileType,"socket");
            break;
        case S_IFLNK:
            strcpy(fileType,"symbolic link");
            break;
        case S_IFBLK:
            strcpy(fileType,"block device");
            break;
        case S_IFDIR:
            strcpy(fileType,"directory");
            break;
        case S_IFCHR:
            strcpy(fileType,"character device");
            break;
        case S_IFIFO:
            strcpy(fileType,"FIFO");
            break;
        default :
            strcpy(fileType,"Unknown File");
            break;
    }

    
    getFilePermissions(sobj.st_mode,perm);

    perm[10] = '\0';

    pwd = getpwuid(sobj.st_uid);

    grp = getgrgid(sobj.st_gid);

    snprintf(
        Buffer,sizeof(Buffer),
        "File Name: %s\nSize: %ld           Blocks: %ld         IO Block: %ld     %s\nDevice: %ld	    Inode: %ld     Links: %ld\nAccess:(%3jo/%s)  UID:(%ju/%s)   GID:(%ju/%s)\nAccess: %sModify: %sChange: %s\n"
        ,fileName,sobj.st_size,sobj.st_blocks,sobj.st_blksize,fileType,sobj.st_dev,sobj.st_ino,sobj.st_nlink,(uint64_t) sobj.st_mode,perm,
        (uint64_t) sobj.st_uid,pwd->pw_name,(uint64_t) sobj.st_gid,grp->gr_name,ctime(&sobj.st_atime),ctime(&sobj.st_mtime),ctime(&sobj.st_ctime)
    );

    write(ClientSocket,Buffer,strlen(Buffer) + 1);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  
//  Read Header Sent From Client 
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
//  Function Name: SendListOfFile
//  Parameter: Client Socket FD (int)
//  Description: Sends List of All Files Present on Server To Client
//  Return Value: Nothing
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SendListOfFiles(int ClientSocket)
{
    DIR * dp = NULL;
    struct dirent *ptr = NULL;

    dp = opendir(".");              //System Call To open Server Directory File

    if(dp == NULL)
    {
        perror("Error Occured while reading directory ");
        printf("\n");
        write(ClientSocket,"Internal Server Error",strlen("Internal Server Error"));

        close(ClientSocket);
    }

    //Loop To Read Directory and File Names Present in Directory 
    while((ptr = readdir(dp)) != NULL)
    {
        if((strcmp(".",ptr->d_name) != 0) && (strcmp("..",ptr->d_name) != 0))
        {            
            write(ClientSocket,strcat(ptr->d_name,"\n"),strlen(ptr->d_name) + 1);           //System Call To Send File Name To Client
        }        
    }

    closedir(dp);           //System Call To Close Directory File
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Function Name: SendFileToClient
//  Parameter: Client Socket FD (int), File Name Which Client Want to Download From Server
//  Description: Sends File To Client as a Response of Download Request
//  Return Value: Nothing
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void SendFileToClient(int ClientSocket,char * FileName)
{
    int fd = 0;
    struct stat sobj;
    char Buffer[1024];
    int BytesRead = 0;
    char Header[64] = {'\0'};
    
    printf("Filename is: %s : %ld\n",FileName,strlen(FileName));

    fd = open(FileName,O_RDONLY);               //System Call To Open File In Read Mode Only

    //Unable to Open File
    if(fd < 0)                      
    {
        printf("Unable to open file\n");
        //Send Error Message to Client
        write(ClientSocket,"ERR\n",4);          //System Call to Send Error Message To Client           

        return;
    }

    stat(FileName,&sobj);                       //System Call To Get Statistical Information of File 

    //Header : "OK 1700"
    snprintf(Header,sizeof(Header),"OK %ld\n",(long)sobj.st_size);          //Library Function To Build Header          

    //Write Header to Client
    write(ClientSocket,Header,strlen(Header));          //System Call to Send Header as Success Response To Client

    memset(Buffer,'\0',sizeof(Buffer));

    //Loop To Send actual File Conents
    while((BytesRead = read(fd,Buffer,sizeof(Buffer))) != 0)
    {
        //Send The Data to Client

        write(ClientSocket,Buffer,BytesRead);           //System Call To Send File Contents to client
        
        memset(Buffer,'\0',sizeof(Buffer));
    }

    close(fd);              //System Call TO Close File after Reading is Completed
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Function Name: ReceiveFileFromClient
//  Parameter: Client Socket FD (int), File Name Which Client Want to Download From Server
//  Description: Receive File From Client as a Response of Upload Request
//  Return Value: Nothing
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ReceiveFileFromClient(int ClientSocket,char * FileName)
{
    int fd = 0;
    struct stat sobj;
    char Buffer[1024];
    int BytesRead = 0;
    int fileSize = 0;
    int received = 0;
    int remaining = 0;
    int toRead = 0;
    int n = 0;
    int iRet = 0;
    char Header[64] = {'\0'};
    
    printf("Filename is: %s : %ld\n",FileName,strlen(FileName));

    fd = open(FileName,O_CREAT | O_WRONLY | O_TRUNC,0777);               //System Call To Open File In Read Mode Only

    //Unable to Open File
    if(fd < 0)                      
    {
        printf("Unable to open file\n");
        //Send Error Message to Client
        perror("");
        return;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //  Step 4: Read the header 
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    iRet = ReadLine(ClientSocket,Header,sizeof(Header));

    if(iRet <= 0)
    {
        printf("Server Gets Disconnected abnormally\n");
        close(ClientSocket);
        return;
    }

    printf("Header is: %s",Header);

    if(errno)
    {
        perror("");
        return;

    }
    fileSize = atoi(Header);

    printf("File Size is: %d",fileSize);

    memset(Buffer,'\0',sizeof(Buffer));

    //Loop To Read File Contents From Server and Write it into Output File
    while(received < fileSize)
    {
        remaining = fileSize - received;

        if(remaining > 1024)
        {
            toRead = 1024;
        }
        else 
        {
            toRead = remaining;
        }

        n = read(ClientSocket,Buffer,toRead);

        if(n < 0) 
        {
            perror("Error Occured: ");
            
            return;
        }

        write(fd,Buffer,n);

        memset(Buffer,'\0',sizeof(Buffer));

        received = received + n;
    } // End of While
 
    write(ClientSocket,"1",1);

    close(fd);  //System Call To Close File
    close(ClientSocket);   //System Call To Close Server Socket

    if(received == fileSize)
    {
        printf("\nDownload Complete\n");
        return;
    }
    else 
    {
        printf("Download Failed\n");
        return;
    }

    printf("\n");
    
    close(fd);              //System Call TO Close File after Reading is Completed
}
