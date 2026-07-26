#ifndef HELPER_H
#define HELPER_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/socket.h>
#include<sys/stat.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdbool.h>
#include<dirent.h>
#include<pwd.h>
#include <grp.h>
#include <time.h>
#include<errno.h>

void getFilePermissions(mode_t m,char * perm);
void SendStatOfFile(int ClientSocket,char * fileName);
int ReadLine(int Sock,char *line,int max);
void SendListOfFiles(int ClientSocket);
void SendFileToClient(int ClientSocket,char * FileName);
void ReceiveFileFromClient(int ClientSocket,char * FileName);

#endif