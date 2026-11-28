#include "physics.h"
#include "util.h"

// Movement validation and resource adjustments without structures


// Adds points to the player's score when he does something worth points
void applyScoreDelta(int& score,int deltaScore){
    score=score+deltaScore; // Adds the score
}


// Changes the energy and time and then uses clampInt to  make sure neither goes below 0 or above the maximum.
void applyResourceDelta(int& energy, int& time, int deltaEnergy, int deltaTime){
    energy = energy + deltaEnergy; // Adds the energy
    time = time + deltaTime; //Adds the time
    energy = clampInt(energy,0,MAX_ENERGY); // Range Check
    time = clampInt(time,0,MAX_TIME); // Range Check
}

// Searches player's inventory for an item
bool hasItem(const char inventory[MAX_INVENTORY_CAPACITY], int inventoryCount, char itemSymbol){
    // inventoryCount stores the number of items in the bag
    for (int i=0;i<inventoryCount;i++){ 
        if(inventory[i]==itemSymbol){ //Checks each item in inventory
            return true;
        }
    }
    return false;
}


// Adds items to the inventory if space exists
// Inventory is an array
// MAX_INVENTORY_CAPACITY holds the number of boxes in the array, it is set to 10 in config.h
bool addItem(char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount, char itemSymbol) {
    if (inventoryCount >=MAX_INVENTORY_CAPACITY) {     // Checks if space exists, if the bag is full it returns false
        return false;
    }
    inventory[inventoryCount] = itemSymbol;     // Puts the item in the next empty slot
    inventoryCount++;                             // Increments the inventoryCount
    return true;
}


// Removes item from inventory and then it closes the gap
bool removeItem(char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount, char itemSymbol){
    int foundAt= -1; // -1 shows item is not found yet

    // Step 1: first find the slot of the item
    for (int i = 0; i < inventoryCount; i++) {
        if (inventory[i] == itemSymbol) {
            foundAt = i;               // Holds the position where it was found
            break;
        }
    }
    // Step 2: we can't remove the item if never found
    if (foundAt == -1) {
        return false;
    }
    
    // Step 3: every item after that moves one step left
    for (int i = foundAt; i < inventoryCount - 1; i++) {
        // i starts at foundAt so that the hole/gap can be replaced by the box on its right
        inventory[i] = inventory[i+1];   // Box i gets a copy of the box to its right
    }

    // Step 4: there is one item less in the bag after that
    inventoryCount--;                                
    return true;
}

// Tells if the player is allowed to use a connection
bool canTraverse(int targetFloor, int targetRoom, char connType, const char inventory[MAX_INVENTORY_CAPACITY], int inventoryCount, int emergencyTripsLeft, bool lift2Operational){
    
    // Checks if target floor exists
    if ( targetFloor< 1 || targetFloor > NUM_FLOORS ){
        return false;
    }
    // Checks if target room exists
    if ( targetRoom< 1 || targetRoom > ROOMS_PER_FLOOR ){
        return false;
    }

    // Windows, doors and stairs always remain open
    if (connType == CONN_DOOR || connType == CONN_WINDOW || connType == CONN_STAIRS){
        return true;
    }

    // Lift 1 is always open/works
    if (connType == CONN_LIFT1){
        return true;
    }
    // Locked door: Open only when player has a master key
    if (connType == CONN_LOCKED_DOOR){
        return hasItem(inventory, inventoryCount, ENT_MASTER_KEY);
    }

    // Lift 2: if repaired it works, or if emergency trips are left
    if (connType == CONN_LIFT2){
        if (lift2Operational ||  emergencyTripsLeft > 0 ) {       // Checks if lift is operational or emergency trips are left
            return true;
        }
        return false;
    }
    // Wall or anything unknown entered: Not allowed!
    return false;
}

// Finds out if two rooms are connected and what connects them (door, window, stairs, lift )
//horizontalConn stores the coneections between the rooms on the same floor
// ROOMS_PER_FLOOR - 1 is done bcz the connections are always less than the rooms e.g 4 rooms have 3 gaps b/w them
bool isDirectlyConnected(int fromF, int fromR, int toF, int toR, const char horizontalConn[NUM_FLOORS][ROOMS_PER_FLOOR - 1], char& outConnType){
    
    outConnType=CONN_NONE; // Initialised with no connection
    
    // Safety check: Checks if the floors exist
    if (fromF < 1 || fromF > NUM_FLOORS || toF < 1 || toF > NUM_FLOORS){
        return false;
    }
    // Safety check: Checks if the rooms exist
    if (fromR < 1 || fromR > ROOMS_PER_FLOOR || toR < 1 || toR > ROOMS_PER_FLOOR ){
        return false;
    }

    // Part 1: Both rooms exist on the same floor
    if (fromF == toF){
        int leftRoom = -1; // Holds the room on the left side of the gap
        
        if (toR == fromR + 1){
            leftRoom = fromR;          // moving right, so the left room is where we are now
        }
        if (toR == fromR - 1){
            leftRoom = toR;            // moving left, so the left room is where we're going
        }
        if (leftRoom == -1){
            return false;              // rooms are not side by side, so no path exists 
        }
        outConnType = horizontalConn[fromF-1][leftRoom-1];   // -1 bcz arrays start from index 0 (Floor 1 = row 0, Room 1 = gap 0)
        if (outConnType == CONN_NONE || outConnType == CONN_SOLID_WALL){
            return false;              // Empty gap, or a wall
        }
        return true; //Door, window or locked door found
    }

    // Part 2: Checks if stairs connect the rooms (stairs are only in Room 1)
    if (fromR == 1 && toR == 1){
        int lowerFloor = fromF; // Holds the lower of the two floors
        if (toF < fromF){
            lowerFloor = toF; // We're going down, so the lower floor is where we are going
        }
        bool nextFloor = (fromF - toF == 1 || toF - fromF == 1); // Only true if the floors are right above or below each other
        if (nextFloor && lowerFloor != 3){ // There is no staircase b/w floor 3 and 4
            outConnType = CONN_STAIRS; // Stairs Connect them
            return true;
        }
    }
    // Part 3: Checks if Lift 1 connects the rooms (only between Floors 1 and 2 and from Room 2 or 3)
    bool lift1Floors = (fromF == 1 && toF == 2) || (fromF == 2 && toF == 1);   // True only when you are moving between Floor 1 and 2
    bool lift1Rooms  = (fromR == 2 || fromR == 3) && (toR == 2 || toR == 3);   // True only when both rooms are next to Lift 1 (Room 2 or 3)
    if (lift1Floors && lift1Rooms){
        outConnType = CONN_LIFT1;      // Lift 1 connects them
        return true;
    }

    // Part 4: Checks if Lift 2 connects the rooms (only Room 4 on Floors 2, 4 and 5. Floor 3 is skipped)
    bool fromLift2Floor = (fromF == 2 || fromF == 4 || fromF == 5);   // True only when starting floor has a Lift 2 entrance
    bool toLift2Floor   = (toF == 2 || toF == 4 || toF == 5);         // True only when target floor has a Lift 2 exit
    if (fromR == 4 && toR == 4 && fromLift2Floor && toLift2Floor){   //  Lift 2 is in Room 4 only,so both rooms have to be Room 4 on Floor 2, 4 or 5
        outConnType = CONN_LIFT2;      // Lift 2 connects them
        return true;
    }
    // No window, stairs, door or lift connects these rooms
    return false;
}







