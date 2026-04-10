
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct Org_struct {
    char name[20];
    int* prey; //dynamic array of indices  
    int numPrey;
} Org;


/*
Purpose: Add a new organism to the end of the food web
Parameter: 
- pWeb - a pointer to a dynamically allocated array of Orgs; i.e. a pointer to a pointer
- pNumOrgs - a passed-by-pointer parameter for the number of organisms = size of web[]
- newOrgName - a character array (i.e. string) for the name of the new organism 
Return: (void)
Note: 
- Uses malloc to create a new larger array, then modify that and assign the new array with the new size
*/
void addOrgToWeb(Org** pWeb, int* pNumOrgs, char* newOrgName) {
    //Allocate a new web array with 1 extra slot.
    Org* newWeb = (Org*)malloc(sizeof(Org) * ((*pNumOrgs) + 1));
    //Copy into the new array
    for (int i = 0; i < *pNumOrgs; i++){
        newWeb[i] = (*pWeb)[i];
    }
    //free the old web array
    free(*pWeb);

    //Appended organism at the end of web
    strcpy(newWeb[*pNumOrgs].name, newOrgName);
    newWeb[*pNumOrgs].prey = NULL;
    newWeb[*pNumOrgs].numPrey = 0;

    *pWeb = newWeb;
    *pNumOrgs = (*pNumOrgs) + 1;
}





/*
Purpose: Add a predator -> prey relationsip to the web
Parameter:
- web - a dynamically allocated array of Orgs, i.e. a pointer to an Org
- numOrgs - number of organisms = size of web[], passed-by-value
- predInd - predator index in web[]; an entry to its prey[] subitem will be added
- preyInd - prey index in web[]; will be added to predator's prey[] subitem
Return:
- true if the relation was added
- false if the relation was not added (invalid indices or duplicate)
*/
bool addRelationToWeb(Org* web, int numOrgs, int predInd, int preyInd) {
    
    

    //Validate indices checking
    if(predInd < 0 || predInd >= numOrgs || preyInd < 0 || preyInd >= numOrgs || predInd == preyInd){
        printf("Invalid predator and/or prey index. No relation added to the food web.\n");
        return false;
    }

    //Check for duplicates
    for(int i = 0; i < web[predInd].numPrey; i++){
        if(web[predInd].prey[i] == preyInd){
            printf("Duplicate predator/prey relation. No relation added to the food web.\n");
            return false;
        }
    }

    //Case 1: IF predator's list is null
    if(web[predInd].prey == NULL){
        web[predInd].prey = (int*)malloc(sizeof(int)*1);
        web[predInd].prey[0] = preyInd;
        web[predInd].numPrey = 1;
    //Case 2: If predator already has at least 1 prey
    } else{
        int* newPrey = (int*)malloc(sizeof(int)*(web[predInd].numPrey + 1));

        for(int i = 0; i < web[predInd].numPrey; i++){
            newPrey[i] = web[predInd].prey[i];
        }

        free(web[predInd].prey);

        newPrey[web[predInd].numPrey] = preyInd;

        web[predInd].prey = newPrey;
        web[predInd].numPrey = (web[predInd].numPrey + 1);
    }
    return true;
}


/*
Purpose: Remove one organism from the web and re-link all prey indices
Parameter:
- pWeb - a pointer to a dynamically allocated array of Orgs; i.e. a pointer to a pointer
- pNumOrgs - a passed-by-pointer parameter for the number of organisms = size of web[]
- index - organism index in the (*pWeb)[] array to remove
Return:
- true if removal succeeds
- false if index invalid
*/
    
