#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//validation: check to make sure every line has two spaces, 3 tokens,
//first token positive integer > 0, second token a single char that
// must be C/R/T/I and 3rd token an integer > 0
//nvm



/**
 * Struct of Process containing various Process attributes
 **/
typedef struct {
    int processId;
    char state;
    int arrivalTime;
    int timeRun;
    int timeReady;
    int timeBlocked;
} Process;

char *replace_str(char *str, char *orig, char *rep)
{
  static char buffer[4096];
  char *p;

  if(!(p = strstr(str, orig)))  // Is 'orig' even in 'str'?
    return str;

  strncpy(buffer, str, p-str); // Copy characters from 'str' start to 'orig' st$
  buffer[p-str] = '\0';

  sprintf(buffer+(p-str), "%s%s", rep, p+strlen(orig));

  return buffer;
}
// void printOutput(Process * processArray[]); // prints process time outputs
Process * parseProcess(Process * processArray, char * resources[], char * line, char * readyQueue, int * processRunning, int * arrayCount); // parses lines from file

int main(int argc, char *argv[] ) {
    // int i = 0;
    char * readyQueue = malloc(sizeof(char) * 1000 + 1); // assuming 1000 processes max
    Process * processArray = malloc(1000 * sizeof(Process)); // assuming size 1000
    Process process;
    process.processId = 0;
    processArray[0] = process; // Idle process

    char ** resources = malloc(1000 * sizeof(char *));  // assuming at most 1000 resources used
    // for (i = 0; i < 1000; i++) {
    //     resources[i] = malloc(sizeof(char) + 1);
    // }

    // ^Index represents resource, value represents process concactenated with arrival time
    // Special Case - Index 0 of Array represents System Idle Process
    int arrayCount = 1; // represents current size of array-1
    int processRunning = 0; // represents current running process (0 if none/idle)

    char buffer[1000]; // buffer with size 1000 to store file line input
    // char line[1000];

    while (fgets(buffer, 1000, stdin) != NULL) {
        buffer[strlen(buffer)-1] = '\0';
        printf("%s\n", buffer);
        // line = (char*)malloc(sizeof(char) * (strlen(buffer)+1));
        // strcpy(line, buffer);
        if (strlen(buffer) == 0) {
            continue;
        }

        processArray = parseProcess(processArray, resources, buffer, readyQueue, &processRunning, &arrayCount);
    }

    for (int i = 1; i < arrayCount; i++) {
        printf("test %d process %d\n", processArray[i].timeBlocked, processArray[i].processId);
    }
    free(processArray);
    free(resources);
    return 0;
}

// void printOutput(Process * processArray[]) {
//     // print processArray output
//     return;
// }

Process * parseProcess(Process * processArray, char * resources[], char * line, char * readyQueue, int * processRunning, int * arrayCount) {
    char * values = NULL;
    char * arrivalTime = strtok(line, " "); // arrival time
    values = strtok(NULL, " "); // state

    if (strcmp(values, "C") == 0) { // creation
        values = strtok(NULL, " "); // process ID
        Process process;
        process.arrivalTime = atoi(arrivalTime);
        process.processId = atoi(values);
        process.state = 'C';
        process.timeRun = 0;
        process.timeReady = 0;
        process.timeBlocked = 0;
        processArray[*arrayCount] = process;
        (*arrayCount)++;

        switch(*processRunning) { // Manages ready queue and currently running process
            case 0:
                *processRunning = atoi(values);
                break;
            default:
                strcat(readyQueue, " ");
                strcat(readyQueue, values);
                break;
        }

    } else if (strcmp(values, "R") == 0) {
        char * resReq = NULL;
        char * idS;
        int id = 0;
        char * tempBuffer = malloc(sizeof(char*) * 100);
        resReq = strtok(NULL, " "); // resource requested
        idS = strtok(NULL, " ");
        id = atoi(idS);
        resources[id] = (char *)malloc(sizeof(char) * (strlen(arrivalTime) + 100 + 2)); //2 for null terminator + space
        strcpy(resources[id], arrivalTime);
        strcat(resources[id], " ");
        strcat(resources[id], resReq);
        free(tempBuffer);

        if (*processRunning == id) {
            printf("A PROCESS %d\n", *processRunning);
            printf("READYQUEUE %s\n", readyQueue);
            char * nextProcess = strtok(readyQueue, " ");
            *processRunning= atoi(nextProcess); // run next in queue
            strcpy(readyQueue, readyQueue + 2);
            printf("A PROCESS %d\n", *processRunning);
            printf("READYQUEUE %s\n", readyQueue);
        } else {
            printf("B PROCESS %d\n", *processRunning);
            printf("READYQUEUE %s\n", readyQueue);
            char * temp = malloc(sizeof(char) * strlen(idS) + 2);
            strcpy(temp, " ");
            strcat(temp, idS);
            replace_str(readyQueue, temp, "");
            printf("B PROCESS %d\n", *processRunning);
            printf("READYQUEUE %s\n", readyQueue);
            exit(0);
        }
    } else if (strcmp(values, "I") == 0) {
        strtok(NULL, " ");
        int interruptId = atoi(strtok(NULL, " "));
        for (int i = 1; i < *arrayCount; i++) {
            if (processArray[i].processId == interruptId) {
                printf("process %d\n", processArray[i].processId);
                printf("er %d\n", atoi(arrivalTime));
                printf("re %s\n", resources[interruptId]);
                processArray[i].timeBlocked += (atoi(arrivalTime) - atoi(strtok(resources[interruptId], " ")));
                printf("should be good %d\n", processArray[i].timeBlocked);
                free(resources[interruptId]);
                break;
            }
        }
    } else if (strcmp(values, "T") == 0) {
        char readyProcess[10000]; // contains currently running process to be placed in queue
        int tempProcess = *processRunning; // temp variable that contains running process
        *processRunning = atoi(strtok(readyQueue, " "));
        strcpy(readyQueue, readyQueue + 2);
        strcat(readyQueue, " ");
        sprintf(readyProcess,"%d", tempProcess);
        strcat(readyQueue, readyProcess);

    } else if (strcmp(values, "E") == 0) {
        
    }
    return processArray;
}
