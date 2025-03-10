#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#define ARE_SAME 0
#define IS_NUMBER 0
#define NOT_CORRECT -1
#define SA struct sockaddr_in
#define MAX_SIZE 5000
#define TRUE 1
#define FALSE 0

int isNumber(char* number){
    for (int i = 0; i < strlen(number); i++){
        if (isdigit(number[i]) == 0){
            return -1;
        }
    }
    return 0;
}

void errMessage(int pos){
    switch (pos){
        case -1:
            fprintf(stderr, "Couldn't create socket\n");
            break;
        default:
            fprintf(stderr, "Wrong %d. argument\n", pos);
    }
}


int main(int argc, char *argv[]) {

    // Check number of arguments
    if (argc != 7) {
        fprintf(stderr, "Wrong number of arguments\n");
        return -1;
    }

    int host[4];
    int host_index;
    const char* server_host;
    int port;
    int mode;


    int index = 1; // Currently being checked argv
    while (1){
        if (strcmp(argv[index],"-h") == ARE_SAME){
            index++;
            host_index = index;
            server_host = argv[index];
            // Checking, if host is correctly written   [0-9].[0-9].[0-9].[0-9]
            int consecutive_num = 0;
            int num_of_dots = 0;
            char temp_num[4] = "\0";

            for (int i = 0; i < strlen(argv[index]); i++){
                if ((isdigit(argv[index][i]) != 0) && (consecutive_num < 3)){
                    temp_num[consecutive_num] = argv[index][i];
                    consecutive_num++;
                }
                else if ((argv[index][i] == '.') && (num_of_dots < 3) && (consecutive_num != 0)){
                    host[num_of_dots] = atoi(temp_num);
                    temp_num[0] = '\0';
                    temp_num[1] = '\0';
                    temp_num[2] = '\0';
                    temp_num[3] = '\0';
                    consecutive_num = 0;
                    num_of_dots++;
                }
                else {
                    errMessage(index);
                    return -1;
                }
            }
            if (!((num_of_dots == 3) && (consecutive_num != 0))){
                errMessage(index);
                return -1;
            }
            host[num_of_dots] = atoi(temp_num);

            for (int i = 0; i < 4; i++){
                if (!((host[i] >= 0) && (host[i] <= 255))){
                    errMessage(index);
                    return -1;
                }
            }
            // End of checking host
        }



        else if (strcmp(argv[index],"-p") == ARE_SAME){
            index++;
            // Checking, if port is number in correct range
            if (isNumber(argv[index]) == IS_NUMBER){
                port = atoi(argv[index]);
                if (!((port >= 0) && (port <= 65535))){
                    errMessage(index);
                    return -1;
                }
            }
            else {
                errMessage(index);
                return -1;
            }
        }



        else if (strcmp(argv[index],"-m") == ARE_SAME){
            index++;
            if (strcmp(argv[index],"tcp") == ARE_SAME){
                mode = SOCK_DGRAM;
                break;
            }
            else if (strcmp(argv[index],"udp") == ARE_SAME){
                mode = SOCK_STREAM;
                break;
            }
            else {
                errMessage(index);
                return -1;
            }
        }



        else {
            errMessage(index);
            return -1;
        }

        index++;
    }
    printf("IP: %d.%d.%d.%d\n",host[0], host[1], host[2], host[3]);
    printf("Port: %d\n", port);
    printf("Mode: %d\n", mode);

    struct hostent *server = gethostbyname(server_host);
    if (server == NULL) {
        errMessage(-1);
        return -1;
    }

    SA address;
    bzero((char *) &address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    bcopy((char *)server->h_addr, (char *)&address.sin_addr.s_addr, server->h_length);
    
    int client = socket(AF_INET, mode, 0);
    if (client <= 0) {
        errMessage(-1);
        return -1;
    }

    int ending = FALSE;
    char buffer[MAX_SIZE];
    int mess_out;
    int mess_in;

    while (!ending){
        for (int i = 0; i<MAX_SIZE; i++){
            buffer[i] = '\0';
        }
        
        if (fgets(buffer, MAX_SIZE, stdin)){
            if (strcmp(buffer, "BYE\n") == ARE_SAME){
                ending = TRUE;
            }
        }        
        
        if (connect(client, (struct sockaddr *) &address, sizeof(address)) != 0) {
            errMessage(-1);
            return -1;      
        }
        mess_out = send(client, buffer, MAX_SIZE, 0);
        if (mess_out < 0){
            errMessage(-1);
        }
        mess_in = recv(client, buffer, MAX_SIZE, 0);
        if (mess_in < 0){
            errMessage(-1);
        }
    }
    close(client);
    return 1;
}