bool removeOrgFromWeb(Org** pWeb, int* pNumOrgs, int index) {
    //Checking extinction index is valid
    if (index < 0 || index >= *pNumOrgs){
        return false;
    }
    
    /*
    Part 1: For every organism, update it's prey list
        - remove any occurrence of extinct index
        - decrement any prey indices that were larger than the extinct organism index
    */
    for(int i = 0; i < *pNumOrgs; i++){

        if(i == index) continue; //skip the organism that going extinct
        if((*pWeb)[i].numPrey == 0) continue; // skip the producer

        int size = (*pWeb)[i].numPrey;

        //Checking if the extinct index appear in the prey list
        bool found = false;
        for (int j = 0; j < size; j++){
            if((*pWeb)[i].prey[j] == index){
                found = true;
                break;
            }
        }

        //If not found still have to shift the index and do not resize it
        if (!found){
            for (int j = 0; j < size; j++){
                if((*pWeb)[i].prey[j] > index){
                    (*pWeb)[i].prey[j]--;
                }
            }
            continue;
        }

        //Count how many prey will remain after the removing
        int newCount = 0;
        for (int j = 0; j < size; j++){
            if ((*pWeb)[i].prey[j] != index){
                newCount++;
            }
        }

        //IF the prey list empty after remove, and set to NULL
        if (newCount == 0){
            free((*pWeb)[i].prey);
            (*pWeb)[i].prey = NULL;
            (*pWeb)[i].numPrey = 0;
            continue;
        }
        
        //Remove the index
        int* newPrey = (int*)malloc(sizeof(int) * newCount);

        int removeIndex = 0;
        for (int j = 0; j < size; j++){
            if(index == (*pWeb)[i].prey[j]){
                continue;
            }
            if(index < (*pWeb)[i].prey[j]){
                (*pWeb)[i].prey[j] = (*pWeb)[i].prey[j] - 1;
            }
            newPrey[removeIndex] = (*pWeb)[i].prey[j];
            removeIndex++;
        }

        free((*pWeb)[i].prey);

        (*pWeb)[i].prey = newPrey;
        (*pWeb)[i].numPrey = newCount;
            
    }



    /*
    Part 2: Remove the exti organism from the web array
    */


    //If the web has only one organism, extinct make the web empty
    if(*pNumOrgs == 1){
        free((*pWeb)[0].prey);
        free(*pWeb);
        *pWeb = NULL;
        *pNumOrgs = 0;
        return true;
    } else { //If the web has at least one organism, resize and reasign the web

        Org* newWeb = (Org*)malloc(sizeof(Org) * ((*pNumOrgs) - 1));
        int pIndex = 0;
        for (int i = 0; i < *pNumOrgs; i++){
            if(i == index){
                continue;
            }
            newWeb[pIndex] = (*pWeb)[i];
            pIndex++;
        }


        free((*pWeb)[index].prey);
        (*pWeb)[index].prey = NULL;
        (*pWeb)[index].numPrey = 0;
        free(*pWeb);

        *pWeb = newWeb;
        *pNumOrgs = (*pNumOrgs) - 1;

        return true;
    }
    

}

/*
Purpose: Free all heap memory
Parameter: 
- web: pointer to dynamic Org array
- numOrgs: number of elements in web
Return: void
*/
void freeWeb(Org* web, int numOrgs) {
    //Free all heap memory for each prey list
    for (int i = 0; i < numOrgs; i++){
        free(web[i].prey);
    }
    //Free all heap memory for each organism
    free(web);

}

/*
Purpose: Print the web
Parameter: 
- web: dynamic Org array
- numOrgs: number of Organisms
Returns: void
*/

void printWeb(Org* web, int numOrgs) {

    for (int i = 0; i < numOrgs; i++){
        printf("  (%d)", i);
        printf(" %s", web[i].name);
        if (web[i].numPrey != 0){
            printf(" eats ");
            //Print each prey name 
            for (int j = 0; j < web[i].numPrey; j++){
                printf("%s", web[web[i].prey[j]].name);
                // Add commas between prey names, but not after the last prey.
                if(j < web[i].numPrey - 1){
                    printf(", ");
                }
            }
        }
        printf("\n");
    }
}


/*
Purpose: Print all apex predators
Parameter:
- web: array of organism
- numOrgs: number of organism in web[]
Return: void
*/
void printApexPred(Org* web, int numOrgs){
    for(int predIndex = 0; predIndex < numOrgs; predIndex++ ){
        int eatenCount = 0; // counts how many pred eat predIndex

        //Scan every organism's prey list to see if it eats predIndex
        for (int eatAnimal = 0; eatAnimal < numOrgs; eatAnimal++){
            for (int i = 0; i < web[eatAnimal].numPrey; i++){
                if(web[eatAnimal].prey[i] == predIndex){
                    eatenCount++;
                }
            }
        }
        //If nobody eats it, then apex pred
        if (eatenCount == 0){
            printf("  %s", web[predIndex].name);
        }

    }
}

/*
Purpose: Print all producer
Parameter:
- web: array of organism
- numOrgs: number of organism in web[]
Return: void
*/
void printProducers(Org* web, int numOrgs){
    for (int i = 0; i < numOrgs; i++){
        if(web[i].numPrey == 0){
            printf("  %s", web[i].name);
        }
    }
}

