#include "switches.h"
#include "util.h"

// Door lock mechanics, lifts, and cabinet verification without structures
bool canUseLift1(int fromFloor, int toFloor){ //only allows movemoent from 1 to 2 or 2 to 1
    if((fromFloor == 1 && toFloor == 2) || (fromFloor == 2 && toFloor == 1))    
        {return true;}
    return false;
}

bool canUseLift2(int fromFloor, int toFloor, bool lift2Operational, int emergencyTripsLeft){
    bool from = false;
    bool to = false;
    if(fromFloor == 2 || fromFloor == 4 || fromFloor == 5)
    {  from = true;} 
    if (toFloor == 2 || toFloor == 4 || toFloor == 5){
        to = true; } //lift 2 only allows movement to and from 2nd, 4th and 5th floor
    if (to == false || from == false ){ //if none of the floors, lift doesnt work
        return false;
    }
    if (fromFloor == toFloor){ //lift doesnt work if from and to are the same
        return false;
    }
    if (lift2Operational || emergencyTripsLeft > 0) { //lift is operational meaning fuse is fixed
        return true; //if fused is not fixed, lift allows 3 emergency trips before stopping
    }
    return false; //if neither condition is fulfilled, false is returned
}
bool unlockFloor3Door(const char inventory[MAX_INVENTORY_CAPACITY], int inventoryCount){
    for(int i = 0; i < inventoryCount; i++){
        if (inventory[i] == '<'){
            return true;
        }}
        return false;

}

bool installFuseControlPanel(char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount,
                             bool objectives[NUM_OBJECTIVES], int& score){
    if (hasItem(inventory, inventoryCount, ENT_FUSE) == false){
        return false; //checks if player has fuse. if not, it stops and returns false
    }
    removeItem(inventory, inventoryCount, ENT_FUSE); //if fuse is present, it removes it
    objectives[OBJ_FUSE_INSTALLED] = true;
    applyScoreDelta(score, SCORE_INSTALL_FUSE);
    return true;
    objectives[OBJ_FUSE_INSTALLED] = true; //ticks checkbox that fuse has been installed
    applyScoreDelta(score, SCORE_INSTALL_FUSE); //gives player 20 points bcz score of installing fuse is 20 pts
    return true; //proves that the code worked
}

bool verifyCabinetCode(const char* enteredCode, const bool objectives[NUM_OBJECTIVES]);


//to open cabinet you need the right code
bool verifyCabinetCode(const char* enteredCode, const bool objectives[NUM_OBJECTIVES]){
    if (objectives[OBJ_CLUE_4] == false || objectives[OBJ_CLUE_2] == false || objectives[OBJ_CLUE_7] == false)
        {return false;} //to figure out if the checkbox has been ticked before or not. If yes, code failed, cabinet remains shut
    return stringEquals(enteredCode, "427"); //to check if order of digits is right (427)
}
//there are two checks in this code. if a person is a replayer, he would simply move to the cabinet and put in the code
//but the game demands exploration and collecction of clues. so it demands the checkbox being ticked.



