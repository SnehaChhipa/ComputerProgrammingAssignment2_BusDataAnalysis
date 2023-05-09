//I used CLion for this assignment.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//Declare Global Variables:
int maxNumOfIndSeq = 0; //Counts the number of indicative sequence / stop point ID in a trip.
double uniqueTrips = 0;
int maxFreq = 0; //Keeps count of the most occurred bus trip. (its frequency)
int numOfStopsIDsIncorrectSeq = 0;


//Declare structs:
typedef struct {   //Stores each row of data in one struct, in an array of structs busData[].
    char dayOfService[10];
    int indSeq;
    int stopPntID;
    int time_ARR;
    int uniqueID;
} busTrip;

typedef struct { //Stores each unique trip in one struct, in an array of structs busRoute[].
    int uniqueID;
    int* indSequence;
    int* stopPointID;
    int* timeAPR;
} individualBusRoute;

struct uniqueSequences { //Stores information of each unique sequence in one struct, in a linked list.
    int uniqueSequenceIndex; //Keeps an index position of a unique sequence according to its position in structs array.
    int frequency;
    struct uniqueSequences* nextSeq;
};


typedef struct uniqueSequences UniqueSeq;
typedef UniqueSeq* UniqueSeqPtr;

//Declaring useful pointers:
busTrip *busData;
individualBusRoute *busRoute;
UniqueSeqPtr startNode;


//Declaring functions:
int findNumOfRows(void); //finds number of rows in file
void readingFile(int size); //reads data from file and stores it in busData[]
void storingValues(int size); //finds uniques bus trips and stores data in busRoute[]
void rearrangingData(int index); //sorts Indicative Sequence, and further elements of struct.
int findingUniqueSequences(void); //finds unique sequences and stores them in a linked list.
int initialChecks(void); //a function which calls repair function for each unique sequence and returns the number of additional trips which can be repaired.
int repair(int* correctSequence, int* incorrect_sequence, int sizeOfCorrectSeq); //Finds and Prints repairs. Returns 1 if a sequence can be repaired, else returns 0.


int main(void)
{
    int size = findNumOfRows(); //Number of rows.

    readingFile(size);
    storingValues(size);

    for (int i=0; i<uniqueTrips; i++)
    {
        rearrangingData(i);
    }

    int numberOfUniqueSequences = findingUniqueSequences();

    //Analysis:
    printf("There are %d rows in the dataset.\n", size);
    printf("There are %.0lf unique trips.\n", uniqueTrips);
    printf("There are %d unique sequences.\n\n", numberOfUniqueSequences);

    //Printing the Correct Sequence:
    printf("Correct Sequence. Occurs %d times.\n", maxFreq);
    UniqueSeqPtr correctSequence = startNode;
    for (int m=0; busRoute[correctSequence->uniqueSequenceIndex].stopPointID[m] != 0; m++)
    {
        printf("%d ", busRoute[correctSequence->uniqueSequenceIndex].stopPointID[m]);
        numOfStopsIDsIncorrectSeq++;
    }
    printf("\n\n");


    int numOfAdditionalRepairedTrips = initialChecks();
    int totalCorrectTrips = startNode->frequency + numOfAdditionalRepairedTrips;

    //Printing Analysis:
    printf("Initially %d trips contained the correct sequence.\n", maxFreq);
    printf("This is %.2lf percent of the dataset.\n", maxFreq / uniqueTrips*100 );
    printf("%d additional trips can be repaired by dropping additional stops or interpolating missing stops.\n", numOfAdditionalRepairedTrips);
    printf("This is a total of %d trips.\n", totalCorrectTrips);
    printf("This is %.2lf percent of the dataset.\n", totalCorrectTrips/uniqueTrips*100);

    return 0;
}




int findNumOfRows()
{
    char headerOfFile[70];
    char buffer[32]; //to temporary store data
    int countRows = 0;

    FILE *dataPtr;

    if ((dataPtr = fopen("comp10120_bus.csv", "r")) == NULL)
    {
        puts("File could not be opened");
    }
    else
    {
        fscanf(dataPtr,"%s", headerOfFile);

        while(!feof(dataPtr))
        {
            fscanf(dataPtr, "%s", buffer);
            countRows++;
        }
    }

    fclose(dataPtr);
    return countRows;
}





