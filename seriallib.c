#if defined(_WIN32) || defined(_WIN64)
	#include <windows.h>
#endif

#include "seriallib.h"
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>

char* formatSerialPortName(int num) {

    #if defined(_WIN32) || defined(_WIN64)

        char *data = malloc (sizeof (char) * 15); // Allocates memory to handle all possible argument numbers between 1 and 256
        char *str = malloc (sizeof (char) * 4);   // Allocates memory just to cast the given argument number into a char
        sprintf(str, "%d", num);                  // Casts the int to char
        strcpy(data, "\\\\.\\COM");               // Copies the port base name into the previously allocated space
        strcat(data, str);                        // Concatenates the newly casted char with the port base name

        return data;

    #elif defined(__linux__) || defined(__linux)
        // No code yet
    #endif

}

int getSerialPortAvailability(char *portName) {

    #if defined(_WIN32) || defined(_WIN64)

        HANDLE hComm;

        /*----------------------------------- Opening the Serial Port --------------------------------------------*/
        hComm = CreateFile(portName,                       // Port name
                           GENERIC_READ | GENERIC_WRITE,   // Read/Write
                           0,                              // No Sharing
                           NULL,                           // No Security
                           OPEN_EXISTING,		           // Open existing port only
                           0,            		           // Non Overlapped I/O
                           NULL);        		           // Null for Comm Devices

        if (hComm == INVALID_HANDLE_VALUE) {
            printf("Error! CreateFile() %s\n", portName);
            return 0;
        }

        CloseHandle(hComm);	//Closing the Serial Port

        return 1;

    #elif defined(__linux__) || defined(__linux)
        // No code yet
    #endif
}

void updateAvailablePortsInfo(AVAILABLE_PORTS *ports) {

    #if defined(_WIN32) || defined(_WIN64)

        ports->openQuantity = 0;                        // Struct initialization
//        int array[S_PORTS] = { 0 };                         // As the memory was already allocated/initialized for the array, it can only be updated by whole copying a new one over the existant one
//        memcpy(ports->statusList, array, sizeof array); // These two lines are just for good practices, they are not actually needed

        int i, status;                                  // These two variables are just for loop actions control

        for (i = 0; i < S_PORTS; ++i) {
            status = getSerialPortAvailability(formatSerialPortName(i));
            if (status != 0) {                  // Zero status means that the port is not available
                ports->openQuantity++;          // Updates the number of open ports
                ports->statusList[i] = status;  // Records which port got its status different from zero on its proper position of number array
                status = 0;                     // Clear the status value for the next iteration
            }
        }

    #elif defined(__linux__) || defined(__linux)
        // No code yet
    #endif
}

void writeSerialPort(char *portName, int baudRate, DATA_BUFFER *buffer) {

    #if defined(_WIN32) || defined(_WIN64)

        HANDLE hComm;   // Handle to the Serial port
        BOOL   Status;

        buffer->code = 0;                       // Default code is zero
        *buffer->message = 0;                   // Straightest way to erase all pointer content

        /*----------------------------------- Opening the Serial Port --------------------------------------------*/
        hComm = CreateFile(portName,                          // Name of the Port to be Opened
                           GENERIC_READ | GENERIC_WRITE,      // Read/Write Access
                           0,                                 // No Sharing, ports cant be shared
                           NULL,                              // No Security
                           OPEN_EXISTING,                     // Open existing port only
                           0,                                 // Non Overlapped I/O
                           NULL);                             // Null for Comm Devices

        if (hComm == INVALID_HANDLE_VALUE) {
            strcpy(buffer->message, "Error! CreateFile() ");
            strcat(buffer->message, portName);
            return;
        }

        /*------------------------------- Setting the Parameters for the SerialPort ------------------------------*/
        DCB dcbSerialParams = { 0 };                        // Initializing DCB structure
        dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

        Status = GetCommState(hComm, &dcbSerialParams);     //retreives  the current settings

        if (Status == FALSE) {
            strcpy(buffer->message, "Error! GetCommState() ");
            strcat(buffer->message, portName);
            return;
        }

        dcbSerialParams.BaudRate = baudRate;                      // Setting BaudRate = CBR_<baud_rate_number>
        dcbSerialParams.ByteSize = 8;                             // Setting ByteSize = 8
        dcbSerialParams.StopBits = ONESTOPBIT;                    // Setting StopBits = 1
        dcbSerialParams.Parity   = NOPARITY;                      // Setting Parity = None 

        Status = SetCommState(hComm, &dcbSerialParams);  //Configuring the port according to settings in DCB 

        if (Status == FALSE) {
            strcpy(buffer->message, "Error! GetCommState() ");
            strcat(buffer->message, portName);
            return;
        }

        /*------------------------------------ Setting Timeouts --------------------------------------------------*/
        COMMTIMEOUTS timeouts = { 0 };

        timeouts.ReadIntervalTimeout         = 50;
        timeouts.ReadTotalTimeoutConstant    = 50;
        timeouts.ReadTotalTimeoutMultiplier  = 10;
        timeouts.WriteTotalTimeoutConstant   = 50;
        timeouts.WriteTotalTimeoutMultiplier = 10;

        if (SetCommTimeouts(hComm, &timeouts) == FALSE) {
            strcpy(buffer->message, "Error! SetCommState() ");
            strcat(buffer->message, portName);
            return;
        }
    
        /*----------------------------- Writing a Character to Serial Port----------------------------------------*/
        DWORD  dNoOFBytestoWrite;                // No of bytes to write into the port
        DWORD  dNoOfBytesWritten = 0;            // No of bytes written to the port

        dNoOFBytestoWrite = sizeof(buffer->message);  // Calculating the no of bytes to write into the port

        Status = WriteFile(hComm,                // Handle to the Serialport
                           buffer->message,      // Data to be written to the port (should be char or byte array, otherwise write will fail)
                           dNoOFBytestoWrite,    // No of bytes to write into the port
                           &dNoOfBytesWritten,   // No of bytes written to the port
                           NULL);

        if (Status != TRUE) {
            strcpy(buffer->message, "Error ");
            char *str = malloc (sizeof (char) * 4);   // Allocates memory just to cast the given argument number into a char
            sprintf(str, "%d", GetLastError());       // Casts the int to char
            strcat(buffer->message, str);
            strcat(buffer->message, " WriteFile() ");
            strcat(buffer->message, portName);
            return;
        }
        
        buffer->code = 1;
        
        CloseHandle(hComm);  // Closing the Serial Port
        return;

    #elif defined(__linux__) || defined(__linux)
        // No code yet
    #endif
}

