#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <strings.h>
#include <errno.h>
#include <string.h>
#include <sys/select.h>
#include <stdlib.h>

#include "router.h"

#define MAXLINE 90


int main(int argc, char * argv[])
{
    unsigned int myID = atoi(argv[1]);
    char * hostname = argv[2];
    int ne_port = atoi(argv[3]);
    int router_Port = atoi(argv[4]);

    struct pkt_INIT_REQUEST requestPacket;
    requestPacket.router_id = htonl(myID);

    /*-=-=-==-=-==-=-=-=--=-==-=-=-=-=-=-=-==-=-=-==-=-=-==
    *  Set up Socket File Descriptor to bind to
    *  Use local router port number
    * -=-=-==-=-==-=-=-=--=-==-=-=-=-=-=-=-==-=-=-==-=-=-== */

    /* Create a socket descriptor */ 
    int sockfd; 
    struct sockaddr_in routeraddr;  //server and client addresses

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
        return -1; 

    int optval = 1;

    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *) &optval, sizeof(int));

    bzero((char *) &routeraddr, sizeof(routeraddr)); 
    routeraddr.sin_family = AF_INET;  
    routeraddr.sin_addr.s_addr = htonl(INADDR_ANY);  
    routeraddr.sin_port = htons(router_Port);

    if (bind(sockfd, (struct sockaddr *)&routeraddr, sizeof(routeraddr)) == -1)
    {
        perror("Error binding to port");
        return -1;
    } 


    /* -=-=-==-=-==-=-=-=--=-==-=-=-=-=-=-=-==-=-=-==-=-=-==
    *   Build sockaddr_in struct to hold information for network emulator
    *   Use ne port number and IP address of hostname
    * -=-=-==-=-==-=-=-=--=-==-=-=-=-=-=-=-==-=-=-==-=-=-== */
    struct hostent * host_entry;
    int transfered = 0;

    if ((host_entry = gethostbyname(hostname)) == NULL)
    {
        return EXIT_FAILURE;
    }

    // build network emulator address
    struct sockaddr_in ne_addr;
    bzero((char *) &ne_addr, sizeof(ne_addr)); 
    ne_addr.sin_family = AF_INET;  
    //ne_addr.sin_addr.s_addr = (unsigned long)host_entry->h_addr_list[0];  //already in network order
    memcpy((void *)&ne_addr.sin_addr, host_entry->h_addr_list[0], host_entry->h_length);
    ne_addr.sin_port = htons((unsigned short)ne_port);

    //send the Request Packet
    transfered = sendto(sockfd, (struct pkt_INIT_REQUEST *) &requestPacket, sizeof(requestPacket), MSG_CONFIRM, (struct sockaddr *) &ne_addr, sizeof(ne_addr));
    
    printf("\nSent %d bytes\n", transfered);

    // allocate memory for init_response packet
    // struct pkt_INIT_RESPONSE * response = malloc(sizeof(struct pkt_INIT_RESPONSE));
    // struct nbr_cost * temp = malloc(sizeof(struct nbr_cost) * MAX_ROUTERS);
    // memcpy(response->nbrcost, temp, sizeof(struct nbr_cost));
    struct pkt_INIT_RESPONSE response;

    // receive the packet
    transfered = recvfrom(sockfd, (struct pkt_INIT_RESPONSE *) &response, sizeof(struct pkt_INIT_RESPONSE), 0, (struct sockaddr*) &ne_addr, sizeof(ne_addr));
    
    // convert to host order
    ntoh_pkt_INIT_RESPONSE(&response);

    // init local router routing table
    InitRoutingTbl(&response, myID);

    printf("Received %d bytes", transfered);
    printf("\nNum_Neighbors: %d", response.no_nbr);

    // ---- timing threads ----
    // Update neighbors every timeout

    // free(response->nbrcost);
    // free(temp);

    return 0;

}
