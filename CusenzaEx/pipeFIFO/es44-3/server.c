/*
 * Add code to the fifo_seqnum_server.c so that if the program receives the SIGINT or SIGTERM signals,
 * it removes the server FIFO and terminates.
 * Server reads from client the number to be added after the message to a int total form a certain fifo. Client determines
 * through its pid where the server must send the message, which contains total.
 * We will use one address to receive (see header) data and the one decided by the client to write data.
 */

#include <signal.h>
#include <sys/stat.h>
#include "fifo_seqnum.h"

static void removeFIFO(int sig) //called on exit
{
	unlink();
	if(sig == SIGNINT || sig == SIGTERM)
		exit(EXIT_SUCCESS);
}

int main(void) {
	int fdC2S, fdS2C, total = 0;
	struct msgS2C response;
	struct msgC2S request;

	atexit(unlink);
	signal(SIGINT, removeFIFO);
	signal(SIGTERM, removeFIFO);

	//create fifo
	mkfifo(FIFO_2SERVER, 0666); // see header, since clients->server fifo's name must be shared
	fdC2S = open(FIFO_2SERVER, O_RDONLY);

	while(1) {
		//read message from client
		read(fdC2S, &request, sizeof(request));

		//write total read message number
		char server2client_fn[20];
		sprintf(server2client_fn, SERVER2CLIENT_TEMPLATE, request.pid); //build path for fifo server -> client
		fdS2C = open(server2client_fn, O_WRONLY); //child make this fifo
		response.total = total;

		write(fdS2C, &response, sizeof(response));
		close(fdS2C);

		//update total read message number
		total += request.seqNum;
	}

	return 0;
}