void readingFile(int size)
{
    char headerOfFile[70];
    char line[31];
    const char comma[2] = ",";
    char *token;
    int buffer, numOfIndSeq = 0;

    busData = malloc(sizeof(busTrip)*size); //assigning memory location to struct array to store each row of data


    FILE *dataPtr;

    if ((dataPtr = fopen("comp10120_bus.csv", "r")) == NULL)
    {
        puts("File could not be opened");
    }
    else
    {
        fscanf(dataPtr, "%s", headerOfFile);

        for(int i = 0; i<size; i++) //Storing data:
        {

            fscanf(dataPtr, "%s", line);

            token = strtok(line, comma);
            strcpy(busData[i].dayOfService, token);

            token = strtok(NULL, comma);
            buffer = atoi(token);
            busData[i].indSeq = buffer;

            token = strtok(NULL, comma);
            buffer = atoi(token);
            busData[i].stopPntID = buffer;

            token = strtok(NULL, comma);
            buffer = atoi(token);
            busData[i].time_ARR = buffer;

            token = strtok(NULL, comma);
            buffer = atoi(token);
            busData[i].uniqueID = buffer;

            if (i==0 || busData[i].uniqueID != busData[i-1].uniqueID)
            {
                uniqueTrips++; //counting unique trips

                if (numOfIndSeq >= maxNumOfIndSeq)
                {
                    maxNumOfIndSeq = numOfIndSeq; //finding the maximum number of stop point ID/Indicative Sequence
                }
                numOfIndSeq = 0;
            }
            numOfIndSeq++;
        }

    }

    fclose(dataPtr);
}




void storingValues(int size)
{
    int j, k;

    busRoute = malloc(sizeof(individualBusRoute) * uniqueTrips); //assigning memory location to struct array to store each unique trip

    j=0;
    for (int i=0; i<size; i++)
    {

        if (i==0 || busData[i].uniqueID != busData[i-1].uniqueID) //If new trip found:
        {
            busRoute[j].uniqueID = busData[i].uniqueID;
            busRoute[j].indSequence = malloc(sizeof(int)*maxNumOfIndSeq);  //assigning memory location to integer array
            busRoute[j].stopPointID = malloc(sizeof(int)*maxNumOfIndSeq); //assigning memory location to integer array
            busRoute[j].timeAPR = malloc(sizeof(int)*maxNumOfIndSeq); //assigning memory location to integer array
            k=0;
            j++;
        }

        //storing values:
        busRoute[j-1].indSequence[k] = busData[i].indSeq;
        busRoute[j-1].stopPointID[k] = busData[i].stopPntID;
        busRoute[j-1].timeAPR[k] = busData[i].time_ARR;

        k++;
    }
}





void rearrangingData(int index)
{
    int p, q, swapIndSeq, swapStopID, swapTime;
    int nonzeroIndex = 0;

    for (p=1; p < maxNumOfIndSeq; p++)
    {
        q=p;
        while(q > 0 && busRoute[index].indSequence[q] < busRoute[index].indSequence[q-1])
        {

            swapIndSeq = busRoute[index].indSequence[q];
            swapStopID = busRoute[index].stopPointID[q];
            swapTime = busRoute[index].timeAPR[q];

            busRoute[index].indSequence[q] = busRoute[index].indSequence[q-1];
            busRoute[index].stopPointID[q] = busRoute[index].stopPointID[q-1];
            busRoute[index].timeAPR[q] = busRoute[index].timeAPR[q-1];

            busRoute[index].indSequence[q-1] = swapIndSeq;
            busRoute[index].stopPointID[q-1] = swapStopID;
            busRoute[index].timeAPR[q-1] = swapTime;

            q--;
        }
    }

    //Since all indicativeSequence and StopPointID arrays are of maximum size, it means that some positions within arrays will be filled with zero.
    // Therefore, the following code is to ensure that the sorted array doesn't start with zero.
    for (p=0; p<maxNumOfIndSeq; p++)
    {
        if (busRoute[index].indSequence[p] != 0)
        {
            nonzeroIndex = p;
            break;
        }
    }

    if (nonzeroIndex > 0)
    {
        for (p= nonzeroIndex; p<maxNumOfIndSeq; p++)
        {
            busRoute[index].indSequence[p-nonzeroIndex] = busRoute[index].indSequence[p];
            busRoute[index].stopPointID[p-nonzeroIndex] = busRoute[index].stopPointID[p];
            busRoute[index].timeAPR[p-nonzeroIndex] = busRoute[index].timeAPR[p];
        }

        for (p=(maxNumOfIndSeq-1); p > (maxNumOfIndSeq-1-nonzeroIndex); p--)
        {
            busRoute[index].indSequence[p] = 0;
            busRoute[index].stopPointID[p] = 0;
            busRoute[index].timeAPR[p] = 0;
        }

    }

}