/*
Purpose: Print all apex predators
Parameter:
- web: array of organism
- numOrgs: number of organism in web[]
Return: void
*/
void printMostFlexibleEaters(Org* web, int numOrgs){
    int max = 0;
    //Find the maxium numPrey in the web
    for (int i = 0; i < numOrgs; i++){
        if(web[i].numPrey > max){
            max = web[i].numPrey;
        }

    }
    // Print all Organisms that match the max
    for (int i = 0; i < numOrgs; i++){
        if(web[i].numPrey == max){
             printf("  %s", web[i].name);
        }
    }
   
}


/*
Purpose: Print organism eaten by the most other organisms
Parameter:
- web: array of organism
- numOrgs: number of organism in web[]
Return: void
*/
void printTatiestFood(Org* web, int numOrgs){
    int maxEaten = 0; // Max times eaten found
    int count[numOrgs]; // count[i] = times organism i is eaten

    //Count how often each organism is eaten
    for(int predIndex = 0; predIndex < numOrgs; predIndex++ ){
        int eatenCount = 0;

        //Scan all prey lists and count occurrence of predIndex
        for (int eatAnimal = 0; eatAnimal < numOrgs; eatAnimal++){
            for (int i = 0; i < web[eatAnimal].numPrey; i++){
                if(web[eatAnimal].prey[i] == predIndex){
                    eatenCount++;
                }
            }
        }
        count[predIndex] = eatenCount;
        if ( eatenCount > maxEaten){
            maxEaten = eatenCount;
        }

    }

    // Print all organisms tied for maxEaten
    for(int i = 0; i < numOrgs; i++){
        if(count[i] == maxEaten){
            printf("  %s", web[i].name);
        }
    }
}



/*
Purpose: Compute and print a "height" for each organism
Parameter:
- web: array of organism
- numOrgs: number of organism in web[]
Return: void
*/
void printFoodHeight(Org* web, int numOrgs){
    int heightArr[numOrgs]; // heightArr[i] = current height of organism
    // Initialize all heights to 0
    for(int i = 0; i < numOrgs; i++){
        heightArr[i] = 0;
    }

    bool flagChange = true; //tracks whether a pass changed any heights
    while(flagChange){
        flagChange = false;

        
        for(int orgIndex = 0; orgIndex < numOrgs; orgIndex++){
            int tempMaxHeight = 0;

            //Producer stay at height 0
            if(web[orgIndex].numPrey == 0){
                continue;
            }

            // Find tallest prey height for this organism
            for(int preyIndex = 0; preyIndex < web[orgIndex].numPrey; preyIndex++){
                if(heightArr[web[orgIndex].prey[preyIndex]] > tempMaxHeight){
                    tempMaxHeight = heightArr[web[orgIndex].prey[preyIndex]];
                }
            }

            //Calculate new height
            int heightCalc = tempMaxHeight + 1;

            //Update if we found a higher value
            if (heightCalc > heightArr[orgIndex]){
                heightArr[orgIndex] = heightCalc;
                flagChange = true;

            }
        }
    }
   
    // Print final heights
    for(int i = 0; i < numOrgs; i++){
        printf("  %s: %d\n", web[i].name, heightArr[i]);
    }
}



/*
Purpose: Classify organisms into producers, herbivores, omnivores, and carnivores
Parameter:
- web: array of organism
- numOrgs: number of organism in web[]
Return: void
*/
void printVoreType(Org* web, int numOrgs){
    printf("  Producers:\n");
    for (int i = 0; i < numOrgs; i++){
        if (web[i].numPrey == 0){
            printf("  %s\n", web[i].name);
        }
    }

    printf("  Herbivores:\n");
    for(int i = 0; i < numOrgs; i++){
        if (web[i].numPrey == 0) continue;

        bool herbivoresFlag = true;
        for(int j = 0; j < web[i].numPrey; j++){
            if(web[web[i].prey[j]].numPrey > 0){
                herbivoresFlag = false;
                break;
            }

            
        }
        if(herbivoresFlag){
            printf("  %s\n", web[i].name);
        }
    }


    printf("  Omnivores:\n");
    for (int i = 0; i < numOrgs; i++){
        if (web[i].numPrey == 0) continue;

        bool producerFlag = false;
        bool nonProducerFlag = false;
        for(int j = 0; j < web[i].numPrey; j++){
            if(web[web[i].prey[j]].numPrey > 0){
                nonProducerFlag = true;
            }
            if(web[web[i].prey[j]].numPrey == 0){
                producerFlag = true;
            }
        }
        if(producerFlag && nonProducerFlag){
            printf("  %s\n", web[i].name);
        }

    }

    printf("  Carnivores:\n");
    for(int i = 0; i < numOrgs; i++){
        if (web[i].numPrey == 0) continue;

        bool carnivoresFlag = true;
        for(int j = 0; j < web[i].numPrey; j++){
            if(web[web[i].prey[j]].numPrey == 0){
                carnivoresFlag = false;
                break;
            }

            
        }
        if(carnivoresFlag){
            printf("  %s\n", web[i].name);
        }
    }
}

