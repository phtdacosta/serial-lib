#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "seriallib.h"

int main (int argc, char *argv []) {

    char *port = malloc (sizeof (char) * 12); // Just for "straight to test purposes", in production, use the port detection functions instead
    strcpy(port, "\\\\.\\COM5");

//------------------------------------ Stand-alone function to do their straight work the most synchronous way ------------------------------------

    AVAILABLE_PORTS *ports;                         // Initializing the struct values
    ports = malloc(sizeof(AVAILABLE_PORTS));

    updateAvailablePortsInfo(ports);                // Setup/load time, the function will update the living struct



    DATA_BUFFER *wbuffer;                    // Initializing the struct values
    wbuffer = malloc(sizeof(DATA_BUFFER));   // Buffer Containing Txed Data
    strcpy(wbuffer->data, "Hello World");

    writeSerialPort(port, 9600, wbuffer);
    printf("TX Code %i - %s Data -> %s\n", wbuffer->code, wbuffer->message, wbuffer->data);



    DATA_BUFFER *rbuffer;                    // Initializing the struct values
    rbuffer = malloc(sizeof(DATA_BUFFER));   // Buffer Containing Rxed Data

    while (1) {
        listenSerialPort(port, 9600, rbuffer);
        printf("RX Code %i - %s Data -> %s\n", rbuffer->code, rbuffer->message, rbuffer->data);
    }

    return 0;
}