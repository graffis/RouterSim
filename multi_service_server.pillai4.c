#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <strings.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/select.h>
//#define MAXLINE 1024
#define LISTENQ 10
#define MAXLINE 100
#define DAMAX 1024
int open_listenfd(int port)  
{ 
  int listenfd, optval=1; 
  struct sockaddr_in serveraddr; 
   
  /* Create a socket descriptor */ 
  if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    return -1; 
  
  /* Eliminates "Address already in use" error from bind. */ 
  if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,  
		 (const void *)&optval , sizeof(int)) < 0) 
    return -1; 
 
  /* Listenfd will be an endpoint for all requests to port 
     on any IP address for this host */ 
  bzero((char *) &serveraddr, sizeof(serveraddr)); 
  serveraddr.sin_family = AF_INET;  
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);  
  serveraddr.sin_port = htons((unsigned short)port);  
  if (bind(listenfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0) 
    return -1; 
 
  /* Make it a listening socket ready to accept 
     connection requests */ 
  if (listen(listenfd, LISTENQ) < 0) 
    return -1; 
 
  return listenfd; 
} 

void echo(int connfd)  
{ 
  size_t n;  
  char buf[MAXLINE];  
  char dest[100];
  FILE *pf;
  int err;
  char* token;
  char* token2;
  buf[0] = '\0';
  bzero(buf,sizeof(buf));
 // 

 //need to check the request, find the file, reply with cipher text
   //printf("%s.4s\n", buf); 
  // printf("%c\n",&buf[0]);
  // printf("%s",&buf[0]);
   //printf("%d",connfd);
    //fputs(buf,stdout);
  if((n = read(connfd, buf, MAXLINE)) != 0) { //used to be while
   //printf("server received %d bytes\n", n); 
    //write(connfd, buf, n); //replies with the request it got to the client
    //printf("%s\n",&buf[0]);
    //token2 = &buf[3];
   // printf("%s\n",token2);
  } 

    memcpy(dest,buf,sizeof(buf));
    token = strtok(buf," ");
    if(strcmp(token,"GET")==0)
    {
    //printf("%s\n",token);
    int b;
    for(b = 0; b<1;b++)
    {
      
      token = strtok(NULL," ");
      
    }
    errno = 0;
    //char tempo = &token[1];
     //printf("%s\n",tempo);
    //if(token[0]=='/')
    //{token = strtok(NULL,"/");}
    //token2 = strtok(token,"/");
    
   // char goes[MAXLINE];
   // goes[1]= token[0];
   // printf("%s\n",&token[0]);

   //token = strtok(token,"/");
    //printf("%s\n",&token[0]);
    int p = 0;
   // while(dest[p+4]!=" ")
   //dest[5] = 5;
    {
      if(dest[4]==47)
      {dest[3]=46;
      dest[2]=32;}
     // if(dest[3]!=)
    }
    
    
  //  for(p=0;p<MAXLINE;p++)
    {
      //printf("%c",dest[p]);
      //token2[p] = token[p];

    }
    token2 = strtok(dest," ");
    token2 = strtok(NULL," ");
     
   //printf("%s\n",&token2[0]);
    pf = fopen(token2,"r");
    
    if(pf==NULL)
    {
      
      err = errno;
   // printf("%d\n",err);
   
    if(err == 2)
    {//not found
    //printf("%s\n",&buf[0]);
    write(connfd,"HTTP/1.0 404 Not Found\r\n\r\n", strlen("HTTP/1.0 404 Not Found\r\n\r\n"));
    
    }
    else
    {
      // permission denied
      //printf("%s\n",&buf[0]);
      write(connfd,"HTTP/1.0 403 Forbidden\r\n\r\n", strlen("HTTP/1.0 403 Forbidden\r\n\r\n"));
    }
    
    }
  else
  {
    /* code */
  token = strtok(NULL," ");
  int offset;
  offset = atoi(token);
  size_t nread;
  char buf2[DAMAX];
  buf2[0] = '\0';
  bzero(buf2,sizeof(buf2));
  //printf("yes");
  write(connfd,"HTTP/1.0 200 OK\r\n\r\n", strlen("HTTP/1.0 200 OK\r\n\r\n"));
  
  while((nread = fread(buf2,1,sizeof(buf2),pf))!=0)//((nread = fread(buf2,1,sizeof(buf2),pf))!=0)//((n = read(connfd, buf, MAXLINE)) != 0) 
  {  // 

    buf2[nread] = '\0';
    
    int x = 0;
    for(x<0;x<sizeof(buf2);x++)
    {
      if((int)buf2[x]>96 && (int)buf2[x]<123)
      {
        int temp = (((int)buf2[x] )-offset);
        if(temp < 97)
        {temp = 123-(97-temp);}
        //printf("%c\n",buf2[x]);
        buf2[x] = (char) temp;

      }
      if((int)buf2[x]>64 && (int)buf2[x]<91)
      {
        int temp = (((int)buf2[x] )- offset);
       //printf("%c\n",buf2[x]);
        if(temp < 65)
        {temp = 91-(65-temp);}
        buf2[x] = (char) temp;

      }
    }
    
    write(connfd,buf2,strlen(buf2));//sizeof
    bzero(buf2,sizeof(buf2));
    }
  //printf("%s\n",&buf2[0]);  
  fclose(pf);
  
  }
  

    
    //printf("%s\n",token);
    //
    }
   // printf("enddddofile\n");
    
} 
int max(int x, int y)
{
  if(x>y){
    return x;
  }
  
  else
  {
    return y;
  }
  
}

