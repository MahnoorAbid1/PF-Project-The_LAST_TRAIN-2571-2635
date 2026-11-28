#include "level.h"
#include "util.h"
#include "physics.h"
#include <fstream>  //For reading files 
#include <cstring>    //For using strlen which counts the letters in a line
using namespace std;

// Modular implementation using primitive arrays for map parsing and verification
//Level cpp

// Acts as a dictionary; you give it a symbol it gives back the name eg. '<' gives "Master Key"
// char* means that the returned value is an address of a character
// const means the character stored at that address cannot be changed
// Each text is stored as an 1D character array in the memory e.g 700 stores:'M', 701 stores:'e' and so on to make medical kit
// When the function returns it actually gives the address of 'M' which is the starting address of the char array e.g in the example in the line above 700 is returned

const char* getEntityName(char entitySymbol){


    //Checks the symbol against every item, one by one
    if (entitySymbol==ENT_MASTER_KEY) { return "Master Key"; }
    if (entitySymbol==ENT_ENERGY_PACK) { return "Energy Pack"; }
    if (entitySymbol==ENT_EMERGENCY_TOOLKIT) { return "Emergency Toolkit"; }
    if (entitySymbol==ENT_CLUE_4) { return "Clue Digit 4"; }
    if (entitySymbol==ENT_FUSE) { return "Fuse"; }
    if (entitySymbol==ENT_CAT) { return "Cat"; }
    if (entitySymbol==ENT_CONTROL_PANEL) { return "Control Panel"; }
    if (entitySymbol==ENT_LIFT_PASS) { return "Lift Pass"; }
    if (entitySymbol==ENT_PASSENGER) { return "Injured Passenger"; }
    if (entitySymbol==ENT_MEDICAL_KIT) { return "Medical Kit"; }
    if (entitySymbol==ENT_EMERGENCY_CARD) { return "Emergency Access Card"; }
    if (entitySymbol==ENT_CLUE_2) { return "Clue Digit 2"; }
    if (entitySymbol==ENT_CLUE_7) { return "Clue Digit 7"; }
    if (entitySymbol==ENT_CABINET_TRAIN_KEY) { return "Cabinet / Train Key"; }
    if (entitySymbol==ENT_CAT_CARRIER) { return "Cat Carrier"; }
    if (entitySymbol==ENT_OBSERVATION_TOKEN) { return "Observation Token"; }

    // Empty Slot
    if (entitySymbol==ENT_NONE) { return "Nothing"; }

    //None of the above items matches
    return "Unknown item";
}




//Checks if the starting room of the player is valid and has a way out(at least one way out)
//Returns true if okay, else returns false
//horizontalConn stores the connections between the rooms on the same floor
// ROOMS_PER_FLOOR - 1 is done bcz the connections are always less than the rooms e.g 4 rooms have 3 gaps b/w them
bool validateStartingRoom(int startFloor, int startRoom, const char horizontalConn[NUM_FLOORS][ROOMS_PER_FLOOR - 1]){
        
    
    //Safety check: Checks if the floors  and room exist
    if (startFloor < 1 || startFloor > NUM_FLOORS || startRoom < 1 || startRoom > ROOMS_PER_FLOOR){
        return false;
    }

    // Checks every room to find a way out of the room
    for (int FloorNum=1; FloorNum <= NUM_FLOORS ; FloorNum++){
        for (int RoomNum=1 ; RoomNum <= ROOMS_PER_FLOOR ; RoomNum++){
            char ConnectionType; // isDirectlyConnected uses this(not used here)
            if (isDirectlyConnected(startFloor, startRoom, FloorNum, RoomNum ,horizontalConn, ConnectionType)){
                return true; //A way out exists
            }
        }
    }
    // Didn't find a way out
    return false;
}