void listenSerialPort(char *portName, int baudRate, DATA_BUFFER *buffer) {

    #if defined(_WIN32) || defined(_WIN64)

        HANDLE hComm;                           // Handle to the Serial port
        BOOL  Status;                           // Status of the various operations 
        DWORD dwEventMask;                      // Event mask to trigger
        char  TempChar;                         // Temperory Character
        DWORD NoBytesRead;                      // Bytes read by ReadFile()
        int i = 0;

        buffer->code = 0;                       // Default code is zero
        *buffer->message = 0;                   // Straightest way to erase all pointer content

        /*----------------------------------- Opening the Serial Port --------------------------------------------*/
        hComm = CreateFile(portName,                          // Name of the Port to be Opened
                           GENERIC_READ | GENERIC_WRITE,      // Read/Write Access
                           0,                                 // No Sharing, ports cant be shared
                           NULL,                              // No Security
                           OPEN_EXISTING,                     // Open existing port only
                           0,                                 // Non Overlapped I/O
                           NULL);                             // Null for Comm Devices

        if (hComm == INVALID_HANDLE_VALUE) {
            strcpy(buffer->message, "Error! CreateFile() ");
            strcat(buffer->message, portName);
            return;
        }

        /*------------------------------- Setting the Parameters for the SerialPort ------------------------------*/
        DCB dcbSerialParams = { 0 };                        // Initializing DCB structure
        dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

        Status = GetCommState(hComm, &dcbSerialParams);     //retreives  the current settings

        if (Status == FALSE) {
            strcpy(buffer->message, "Error! GetCommState() ");
            strcat(buffer->message, portName);
            return;
        }

        dcbSerialParams.BaudRate = baudRate;                      // Setting BaudRate = CBR_<baud_rate_number>
        dcbSerialParams.ByteSize = 8;                             // Setting ByteSize = 8
        dcbSerialParams.StopBits = ONESTOPBIT;                    // Setting StopBits = 1
        dcbSerialParams.Parity   = NOPARITY;                      // Setting Parity = None 

        Status = SetCommState(hComm, &dcbSerialParams);  //Configuring the port according to settings in DCB 

        if (Status == FALSE) {
            strcpy(buffer->message, "Error! SetCommState() ");
            strcat(buffer->message, portName);
            return;
        }

        /*------------------------------------ Setting Timeouts --------------------------------------------------*/
        COMMTIMEOUTS timeouts = { 0 };

        timeouts.ReadIntervalTimeout         = 50;
        timeouts.ReadTotalTimeoutConstant    = 50;
        timeouts.ReadTotalTimeoutMultiplier  = 10;
        timeouts.WriteTotalTimeoutConstant   = 50;
        timeouts.WriteTotalTimeoutMultiplier = 10;

        if (SetCommTimeouts(hComm, &timeouts) == FALSE) {
            strcpy(buffer->message, "Error! SetCommTimeouts() ");
            strcat(buffer->message, portName);
            return;
        }

        /*------------------------------------ Setting Receive Mask ----------------------------------------------*/
        Status = SetCommMask(hComm, EV_RXCHAR); //Configure Windows to Monitor the serial device for Character Reception

        if (Status == FALSE) {
            strcpy(buffer->message, "Error! SetCommMask() ");
            strcat(buffer->message, portName);
            return;
        }

        /*------------------------------------ Setting WaitComm() Event   ----------------------------------------*/
        Status = WaitCommEvent(hComm, &dwEventMask, NULL); //Wait for the character to be received

        /*-------------------------- Program will Wait here till a Character is received ------------------------*/				
        if (Status == FALSE) {
            strcpy(buffer->message, "Error! WaitCommEvent() ");
            strcat(buffer->message, portName);
            return;
        }
        else { //If WaitCommEvent() == True, read the RXed data using ReadFile();
            buffer->code = 1;
            do {
                Status = ReadFile(hComm, &TempChar, sizeof(TempChar), &NoBytesRead, NULL);
//                SerialBuffer[i] = TempChar;
                buffer->data[i] = TempChar;
                i++;
            }
//            while (NoBytesRead > 0 && i < S_BUFFER); // Need improvement in logic
            while (i < S_BUFFER);

        }

        CloseHandle(hComm);     //Closing the Serial Port
        return;

    #elif defined(__linux__) || defined(__linux)
        // No code yet
    #endif
}