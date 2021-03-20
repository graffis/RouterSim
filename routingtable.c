#include "ne.h"
#include "router.h"


/* ----- GLOBAL VARIABLES ----- */
struct route_entry routingTable[MAX_ROUTERS];
int NumRoutes = 0;

int checkNewRoutersInRoutes(struct pkt_RT_UPDATE *RecvdUpdatePacket, int costToNbr, int myID);


////////////////////////////////////////////////////////////////
void InitRoutingTbl (struct pkt_INIT_RESPONSE *InitResponse, int myID){

	//ntoh_pkt_INIT_RESPONSE(InitResponse);

	unsigned int no_nbr = InitResponse -> no_nbr;
	int i; //iterator variable

	// path to self is len 1
	// to neighbor is 2
	//

	//iterate through all neighbors and update routing table accordingly
	struct route_entry temp;
	temp.dest_id = myID;
	temp.cost = 0;
	temp.next_hop = myID;
	temp.path_len = 1;
	temp.path[0] = myID;

	routingTable[0] = temp;

	NumRoutes++;

	for (i = 0; i < no_nbr; i++)
	{

		struct route_entry temp;
		temp.dest_id = InitResponse->nbrcost[i].nbr;
		temp.cost = InitResponse->nbrcost[i].cost;
		temp.next_hop = InitResponse->nbrcost[i].nbr;
		temp.path_len = 2;
		temp.path[0] = myID;
		temp.path[1] = InitResponse->nbrcost[i].nbr;
		routingTable[i+1] = temp;

		NumRoutes++;
	}


	return;
}

/* Routine Name    : UpdateRoutes
 * INPUT ARGUMENTS : 1. (struct pkt_RT_UPDATE *) - The Route Update message from one of the neighbors of the router.
 *                   2. int - The direct cost to the neighbor who sent the update. 
 *                   3. int - My router's id received from command line argument.
 * RETURN VALUE    : int - Return 1 : if the routing table has changed on running the function.
 *                         Return 0 : Otherwise.
 * USAGE           : This routine is called after receiving the route update from any neighbor. 
 *                   The routing table is then updated after running the distance vector protocol. 
 *                   It installs any new route received, that is previously unknown. For known routes, 
 *                   it finds the shortest path using current cost and received cost. 
 *                   It also implements the forced update and split horizon rules. My router's id
 *                   that is passed as argument may be useful in applying split horizon rule.
 */
// struct pkt_RT_UPDATE {
//  	unsigned int sender_id; /* id of router sending the message */
//		unsigned int dest_id; /* id of neighbor router to which routing table is sent */
// 		unsigned int no_routes; /* number of routes in my routing table */
// 		struct route_entry route[MAX_ROUTERS]; /* array containing rows of routing table */
// 
// 
////////////////////////////////////////////////////////////////
int UpdateRoutes(struct pkt_RT_UPDATE *RecvdUpdatePacket, int costToNbr, int myID){
	int ret_Val = 0;

	int sender = RecvdUpdatePacket->sender_id;
	int no_routes = RecvdUpdatePacket->no_routes;

	int i, j, k, m;

	for (j = 0; j < no_routes; j++)
	{
		int route_destination = RecvdUpdatePacket->route[j].dest_id;
		int newEntry = 1;

		// iterate through all previously known elements in routing table

		for (i = 0; i < NumRoutes; i++)
		{
			// flag to determine if route destination already exists
						
			//if we're talking about the same destination router. it already exists in routing table
			if (route_destination == routingTable[i].dest_id)
			{
				newEntry = 0;

				int myId_inRoute = 0;

				//check if myID is in the route and set myId_inRoute if so
				for (k = 0; k < RecvdUpdatePacket->route[j].path_len; k++)
				{
					if (myID == RecvdUpdatePacket->route[j].path[k])
					{
						myId_inRoute = 1;
					}
				}


				//Path Vector Rule
				// if myId is not in the route and the cost is cheaper than previous cost, replace path
				if (!myId_inRoute && (RecvdUpdatePacket->route[j].cost < routingTable[i].cost))
				{
					//assign next hop to be the sender of path
					routingTable[i].next_hop = sender;

					//update the cost with cost from sender to dest + cost to sender
					routingTable[i].cost = RecvdUpdatePacket->route[j].cost + costToNbr;

					//preprend myId router to path list and increment path length
					for (m = 0; m < RecvdUpdatePacket->route[j].path_len; m++)
					{
						routingTable[i].path[m + 1] = RecvdUpdatePacket->route[j].path[m];
					}
					routingTable[i].path[0] = myID;	
					routingTable[i].path_len = RecvdUpdatePacket->route[j].path_len + 1;

					//router updated
					ret_Val = 1;
				}

				//forced update rule

				// if the next hop was already the sender, then update the cost for new path
				if (routingTable[i].next_hop == sender && RecvdUpdatePacket->route[j].cost > routingTable[i].cost)
				{
						routingTable[i].cost = RecvdUpdatePacket->route[j].cost + costToNbr;
						ret_Val = 1;
				}
			}
		}

		// after checking all of known routing table and if no same destination,
		// it must be a new entry
		if (newEntry)
		{

			routingTable[NumRoutes] = RecvdUpdatePacket->route[j];
			routingTable[NumRoutes].next_hop = sender;
			routingTable[NumRoutes].cost = RecvdUpdatePacket->route[j].cost + costToNbr;
			routingTable[NumRoutes].dest_id = route_destination;

			//prepend to path length
			for (m = 0; m < RecvdUpdatePacket->route[j].path_len; m++)
			{
				routingTable[NumRoutes].path[m + 1] = RecvdUpdatePacket->route[j].path[m];
			}

			routingTable[NumRoutes].path[0] = myID;

			routingTable[NumRoutes].path_len++;
			//assign path of sender to new path with sender's id prepended to path
			ret_Val = 1;

			NumRoutes++;

		}
	}
	return ret_Val;
}

////////////////////////////////////////////////////////////////
void ConvertTabletoPkt(struct pkt_RT_UPDATE *UpdatePacketToSend, int myID){
	
	UpdatePacketToSend -> sender_id = myID;
	UpdatePacketToSend -> no_routes = NumRoutes;
	
	memcpy(UpdatePacketToSend->route, routingTable, sizeof(routingTable));

	return;
}


////////////////////////////////////////////////////////////////
//It is highly recommended that you do not change this function!
void PrintRoutes (FILE* Logfile, int myID){
	/* ----- PRINT ALL ROUTES TO LOG FILE ----- */
	int i;
	int j;
	for(i = 0; i < NumRoutes; i++){
		fprintf(Logfile, "<R%d -> R%d> Path: R%d", myID, routingTable[i].dest_id, myID);

		/* ----- PRINT PATH VECTOR ----- */
		for(j = 1; j < routingTable[i].path_len; j++){
			fprintf(Logfile, " -> R%d", routingTable[i].path[j]);	
		}
		fprintf(Logfile, ", Cost: %d\n", routingTable[i].cost);
	}
	fprintf(Logfile, "\n");
	fflush(Logfile);
}


////////////////////////////////////////////////////////////////
void UninstallRoutesOnNbrDeath(int DeadNbr){
	
	int i;
	for (i = 0; i < NumRoutes; i++)
	{
		if (routingTable[i].next_hop == DeadNbr)
		{
			routingTable[i].cost = INFINITY;
		}
	}
	return;
}