#include "simulator.h"
#include "physics.h"
#include "switches.h"
#include "logger.h"
#include "viewer_iface.h"
#include "util.h"
#include <iostream>
#include <iomanip>

using namespace std;

void runSimulation(int& playerFloor, int& playerRoom,
                   int& energy, int& time, int& score,
                   char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount,
                   bool objectives[NUM_OBJECTIVES], int& emergencyTripsLeft,
                   char horizontalConn[NUM_FLOORS][ROOMS_PER_FLOOR - 1],
                   char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM],
                   int entityCounts[NUM_FLOORS][ROOMS_PER_FLOOR]){
    char command[20]; //the command word is typed here
    bool running = true; //game continues til this is true

    while (running) {
        renderDashboard(playerFloor, playerRoom, energy, time, score,
                        inventory, inventoryCount, emergencyTripsLeft); //current status
        cout << "> "; //tells the user to type

        if (!(cin >> setw(20) >> command)) { //takes command input and only allows 19 char so command doesnt overflow
            break;                      // when keyboard input has stopped, loop breaks
        }

        bool lift2Operational = objectives[OBJ_FUSE_INSTALLED] && objectives[OBJ_LIFT_PASS]; //checks whether lift 2 is installed or not. only when fuse is fixed and pass is collected
        objectives[OBJ_LIFT2_ACTIVE] = lift2Operational;
        int f = playerFloor - 1;
        int r = playerRoom - 1;
        if (stringEquals(command, "quit")) { //we use stringEquals bcz char array doesnt allow '=='
            running = false;  //if command is quit, game ends
        } 
        else if (stringEquals(command, "inspect")) {
              handleInspectCommand(playerFloor, playerRoom, energy, time,
                                 roomEntities, entityCounts[f][r], objectives, score); 

        }
         else if (stringEquals(command, "move")) {
            int targetFloor, targetRoom; 
            if (!(cin >> targetFloor >> targetRoom)) { //this line takes reading then checks it, if failed it tells user how to type the command properly
                cin.clear(); //removes cin error
                cin.ignore(10000, '\n');//deletes the earlier input
                cout << "Usage: move <floor> <room>\n";
            } else {
                handleMoveCommand(targetFloor, targetRoom, playerFloor, playerRoom,
                                  energy, time, horizontalConn, inventory, inventoryCount,
                                  emergencyTripsLeft, lift2Operational);
            }

        } 
        else if (stringEquals(command, "collect")) {
            char symbol;
            cin >> symbol; //inputs what to collect
            if (handleCollectCommand(symbol, playerFloor, playerRoom, energy, time, score,
                                     roomEntities, entityCounts[f][r],
                                     inventory, inventoryCount, objectives)) { //if the function returns true, condition works
                if (symbol == ENT_EMERGENCY_CARD) {
                    emergencyTripsLeft = 3;   // the card gives 3 emergency trips
                }
            }

        }
         else if (stringEquals(command, "use")) {
            char symbol;
            cin >> symbol; //inputs what to use
            handleUseCommand(symbol, playerFloor, playerRoom, energy, time, score,
                             inventory, inventoryCount, objectives);

        }
         else if (stringEquals(command, "unlock")) {
            char code[10];
            cin >> setw(10) >> code; //makes sure there is no overflowing
            if (handleUnlockCabinetCommand(code, playerFloor, playerRoom, objectives, score,
                                           roomEntities, entityCounts[f][r])) {
                applyResourceDelta(energy, time, COST_ITEM_USE_ENERGY, COST_ITEM_USE_TIME); 
            }
        }
         else {
            cout << "Unknown command.\n";
        }
         if (playerFloor == 5 && playerRoom == 2 && //if player is in the train platform
            objectives[OBJ_PASSENGER_RESCUED] && objectives[OBJ_CAT_SECURED] &&
            objectives[OBJ_TRAIN_KEY] && !objectives[OBJ_TRAIN_PREPARED]) { //if train hasnt already been prepared and all other conditions are fulfilled, the condition runs
            objectives[OBJ_TRAIN_PREPARED] = true;
            applyScoreDelta(score, SCORE_PREPARE_TRAIN);
            objectives[OBJ_MISSION_WON] = true;
            applyScoreDelta(score, SCORE_MISSION_VICTORY);
        }

        if (checkEndConditions(energy, time, objectives)) { //after each command conditions are checked
            running = false; //if player is out of energy, time or has won or lost the game, game endss
        }
    }
}
bool handleMoveCommand(int targetFloor, int targetRoom,
                       int& currentFloor, int& currentRoom,
                       int& energy, int& time,
                       const char horizontalConn[NUM_FLOORS][ROOMS_PER_FLOOR - 1],
                       const char inventory[MAX_INVENTORY_CAPACITY], int inventoryCount,
                       int& emergencyTripsLeft, bool lift2Operational){
     if (targetFloor < 1 || targetFloor > NUM_FLOORS ||
        targetRoom < 1 || targetRoom > ROOMS_PER_FLOOR) {
        cout << "That room does not exist.\n"; //if less than 1 or greater than MAX
        return false;
    }

    if (targetFloor == currentFloor && targetRoom == currentRoom) {
        cout << "You are already here.\n";
        return false;
    }

    char connType = CONN_NONE; //stores what type of connection there is. it gets changed in the function below
    if (!isDirectlyConnected(currentFloor, currentRoom, targetFloor, targetRoom,
                             horizontalConn, connType)) {
        cout << "You can't get there from here.\n"; 
        return false;
    }

    if (!canTraverse(targetFloor, targetRoom, connType, inventory, inventoryCount,
                     emergencyTripsLeft, lift2Operational)) {
        cout << "Something blocks the way.\n";
        return false;
    }

    int deltaEnergy = COST_MOVE_ORDINARY_ENERGY;
    int deltaTime = COST_MOVE_ORDINARY_TIME; //deducts usual energy and time
    if (connType == CONN_WINDOW) {
        deltaEnergy = COST_MOVE_WINDOW_ENERGY; //deducts special amount of energy or time is connection is through window
        deltaTime = COST_MOVE_WINDOW_TIME;
    } else if (connType == CONN_LIFT1 || connType == CONN_LIFT2) {
        deltaEnergy = COST_MOVE_LIFT_ENERGY; //deducts special amount of energy or time is connection is through lifts
        deltaTime = COST_MOVE_LIFT_TIME;
    }
    applyResourceDelta(energy, time, deltaEnergy, deltaTime); //this applies the variables above 

    if (connType == CONN_LIFT2 && !lift2Operational) {
        emergencyTripsLeft--; //if it was a trip through lift 2 and it wasnt fixed yet, it was an emergency trip
    }

    currentFloor = targetFloor; //after all this, the player is finally moved
    currentRoom = targetRoom;
    return true;
}
bool handleInspectCommand(int currentFloor, int currentRoom,
                          int& energy, int& time,
                          const char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM],
                          int entityCount,
                          bool objectives[NUM_OBJECTIVES], int& score){
    applyResourceDelta(energy, time, COST_INSPECT_ENERGY , COST_INSPECT_TIME); //applies -1 for inspecting (-1 Energy, -1 time)

    cout << "You're searching the room...\n";
    for (int i = 0; i < entityCount; i++) { //this counts the room items one by one
        char item = roomEntities[currentFloor - 1][currentRoom - 1][i]; //stores symbol of item i in the said room. (-1 is done because arr starts from 0. so room 3 is actually room 2)
        cout << " - " << getEntityName(item) << "\n"; //gets the name of what symbol is stored in item var

        if (item == ENT_CLUE_4 && objectives[OBJ_CLUE_4] == false) { //asks if item is clue 4 and if it has not been found yet (this prevents user from getting points again for the same clue)
            objectives[OBJ_CLUE_4] = true; //if both are true, tick checkbox of clue 4
            applyScoreDelta(score, SCORE_CLUE_FOUND); //adds 10 points to score
        }
                if (item == ENT_CLUE_2 && objectives[OBJ_CLUE_2] == false) {
            objectives[OBJ_CLUE_2] = true;
            applyScoreDelta(score, SCORE_CLUE_FOUND);
        }
                if (item == ENT_CLUE_7 && objectives[OBJ_CLUE_7] == false) {
            objectives[OBJ_CLUE_7] = true;
            applyScoreDelta(score, SCORE_CLUE_FOUND);
        }
    }
    return true; //shows that the inspection worked
}
bool handleCollectCommand(char itemSymbol, int currentFloor, int currentRoom,
                          int& energy, int& time, int& score,
                          char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM],
                          int& entityCount,
                          char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount,
                          bool objectives[NUM_OBJECTIVES]){
    int f = currentFloor - 1;
    int r = currentRoom - 1;
    int found = -1; //not found yet. found var defines positions so it isnt initialized as 0
    for (int i = 0; i < entityCount; i++) {
        if (roomEntities[f][r][i] == itemSymbol) {
            found = i; //item is found at position i
        }
    }
    if (found == -1) {
        cout << "That item is not here.\n";
        return false;
    }
    if (itemSymbol == ENT_PASSENGER || itemSymbol == ENT_CAT ||
        itemSymbol == ENT_CONTROL_PANEL || itemSymbol == ENT_CABINET_TRAIN_KEY) {
        cout << "You can't pick that up.\n"; //a passenger, cat, machine and cabinet cant be picked up
        return false;
    }

    if (itemSymbol == ENT_ENERGY_PACK) {
        applyResourceDelta(energy, time, BONUS_ENERGY_PACK, 0); //adds 25 energy points
    } 
    else if (itemSymbol == ENT_EMERGENCY_TOOLKIT) {
        applyResourceDelta(energy, time,  BONUS_EMERGENCY_TOOLKIT, 0); //adds 15 energy points
    } 
    else if (itemSymbol == ENT_OBSERVATION_TOKEN) {
        applyResourceDelta(energy, time, 0,  BONUS_OBSERVATION_TOKEN); //adds 15 time points 
    } 
    else if (inventoryCount >= MAX_INVENTORY_CAPACITY) {
        cout << "Your bag is full.\n"; //the other packs are used up and not put in bag. the rest items need to move past this check
        return false;
    }
    else {//if bag isnt full, item is picked up
        applyResourceDelta(energy, time, COST_ITEM_PICKUP_ENERGY, COST_ITEM_PICKUP_TIME); //costs 1 energy and 1 time
        addItem(inventory, inventoryCount, itemSymbol); //added to bag
    }

    if (itemSymbol == ENT_MASTER_KEY) {
        objectives[OBJ_MASTER_KEY] = true; //if masterkey, box is ticked and 10 points are added
        applyScoreDelta(score, SCORE_KEY_COLLECT);
    } else if (itemSymbol == ENT_FUSE) {
        objectives[OBJ_FUSE_COLLECTED] = true;//if fuse, box is ticked but points are only added after it is installed
    } else if (itemSymbol == ENT_LIFT_PASS) {
        objectives[OBJ_LIFT_PASS] = true; //if pass, box is ticked and 20 points are added
        applyScoreDelta(score, SCORE_LIFT_PASS);
    } else if (itemSymbol == ENT_CAT_CARRIER) {
        objectives[OBJ_CAT_CARRIER] = true; //only ticks box, no points added
    }

    for (int i = found; i < entityCount - 1; i++) {
        roomEntities[f][r][i] = roomEntities[f][r][i + 1]; //next item is copied in its place and the found object is removed from the room
    }
    entityCount--; //entity count is reduced bcz one item is removed
    roomEntities[f][r][entityCount] = ENT_NONE; //the item copied before is removed so that 2 copies dont exist

    return true;
}
bool handleUseCommand(char itemSymbol, int currentFloor, int currentRoom,
                      int& energy, int& time, int& score,
                      char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount,
                      bool objectives[NUM_OBJECTIVES]){
    if (!hasItem(inventory, inventoryCount, itemSymbol)) {
        cout << "You don't have that item.\n";
        return false;   
    }

    if (itemSymbol == ENT_MEDICAL_KIT) {
        if (currentFloor != 4 || currentRoom != 1) {
            cout << "There is nobody to treat here.\n"; //the passenger to be treated is on floor 4, room 1
            return false;
        }
        objectives[OBJ_PASSENGER_RESCUED] = true;   //else passenger is treated
        applyScoreDelta(score, SCORE_RESCUE_PASSENGER); //100 points are added
        removeItem(inventory, inventoryCount, itemSymbol); //med kit is removed

    } 
    else if (itemSymbol == ENT_CAT_CARRIER) {
        if (currentFloor != 3 || currentRoom != 3) { // cat is on floor 3, room 3
            cout << "There is no cat here.\n";
            return false;
        }
        objectives[OBJ_CAT_SECURED] = true;
        applyScoreDelta(score, SCORE_SECURE_CAT); //100 points
        removeItem(inventory, inventoryCount, itemSymbol);

    } 
    else if (itemSymbol == ENT_FUSE) {
        if (currentFloor != 3 || currentRoom != 4) {
            cout << "There is no control panel here.\n";
            return false;
        }
        installFuseControlPanel(inventory, inventoryCount, objectives, score);

    } 
    else {
        cout << "Nothing happens.\n"; //other items have no use here
        return false;
    }

    applyResourceDelta(energy, time, COST_ITEM_USE_ENERGY, COST_ITEM_USE_TIME); //if any one item is used, energy and item is deducted so
    return true;
}
bool handleUnlockCabinetCommand(const char* code, int currentFloor, int currentRoom,
                                bool objectives[NUM_OBJECTIVES], int& score,
                                char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM],
                                int& entityCount){
     if (currentFloor != 5 || currentRoom != 1) {
        cout << "There is no cabinet here.\n";
        return false;
    }

    bool cabinetHere = false;
    for (int i = 0; i < entityCount; i++) {
        if (roomEntities[currentFloor - 1][currentRoom - 1][i] == ENT_CABINET_TRAIN_KEY) {
            cabinetHere = true; //goes through all items in the room. if one is the cabinet's symbol, flag is true
        }
    }
    if (!cabinetHere) {
        cout << "There is no cabinet here.\n";
        return false;
    }

    if (objectives[OBJ_CABINET_UNLOCKED]) {
        cout << "The cabinet is already open.\n"; //if this task is already done
        return false;
    }

    if (!verifyCabinetCode(code, objectives)) {
        cout << "Wrong code, or you haven't found all the clues.\n"; //if code order is false or all clues have not been found
        return false;
    }

    objectives[OBJ_CABINET_UNLOCKED] = true;
    applyScoreDelta(score, SCORE_CABINET_UNLOCK); // 30 points
    objectives[OBJ_TRAIN_KEY] = true;
    applyScoreDelta(score, SCORE_TRAIN_KEY); // 10 points
    cout << "Click! The cabinet opens and you take the Train Key.\n";
    return true;
}
bool checkEndConditions(int energy, int time, const bool objectives[NUM_OBJECTIVES]){
    if (energy <= 0 || time <= 0) { //if energy or time has run out
        return true; //game has ended
    }
    if (objectives[OBJ_MISSION_WON] || objectives[OBJ_MISSION_FAILED]) { //if mission has been won or lost
        return true; //game has ended
    }
    return false; //game has not ended
}          