/*
Purpose: Display the full web plus all computed characteristics
Parameter:
- web: array of organism
- numOrgs: number of organism in web[]
- modified: if true, prints "UPDATED" before each section title
Return: void
*/

void displayAll(Org* web, int numOrgs, bool modified) {

    if (modified) printf("UPDATED ");
    printf("Food Web Predators & Prey:\n");
    printWeb(web,numOrgs); 
    printf("\n");

    if (modified) printf("UPDATED ");
    printf("Apex Predators:\n");
    printApexPred(web, numOrgs);
    printf("\n");



    if (modified) printf("UPDATED ");
    printf("Producers:\n");
    printProducers(web, numOrgs);
    printf("\n");


    if (modified) printf("UPDATED ");
    printf("Most Flexible Eaters:\n");
    printMostFlexibleEaters(web, numOrgs);
    printf("\n");



    if (modified) printf("UPDATED ");
    printf("Tastiest Food:\n");
    printTatiestFood(web, numOrgs);
    printf("\n");


    if (modified) printf("UPDATED ");
    printf("Food Web Heights:\n");
    printFoodHeight(web, numOrgs);
    printf("\n");



    if (modified) printf("UPDATED ");
    printf("Vore Types:\n");
    printVoreType(web, numOrgs);
    printf("\n");

}


/*
Purpose: Parse command-line flags and set mode variables
Parameter:
- argc: argument count
- argv: argument array
- pBasicMode: address of basicMode flag
- pDebugMode: address of debugMode flag
- pQuietMode: address of quietMode flag
Return:
- true if all arguments are valid and non-duplicate
-flase if any invalid or duplicate argument
*/
bool setModes(int argc, char* argv[], bool* pBasicMode, bool* pDebugMode, bool* pQuietMode) {
    *pBasicMode = false;
    *pDebugMode = false;
    *pQuietMode = false;

    //Process each command-line token after program name
    for(int i = 1; i < argc; i++){
        //Check if argv[i] is not null, must start with '-' and at least 2 chars
        if(argv[i] == "" || strlen(argv[i]) < 2 || argv[i][0] != '-'){
            return false;
        }


        // Use argv[i][1] to identify which mode flag
        if(argv[i][1] =='b'){
            if(*pBasicMode == true){
                return false;
            }
            *pBasicMode = true;
        } else if(argv[i][1] =='d'){
            if (*pDebugMode == true){
                return false;
            }
            *pDebugMode = true;
        } else if(argv[i][1] =='q'){
            if(*pQuietMode == true){
                return false;
            }
            *pQuietMode = true;
        } else {
            return false;
        }


    }
    
    return true; // update this return statement
    

}


void printONorOFF(bool mode) {
    if (mode) {
        printf("ON\n");
    } else {
        printf("OFF\n");
    }
}


