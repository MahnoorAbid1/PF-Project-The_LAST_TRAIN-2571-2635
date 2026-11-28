#include "level.h"
#include "util.h"
#include "physics.h" 

// Modular implementation using primitive arrays for map parsing and verification
//Level cppc

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