// Importing necessary librairies
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Struct of Process containing various Process attributes
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

void printOutput(Process * processArray, int lastTime, int arrayCount); // prints process time outputs
Process * parseProcess(Process * processArray, char * resources[], char * line, char * readyQueue, char * prevreadyQueue, int * processRunning, int * prevTime, int * prevTimeTwo, int * lastTime, int * arrayCount); // parses lines from file

int main(int argc, char *argv[] ) {
    char * readyQueue = malloc(sizeof(char) * 1000 + 1); // assuming 1000 char max
    char * prevreadyQueue = malloc(sizeof(char) * 1000 + 1); // readyQueue one process back
    readyQueue[0] = '\0';
    prevreadyQueue[0] = '\0';
    Process * processArray = malloc(1000 * sizeof(Process)); // assuming size 1000
    Process process;
    process.processId = 0;
    processArray[0] = process; // Idle process

    char ** resources = malloc(1000 * sizeof(char *));  // assuming at most 1000 resources used
    // ^Index represents resource, value represents process concactenated with arrival time

    // Special Case - Index 0 of Array represents System Idle Process
    int arrayCount = 1; // represents current size of array-1
    int processRunning = 0; // represents current running process (0 if none/idle)
    int prevTime = 0; // represents arrival time of previous process
    int prevTimeTwo = 0; // represents arrival time of process before previous process (these variables used for calculating ready time)
    int lastTime = 0; // represents last process arrival time (for calculating idle running time)

    char buffer[1000]; // buffer with size 1000 to store file line input

    while (fgets(buffer, 1000, stdin) != NULL) {
        buffer[strlen(buffer)-1] = '\0';
        if (strlen(buffer) == 0) {
            continue;
        }
        processArray = parseProcess(processArray, resources, buffer, readyQueue, prevreadyQueue, &processRunning, &prevTime, &prevTimeTwo, &lastTime, &arrayCount);
    }

    printOutput(processArray, lastTime, arrayCount);
    free(processArray);
    free(resources);
    free(readyQueue);
    free(prevreadyQueue);
    return 0;
}

void printOutput(Process * processArray, int lastTime, int arrayCount) {
    int i = 0;
    for (i = 1; i < arrayCount; i++) {
        lastTime = lastTime - processArray[i].timeRun;
    }
    processArray[0].timeRun = lastTime;
    printf("0 %d\n", processArray[0].timeRun);

    for (i = 1; i < arrayCount; i++) {
        printf("%d %d %d %d\n", processArray[i].processId, processArray[i].timeRun, processArray[i].timeReady, processArray[i].timeBlocked);
    }
    return;
}

Process * parseProcess(Process * processArray, char * resources[], char * line, char * readyQueue, char * prevreadyQueue, int * processRunning, int * prevTime, int * prevTimeTwo, int * lastTime, int * arrayCount) {
    char * values = NULL;
    char * arrivalTime = strtok(line, " "); // arrival time
    values = strtok(NULL, " "); // state

    strcpy(prevreadyQueue, readyQueue);

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
            if (strlen(readyQueue) < 2) {
                *processRunning = 0;
            } else {
                char * tempBuff = malloc(sizeof(char) * strlen(readyQueue) + 100);
                strcpy(tempBuff, readyQueue);
                char * nextProcess = strtok(tempBuff, " ");
                *processRunning = atoi(nextProcess);
                memmove(readyQueue, readyQueue + strlen(nextProcess)+1, strlen(readyQueue));
                free(tempBuff);
            }
        } else {
            char * temp = malloc(sizeof(char) * strlen(idS) + 2);
            strcpy(temp, " ");
            strcat(temp, idS);
            replace_str(readyQueue, temp, "");
            free(temp);
        }
    } else if (strcmp(values, "I") == 0) {
        strtok(NULL, " ");
        char * interruptIds = strtok(NULL, " ");
        int interruptId = atoi(interruptIds);
        for (int i = 1; i < *arrayCount; i++) {
            if (processArray[i].processId == interruptId) {
                processArray[i].timeBlocked += (atoi(arrivalTime) - atoi(strtok(resources[interruptId], " ")));
                free(resources[interruptId]);
                break;
            }
        }

        if (*processRunning == 0 && strlen(readyQueue) < 2) {
            *processRunning = interruptId;
        } else {
            strcat(readyQueue, " ");
            strcat(readyQueue, interruptIds);
        }

    } else if (strcmp(values, "T") == 0) {
        char readyProcess[10000]; // contains currently running process to be placed in queue
        int tempProcess = *processRunning; // temp variable that contains running process

        if (strlen(readyQueue) > 1) {
            char * processString = strtok(readyQueue, " ");
            *processRunning = atoi(processString);
            memmove(readyQueue, readyQueue + strlen(processString) + 1, strlen(readyQueue));
            strcat(readyQueue, " ");
            sprintf(readyProcess,"%d", tempProcess);
            strcat(readyQueue, readyProcess);
        }

    } else if (strcmp(values, "E") == 0) {
        char * id = strtok(NULL, " ");
        char * temp = malloc(sizeof(char) * strlen(id) + 2);
        strcpy(temp, " ");
        strcat(temp, id);

        if (*processRunning == atoi(id)) {
            if (strlen(readyQueue) < 2) {
                *processRunning = 0;
            } else {
                char * tempBuff = malloc(sizeof(char) * strlen(readyQueue) + 100);
                strcpy(tempBuff, readyQueue);
                char * nextProcess = strtok(tempBuff, " ");
                *processRunning = atoi(nextProcess);
                memmove(readyQueue, readyQueue + strlen(nextProcess)+1, strlen(readyQueue));
                free(tempBuff);
            }
        } else {
            replace_str(readyQueue, temp, "");
        }
        free(temp);

        for (int i = 0; i < *arrayCount; i++) {
            if (processArray[i].processId == atoi(id)) {
                processArray[i].timeRun = atoi(arrivalTime) - processArray[i].arrivalTime - processArray[i].timeBlocked - processArray[i].timeReady;
                break;
            }
        }
    }

    if (!(*prevTime == 0 && *prevTimeTwo == 0) && strlen(prevreadyQueue) > 0) {
        char * buffer = malloc(sizeof(char) * strlen(prevreadyQueue) + 100);
        strcpy(buffer, prevreadyQueue);
        char * readyProcesses = strtok(buffer, " ");
        while (readyProcesses != NULL) {
            for (int i = 0; i < *arrayCount; i++) {
                if (processArray[i].processId == atoi(readyProcesses)) {
                    processArray[i].timeReady += atoi(arrivalTime) - *prevTime;
                    break;
                }
            }
            readyProcesses = strtok(NULL, " ");
        }
        free(buffer);
    }

    if (*prevTimeTwo == 0 && *prevTime == 0) {
        *prevTimeTwo = atoi(arrivalTime);
    } else if (*prevTimeTwo != 0 && *prevTime == 0) {
        *prevTime = atoi(arrivalTime);
    } else {
        *prevTimeTwo = *prevTime;
        *prevTime = atoi(arrivalTime);
    }

    *lastTime = atoi(arrivalTime);
    return processArray;
}