// It reads the map drawn in building.txt and fills the 3 arrays
// It returns true if all five floors were successfully read, false if not
bool loadBuildingMap(const char* filename, char horizontalConn[NUM_FLOORS][ROOMS_PER_FLOOR - 1], char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM], int entityCounts[NUM_FLOORS][ROOMS_PER_FLOOR]){

    const int MAX_LINE_LENGTH=200; //Longest line we can actually read(The longest line in building.txt is 72 characters; 200 is used just in case)

    // First step: Empty out all the arrays so that they don't hold any garbage value initially
    for ( int f=0; f < NUM_FLOORS ; f++){
        for ( int r=0; r < ROOMS_PER_FLOOR ; r++ ){
            entityCounts[f][r]=0; //Zero items yet
            for ( int s=0; s < MAX_ENTITIES_PER_ROOM ; s++ ){
                roomEntities[f][r][s]=ENT_NONE; //Every slot is empty
            }
            if ( r < ROOMS_PER_FLOOR-1 ){
                horizontalConn[f][r]=CONN_NONE; // A connection doesn't exist yet
            }

        }
    }

    // Second step: Open the file
    ifstream file(filename);
    if (!file.is_open()){
        return false;   // File is missing/ does not exist
    }

    char NameLine[MAX_LINE_LENGTH]; //Line having the room names, e.g. "| F5-R1   D F5-R2..."
    char ItemLine[MAX_LINE_LENGTH]; //Line under it having the items, e.g."| 7 ^   |..."
    int FloorsFound=0;

    // Third step: Read the file line by line until you find all of the 5 floors
    while ( FloorsFound < NUM_FLOORS && file.getline(NameLine, MAX_LINE_LENGTH)){
        
        // Fourth step: Find where each room's name starts e.g F5-R1
        int NameColumn[ROOMS_PER_FLOOR]; //Column of each room's name
        int RoomsFound=0;

        for (int i=0; NameLine[i] !='\0' && RoomsFound < ROOMS_PER_FLOOR; i++){
            // Room name has the following format: F, a digit, -, R
            if (NameLine[i]== 'F' && NameLine[i+1] >= '0' && NameLine[i+1] <= '9' && NameLine[i+2] == '-' && NameLine[i+3] == 'R'){
                NameColumn[RoomsFound]=i; //Remembers where the room starts
                RoomsFound++;
            }
        }

        if (RoomsFound != ROOMS_PER_FLOOR){
            continue; // This line does not have any rooms, skip to the next line
        }

        // Fifth Step: Get the floor number from the name line( it is the digit after 'F')
        int FloorNum=NameLine[NameColumn[0] + 1 ] -'0'; // Converts '5' into 5
        if ( FloorNum < 1 || FloorNum > NUM_FLOORS) {
            file.close();
            return false; // File contains invalid data
        }

        int Row= FloorNum-1; //-1 bcz array starts at 0

        // Sixth Step: Read connections (They are the two places before next room's name)
        for (int Gap=0; Gap < ROOMS_PER_FLOOR-1; Gap++){
            char C= NameLine[NameColumn[Gap+1] - 2];
            if ( C== CONN_DOOR ||  C== CONN_WINDOW ||  C== CONN_LOCKED_DOOR ||  C== CONN_SOLID_WALL){
                horizontalConn[Row][Gap]= C; // In case of Door, Window or locked door or solid wall
            }
            else{
                horizontalConn[Row][Gap]= CONN_NONE; //Something else, e.g. the '1' of "L1" (Lift 1)

            }
        }

        // Seventh Step : Read the item's line( the line under names)
        if ( !file.getline(ItemLine, MAX_LINE_LENGTH)){
            file.close();
            return false; // Item line does not exist
        }

        int ItemLength=strlen(ItemLine); // Tells how many characters the line has
        
        // 8th step: Collects the items which are inside each room's box
        for (int R = 0; R < ROOMS_PER_FLOOR; R++){
            int col = NameColumn[R] - 1;                     //1st column inside this room
            while (col < ItemLength && ItemLine[col] != '|'){   //Stop when room's wall comes
                char C = ItemLine[col];
                if (C != ' ' && C != '\r' && entityCounts[Row][R] < MAX_ENTITIES_PER_ROOM){
                    roomEntities[Row][R][entityCounts[Row][R]] = C;   //Puts item in an empty slot
                    entityCounts[Row][R]++;                             // One more item is added to the room
                }
                col++;                                    //Move on to the next column
            }
        }

        FloorsFound++;                                    //This particular floor is done
    }

    //Step 9: Close the file
    file.close();
    return FloorsFound == NUM_FLOORS;                     //True only if all 5 floors were successfully read
}