int findingUniqueSequences(void)
{
    bool newUniqueSeq;
    int i=0, j;
    int numOfUniqueSeq = 0, numOfStopsMatched = 0;

    startNode = malloc(sizeof(UniqueSeq)); //Pointer to the first node of linked list.
    //Declaring other pointers:
    UniqueSeqPtr prevNode, currentNode, pointNode;

    //Filling the first node with data:
    startNode->uniqueSequenceIndex = i;
    startNode->frequency = 1;
    startNode->nextSeq = NULL;
    numOfUniqueSeq++;
    prevNode = startNode;

    for (i=1; i<uniqueTrips; i++)
    {
        newUniqueSeq = true;
        currentNode = startNode;

        while (currentNode!=NULL)
        {
            numOfStopsMatched = 0;
            for (j=0; j < maxNumOfIndSeq; j++)
            {
                if (busRoute[currentNode->uniqueSequenceIndex].stopPointID[j] == busRoute[i].stopPointID[j])
                {
                    numOfStopsMatched++;
                }
            }
            if (numOfStopsMatched == maxNumOfIndSeq)
            {
                newUniqueSeq = false;
                break;
            }
            currentNode = currentNode->nextSeq;
        }

        if (newUniqueSeq == true) //If unique sequence found:
        {
            UniqueSeqPtr newNode = malloc(sizeof(UniqueSeq)); //create new node.
            prevNode->nextSeq = newNode;
            newNode->uniqueSequenceIndex = i;
            newNode->frequency = 1;
            newNode->nextSeq = NULL;
            numOfUniqueSeq++;
            prevNode = newNode;
        }
        else
        {
            currentNode->frequency++;
        }
    }

    prevNode->nextSeq = NULL;

    //Sorting contents of linked lists: (used selection sort)
    currentNode = startNode;
    UniqueSeqPtr nextNode;
    UniqueSeqPtr maxValueNode;
    UniqueSeqPtr swapNode = malloc(sizeof(UniqueSeq));

    while(currentNode->nextSeq != NULL)
    {
        maxValueNode = currentNode;

        nextNode = currentNode->nextSeq;
        while (nextNode != NULL)
        {
            if (maxValueNode->frequency < nextNode->frequency) {
                maxValueNode = nextNode;
            }
            nextNode = nextNode->nextSeq;
        }

        swapNode->frequency = currentNode->frequency;
        swapNode->uniqueSequenceIndex = currentNode->uniqueSequenceIndex;

        currentNode->frequency = maxValueNode->frequency;
        currentNode->uniqueSequenceIndex = maxValueNode->uniqueSequenceIndex;

        maxValueNode->frequency = swapNode->frequency;
        maxValueNode->uniqueSequenceIndex = swapNode->uniqueSequenceIndex;

        currentNode = currentNode->nextSeq;
    }



    currentNode = startNode;
    int n = 1;
    while(currentNode != NULL) //Finding the correct sequence.
    {

        busRoute[currentNode->uniqueSequenceIndex].stopPointID[maxNumOfIndSeq] = 0;

        if (currentNode->frequency > maxFreq)
        {
            maxFreq = currentNode->frequency;
        }
        currentNode = currentNode->nextSeq;
        n++;
    }

    return numOfUniqueSeq;
}



int initialChecks(void)
{
    UniqueSeqPtr currentNode;
    currentNode = startNode->nextSeq;
    int* testSequence;
    int numOfRepairedTrips = 0, repairResult;
    int i = 1;
    while (currentNode != NULL)
    {
        printf("Other Sequence %d. Occurs %d times.\n", i, currentNode->frequency);
        testSequence = busRoute[currentNode->uniqueSequenceIndex].stopPointID; //making a copy of the incorrect sequence
        repairResult = repair(busRoute[startNode->uniqueSequenceIndex].stopPointID, testSequence, numOfStopsIDsIncorrectSeq);
        if (repairResult == 1) //If sequence can be repaired:
        {
            numOfRepairedTrips += currentNode->frequency;
        }
        currentNode = currentNode->nextSeq;
        printf("\n");
        i++;
    }

    return numOfRepairedTrips;
}