int main(int argc, char **argv) {
  int listenfd,listenfd2, connfd, port, port2,maxfd,clientlen,retval,recudp;
  struct sockaddr_in clientaddr,clientaddr2;
  //struct socketlen_t clientlen2;
  struct hostent *hp;
  fd_set rfds;
  char *haddrp;
  
 // int childpid;
 //signal(SIGCHLD,SIG_IGN);
  port = atoi(argv[1]); /* the server listens on a port passed 
			   on the command line */
  
port2 = atoi(argv[2]); 
listenfd = open_listenfd(port); 
listenfd2 = open_listenfd2(port2);  
 int comp1;
 int comp2;
   while(1)
   {
    //comp1 = 0;
    //comp2 = 0;
    //  printf("BEFOREtcp:%d\n",comp1);
   
  // printf("BEFOREudp:%d\n",comp2);
    FD_ZERO(&rfds);
    FD_SET(listenfd, &rfds);
    FD_SET(listenfd2, &rfds);
    maxfd = max(listenfd,listenfd2) + 1;
    retval = select(maxfd, &rfds, NULL, NULL, NULL);  
    comp1 = FD_ISSET(listenfd, &rfds);
    comp2 = FD_ISSET(listenfd2, &rfds);
 //  printf("tcp:%d\n",comp1);
   
   //printf("udp:%d\n",comp2);
 
    if (comp1){
    int childpid;
    clientlen = sizeof(clientaddr); 
    connfd = accept(listenfd, (struct sockaddr *)&clientaddr, &clientlen);
    
             if( (childpid = fork() )== 0)
                {     

              close(listenfd);
                echo(connfd);
               exit(0);
                   }
            else{

  //  hp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr,
		 //      sizeof(clientaddr.sin_addr.s_addr), AF_INET);
   // haddrp = inet_ntoa(clientaddr.sin_addr);
            signal(SIGCHLD,SIG_IGN);
    //echo(connfd);
   //close(listenfd);
              }
 close(connfd);
    }
   
    else if(comp2) {
     //  printf("tacolibre");
      //printf("Retval: %d; No data within five seconds.\n",retval);
    //usesendtoandrecvfree
socklen_t clientlen2 = sizeof(clientaddr2); //need rechange
//int connection;
//connection = connect()
char buf3[MAXLINE];
char buf4[MAXLINE];
int num2,num3,num4;
char example[MAXLINE] ;//= {'\0'};
recudp = recvfrom(listenfd2,buf3,MAXLINE,0,(struct sockaddr*)&clientaddr2,&clientlen2);
//printf("aaaaa");
uint32_t ends;//[MAXLINE];
memset(example,'\0',MAXLINE);

memcpy(&ends,buf3+(recudp-4),4);

memcpy(example,buf3,(recudp-4));
//printf("%s\n",example);
//ends = ends +1;
getnameinfo((struct sockaddr*)&clientaddr2,(socklen_t)clientlen2,example,MAXLINE,buf4,MAXLINE,0);
//ends = ends +1;
//printf("%s\n",example);
ends = ntohl(ends) +1;
ends= htonl(ends);
//memcpy(buf3,example,2);
/*int t;
int y =0;
for(t = 0;t<MAXLINE;t++)
{
  if(example[t]!='\0')
  {
    y++;
  }
}
*/
//printf("SIZE:%d\n",y);
//printf("array:%c\n",example[recudp-5]);
memcpy(example+strlen(example),&ends,strlen(example)+sizeof(ends));
sendto(listenfd2,example,strlen(example)+sizeof(ends),0,(struct sockaddr*)&clientaddr2,(socklen_t)clientlen2);//change size of maxline, combo of the words and size
 //close(connfd);


    }

    
  }//}
 
 
 /* while (1) {
    int childpid;
    clientlen = sizeof(clientaddr); 
    connfd = accept(listenfd, (struct sockaddr *)&clientaddr, &clientlen);
    
  if( (childpid = fork() )== 0)
  {

    close(listenfd);
   echo(connfd);
    exit(0);
  }
else{

  //  hp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr,
		 //      sizeof(clientaddr.sin_addr.s_addr), AF_INET);
   // haddrp = inet_ntoa(clientaddr.sin_addr);
signal(SIGCHLD,SIG_IGN);
    //echo(connfd);
   
}
 close(connfd);
  }
  */
}

int open_listenfd2(int port)  
{ 
  int listenfd, optval=1; 
  struct sockaddr_in serveraddr; 
   
  /* Create a socket descriptor */ 
  if ((listenfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
    return -1; 
  
  /* Eliminates "Address already in use" error from bind. */ 
  if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,  
		 (const void *)&optval , sizeof(int)) < 0) 
    return -1; 
 
  bzero((char *) &serveraddr, sizeof(serveraddr)); 
  serveraddr.sin_family = AF_INET;  
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);  
  serveraddr.sin_port = htons((unsigned short)port);  
  if (bind(listenfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0) 
    return -1; 
 
  /* Make it a listening socket ready to accept 
     connection requests 
  if (listen(listenfd, LISTENQ) < 0) 
    return -1; 
 */ 
  return listenfd; 
} 