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
#include<errno.h>
#include<libgen.h>

// int Add(int x, int y);

void print_progress(int current, int total,char * task);    //  Function To print Progress bar
int createTcpSocket();                                      //  Create TCP Socket
int connectToServer(int Sock,char * ip,int Port);           //  Connect To Server
int sendFileNameToServer(int Sock,char * fileName);         //  Send File Name To Server    
int ReadLine(int Sock,char *line,int max);                  //  Read Header Sent From Server
void SendFileToServer(int ServerSocket,char * FileName);     //  Sends File To Server as Upload Request

#endif