int repair(int* correctSequence, int* incorrect_sequence, int sizeOfCorrectSeq) {
    int* otherSequence = incorrect_sequence;
    bool finalRepair;
    bool stopFound = false;
    bool repairByDrop = false;
    int droppedStops[sizeOfCorrectSeq]; //An array to store stops to be deleted.
    int problemStop, numOfStopsToDelete = 0;
    int i, j, m, k=0;


    for (int e=0; otherSequence[e] != 0; e++)
    {
        printf("%d ", otherSequence[e]);
    }
    printf("\n");


    //Deleting all additional stops in the incorrect_sequence:
    for(i=0, j=0; correctSequence[i] != 0 && incorrect_sequence[j] != 0;j++)
    {
        stopFound = false;
        if (i == sizeOfCorrectSeq)
        {
            i--;
        }
        if (correctSequence[i] != incorrect_sequence[j])
        {
            m = i;
            m++;
            while (correctSequence[m] != 0)
            {
                if (correctSequence[m] == incorrect_sequence[j])
                {
                    stopFound = true;
                    break;
                }
                m++;
            }
            if (stopFound == false)
            {
                numOfStopsToDelete++;
                problemStop = incorrect_sequence[j];
                droppedStops[k] = problemStop;
                int n;
                for(n=j; incorrect_sequence[n] != 0; n++) //deleting stop:
                {
                    incorrect_sequence[n] = incorrect_sequence[n+1];
                }
                k++;
                j--;
            }
            i--;
        }
        i++;
        if (i == sizeOfCorrectSeq)
        {
            i--;
        }
    }

    if (numOfStopsToDelete != 0)
    {
        repairByDrop = true;
        int prevStop = droppedStops[0];
        int temp;
        droppedStops[0] = numOfStopsToDelete; //Making the first element in the array to be the total number of stops to be dropped, in order to make printing of the stops easier.
        for(int y=1; y<sizeOfCorrectSeq-1; y++) //Adjusting values in array:
        {
            temp = droppedStops[y];
            droppedStops[y] = prevStop;
            prevStop = temp;
        }
    }


    //Adding all missing stops in the incorrect_sequence:
    bool canBeRepaired = true;
    bool repairByInterpolating = false;
    int interpolateStops[sizeOfCorrectSeq]; //An array to store stops to be added.
    int numOfStopsToAd = 0;
    int n;

    n = 0;
    for (i=0, j=0; correctSequence[i] != 0 && incorrect_sequence[j] != 0; i++, j++)
    {
        if (correctSequence[i] != incorrect_sequence[j])
        {
            m = i;
            m++;

            if (correctSequence[m] != incorrect_sequence[j]) //if the next stop in the comparison is not equal, then the sequence cannot be repaired, as this will result in two consecutive stop missing.
            {
                canBeRepaired = false;
                break;
            }
            else
            {
                interpolateStops[n] = correctSequence[i]; //StopID to add!
                n++;
                numOfStopsToAd++;
                repairByInterpolating = true;
                int currentStop = correctSequence[i];
                int nxtStop;
                k = j;
                int zeroCharacter = 0;
                while(zeroCharacter <= 1) //Adding stop in the sequence.
                {
                    nxtStop = incorrect_sequence[k];
                    incorrect_sequence[k] = currentStop;
                    currentStop = nxtStop;
                    k++;
                    if (incorrect_sequence[k] == 0)
                    {
                        zeroCharacter++;
                    }
                }
            }
        }
    }

    if (correctSequence[i] != 0 && sizeOfCorrectSeq - i == 1) //If the last stop according to the correct sequence is missing:
    {
        interpolateStops[n] = correctSequence[i]; //Add stopId!
        numOfStopsToAd++;
        repairByInterpolating = true;
        int currentStop = correctSequence[i];
        int nxtStop;
        k = j;
        int zeroCharacter = 0;
        while(zeroCharacter <= 1) //Adding stop in the sequence.
        {
            nxtStop = incorrect_sequence[k];
            incorrect_sequence[k] = currentStop;
            currentStop = nxtStop;
            k++;
            if (incorrect_sequence[k] == 0)
            {
                zeroCharacter++;
            }
        }
    }


    if (correctSequence[i] != 0 && sizeOfCorrectSeq - i > 1) //If more than one end stop is missing then the sequence cannot be repaired.
    {
        canBeRepaired = false;
    }

    if (repairByInterpolating == true)
    {
        int prevStop = interpolateStops[0];
        int temp;
        interpolateStops[0] = numOfStopsToAd; //Making the first element in the array to be the total number of stops to be dropped, in order to make printing of the stops easier.
        for(int y=1; y<(numOfStopsToAd + 1); y++) //Adjusting values in array:
        {
            temp = interpolateStops[y];
            interpolateStops[y] = prevStop;
            prevStop = temp;
        }
    }

    if (canBeRepaired == false)
    {
        finalRepair = false;
    }
    else
    {
        finalRepair = true;
    }


    //printing results:
    if (finalRepair == true)
    {
        printf("Can be repaired ;) ");

        if (repairByDrop == true) {
            printf("by dropping stop(s) ");
            for (int g=1; g < (droppedStops[0] + 1); g++) {
                printf("%d ", droppedStops[g]);
            }
        }

        if (repairByDrop == true && repairByInterpolating == true) {
            printf("and ");
        }

        if (repairByInterpolating == true) {
            printf("by interpolating stop(s) ");
            for (int g=1; g < (interpolateStops[0] + 1); g++) {
                printf("%d ", interpolateStops[g]);
            }
        }
        printf("\n");
        return 1;
    }
    else {
        printf("Can't be repaired :( \n");
        return 0;
    }

}