int main(int argc, char* argv[]) {  
    

    bool basicMode = false;
    bool debugMode = false;
    bool quietMode = false;

    if (!setModes(argc, argv, &basicMode, &debugMode, &quietMode)) {
        printf("Invalid command-line argument. Terminating program...\n");
        return 1;
    }

    printf("Program Settings:\n");
    printf("  basic mode = ");
    printONorOFF(basicMode);
    printf("  debug mode = ");
    printONorOFF(debugMode);
    printf("  quiet mode = ");
    printONorOFF(quietMode);
    printf("\n");

    
    int numOrgs = 0;
    printf("Welcome to the Food Web Application\n\n");
    printf("--------------------------------\n\n");

    Org* web = NULL;

    printf("Building the initial food web...\n");
    
    if (!quietMode) printf("Enter the name for an organism in the web (or enter DONE): ");
    char tempName[20] = "";
    scanf("%s",tempName); 
    if (!quietMode) printf("\n");
    while (strcmp(tempName,"DONE") != 0) {
        addOrgToWeb(&web,&numOrgs,tempName);
        if (debugMode) {
            printf("DEBUG MODE - added an organism:\n");
            printWeb(web,numOrgs);
            printf("\n");
        }
        if (!quietMode) printf("Enter the name for an organism in the web (or enter DONE): ");
        scanf("%s",tempName); 
        if (!quietMode) printf("\n");
    }
    if (!quietMode) printf("\n");

    if (!quietMode) printf("Enter the pair of indices for a predator/prey relation.\n");
    if (!quietMode) printf("Enter any invalid index when done (-1 2, 0 -9, 3 3, etc.).\n");
    if (!quietMode) printf("The format is <predator index> <prey index>: ");
        
    int predInd, preyInd;
    scanf("%d %d",&predInd, &preyInd);
    if (!quietMode) printf("\n");

    while (predInd >= 0 && preyInd >= 0 && predInd < numOrgs &&  preyInd < numOrgs && predInd != preyInd) {
        addRelationToWeb(web,numOrgs,predInd,preyInd);
        if (debugMode) {
            printf("DEBUG MODE - added a relation:\n");
            printWeb(web,numOrgs);
            printf("\n");
        }
        if (!quietMode) printf("Enter the pair of indices for a predator/prey relation.\n");
        if (!quietMode) printf("Enter any invalid index when done (-1 2, 0 -9, 3 3, etc.).\n");
        if (!quietMode) printf("The format is <predator index> <prey index>: ");
        
        scanf("%d %d",&predInd, &preyInd);  
        if (!quietMode) printf("\n");
    }
    printf("\n");

    printf("--------------------------------\n\n");
    printf("Initial food web complete.\n");
    printf("Displaying characteristics for the initial food web...\n");
    
    displayAll(web,numOrgs,false);

    if (!basicMode) {
        printf("--------------------------------\n\n");
        printf("Modifying the food web...\n\n");
        char opt = '?';

        while (opt != 'q') {
            if (!quietMode) {
                printf("Web modification options:\n");
                printf("   o = add a new organism (expansion)\n");
                printf("   r = add a new predator/prey relation (supplementation)\n");
                printf("   x = remove an organism (extinction)\n");
                printf("   p = print the updated food web\n");
                printf("   d = display ALL characteristics for the updated food web\n");
                printf("   q = quit\n");
                printf("Enter a character (o, r, x, p, d, or q): ");
            }
            scanf(" %c", &opt);
            if (!quietMode) printf("\n\n");

            if (opt == 'o') {
                char newName[20];
                if (!quietMode) printf("EXPANSION - enter the name for the new organism: ");
                scanf("%s",newName);
                if (!quietMode) printf("\n");
                printf("Species Expansion: %s\n", newName);
                addOrgToWeb(&web,&numOrgs,newName);
                printf("\n");

                if (debugMode) {
                    printf("DEBUG MODE - added an organism:\n");
                    printWeb(web,numOrgs);
                    printf("\n");
                }

            } else if (opt == 'x') {
                int extInd;
                if (!quietMode) printf("EXTINCTION - enter the index for the extinct organism: ");
                scanf("%d",&extInd);
                if (!quietMode) printf("\n");
                if (extInd >= 0 && extInd < numOrgs) {
                    printf("Species Extinction: %s\n", web[extInd].name);
                    removeOrgFromWeb(&web,&numOrgs,extInd);
                } else {
                    printf("Invalid index for species extinction\n");
                }
                printf("\n");
                
                if (debugMode) {
                    printf("DEBUG MODE - removed an organism:\n");
                    printWeb(web,numOrgs);
                    printf("\n");
                }

            } else if (opt == 'r') {
                if (!quietMode) printf("SUPPLEMENTATION - enter the pair of indices for the new predator/prey relation.\n");
                if (!quietMode) printf("The format is <predator index> <prey index>: ");
                scanf("%d %d",&predInd, &preyInd);
                if (!quietMode) printf("\n");

                if (addRelationToWeb(web,numOrgs,predInd,preyInd)) {
                    printf("New Food Source: %s eats %s\n", web[predInd].name, web[preyInd].name);
                };
                printf("\n");
                if (debugMode) {
                    printf("DEBUG MODE - added a relation:\n");
                    printWeb(web,numOrgs);
                    printf("\n");
                }

            } else if (opt == 'p') {
                printf("UPDATED Food Web Predators & Prey:\n");
                printWeb(web,numOrgs);
                printf("\n");
                
            } else if (opt == 'd') {
                printf("Displaying characteristics for the UPDATED food web...\n\n");
                displayAll(web,numOrgs,true);

            }
            printf("--------------------------------\n\n");
        
        }
        
    }

    freeWeb(web,numOrgs);

    //printf("\n\n");

    return 0;
}

