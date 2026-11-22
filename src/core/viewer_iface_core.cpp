#include "viewer_iface.h"
#include "util.h"
#include <iostream>
using namespace std;

// Terminal UI and dashboard rendering using primitive parameters


//Shows the current status on the screen
//Where they are and the energy,time etc.
//Only displays info, doesn't change anything
void renderDashboard(int playerFloor, int playerRoom, int energy, int time, int score, const char inventory[MAX_INVENTORY_CAPACITY], int inventoryCount, int emergencyTripsLeft){
    //Border on top and title of dashboard
    cout<<"--------------------------------------"<<endl;
    cout<<"|          PLAYER'S DASHBOARD        |"<<endl;
    cout<<"--------------------------------------"<<endl;

    //The current location of the player, like F3-R4
    cout<< "Location: F"<<playerFloor<<"-R"<<playerRoom<<endl;

    //Time and energy; output will be displayed as: current/maximum
    cout<< "Energy  : " << energy << "/" << MAX_ENERGY<< endl;
    cout<< "Time    : " << time << "/" << MAX_TIME<< endl;

    //The current score
    cout<< "Score   : " << score << endl;

    //Emergency Rides left on lift 2(Emergency access card tell this)
    cout<< "Lift 2 emergency trips left: " << emergencyTripsLeft << endl;

    //Inventory; output will be displayed as: Current inventory count/ Max capacity
    cout<< "Inventory (" << inventoryCount << "/" << MAX_INVENTORY_CAPACITY<<"): ";

    if (inventoryCount == 0) {
        cout << "Empty";                          //Bag is empty
    } 
    else {
        //Print every item in the bag,followed by a space
        for (int i = 0; i < inventoryCount; i++) {      
            cout << inventory[i] << " ";              
        }
    }
    cout << endl;  //Inventory line finishes here

    //Border on bottom of the dashboard
    cout<<"--------------------------------------"<<endl;
}


//Shows what is in the room: which room is it and the items in there
//Used when a room is inspected by a player
//roomEntities is a 3d array in which the first number tells which floor, second number tells what room on that floor, and third tells which slot on the rooms shelf
void renderRoomDetails(int floor, int room, const char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM], int entityCount){

    // Safety check: Checks if the room exists
    if (floor < 1 || floor > NUM_FLOORS || room < 1 || room > ROOMS_PER_FLOOR){
        cout << "The room isn't valid!" << endl;
        return ;
    }

    // Safety check: Checks if the items are more than the shelf can hold
    entityCount=clampInt(entityCount,0,MAX_ENTITIES_PER_ROOM);

    //Border on top with the name of the room, eg: F3-R4
    cout<<"--------------------------------------"<<endl;
    cout<< "        ROOM F"<<floor<<"-R"<<room<<endl;
    cout<<"--------------------------------------"<<endl;

    //Items in the room
    cout<< "Items here: ";

    if (entityCount==0){
        cout<<"Nothing!"; //Room is empty
    }
    else{
        //Print every item in the room,followed by a space
        for (int i = 0; i < entityCount; i++) {                 //Go through the slots that are filled
            cout << roomEntities[floor-1][room-1][i] << " "; //Floor and room pick the room player is currently in(-1 because array's index start at 0), i picks the slot( slot is the index of the item in the room like e.g in F3-R2, slot zero holds 4, slot one holds * and so on )
        }
    }
    cout << endl;
    
    //Border on bottom of the room box
    cout<<"--------------------------------------"<<endl;
}


//Draws a map of the building (all 20 rooms)
//Shows [ YOU ] instead of the room number, of the room the player is in right now
void renderAsciiBuilding(int playerFloor, int playerRoom){
    //Border on top and title of the map
    cout<<"--------------- BUILDING MAP --------------"<<endl;

    //Go through each floor top to bottom like 5,4,3...
    for (int FloorNum=NUM_FLOORS; FloorNum>=1 ; FloorNum--){
        cout<< "Floor " << FloorNum <<": "; //The floor num at the start of each row, e.g Floor 5: 
        //Go through each of the 4 rooms on this floor, from left to right like 1,2,3...
        for (int RoomNum=1; RoomNum<=ROOMS_PER_FLOOR; RoomNum++){
            if (FloorNum==playerFloor && RoomNum==playerRoom){
                cout<<"[ YOU ] "; //Marks the room player currently is in
            }
            else{
                cout<< "[F" << FloorNum << "-R" << RoomNum << "] ";   //room normally printed like [F3-R2]
            }
        }
        cout<<endl; //Moves to the next line/floor
    }
    //Border on bottom and a key that helps in reading the map
    cout<<"-------------------------------------------"<<endl;
    cout<<"[ YOU ] = represents the room you are currently in"<<endl;
}

//objectives is a row of 16 true/false boxes, one box is for each objective/goal
//Prints [X] if done or [ ] if not done
//objectives is the array. OBJ_MASTER_KEY, OBJ_CLUE_4... are the index positions already stored in state.h
void renderObjectivesChecklist(const bool objectives[NUM_OBJECTIVES]){
    //Border on top and title of the checklist
    cout<<"--------------- MISSION CHECKLIST --------------"<<endl;
    
    if(objectives[OBJ_MASTER_KEY]){ 
        cout<<"[X] ";
    } 
    else{ 
        cout<<"[ ] ";
    }
    cout<<"Master Key Collected"<<endl;

    if(objectives[OBJ_CLUE_4]){ 
        cout<<"[X] ";
    } 
    else{ 
        cout<<"[ ] ";
    }
    cout<<"Clue digit 4 found"<<endl;

    if(objectives[OBJ_CLUE_2]){ 
        cout<<"[X] ";
    } 
    else{ 
        cout<<"[ ] ";
    }
    cout<<"Clue digit 2 found"<<endl;

    if(objectives[OBJ_CLUE_7]){ 
        cout<<"[X] ";
    } 
    else{ 
        cout<<"[ ] ";
    }
    cout<<"Clue digit 7 found"<<endl;

    if(objectives[OBJ_CABINET_UNLOCKED]){ 
        cout<<"[X] ";
    } 
    else{ 
        cout<<"[ ] ";
    }
    cout<<"Cabinet unlocked(code 427) "<<endl;

    if(objectives[OBJ_TRAIN_KEY]){ 
        cout<<"[X] ";
    } 
    else{ 
        cout<<"[ ] ";
    }
    cout<<"Train key acquired"<<endl;

    if(objectives[OBJ_FUSE_COLLECTED]){ 
        cout<<"[X] ";
    } 
    else{ 
        cout<<"[ ] ";
    }
    cout<<"Fuse Collected"<<endl;

    if(objectives[OBJ_FUSE_INSTALLED]){ 
        cout<<"[X] ";
    } 
    else{ 
        cout<<"[ ] ";
    }
    cout<<"Fuse installed at control panel"<<endl;

    if(objectives[OBJ_LIFT_PASS]){ 
        cout<<"[X] ";
    } 
    else{ 
        cout<<"[ ] ";
    }
    cout<<"Lift Pass was Collected"<<endl;

    if(objectives[OBJ_LIFT2_ACTIVE]){ 
        cout<<"[X] ";
    } 
    else{ 
        cout<<"[ ] ";
    }
    cout<<"Lift 2 is working"<<endl;

    if(objectives[OBJ_PASSENGER_RESCUED]){ 
        cout<<"[X] ";
    } 
    else{ 
        cout<<"[ ] ";
    }
    cout<<"Passenger was rescued"<<endl;

    if(objectives[OBJ_CAT_CARRIER]){ 
        cout<<"[X] ";
    } 
    else{ 
        cout<<"[ ] ";
    }
    cout<<"Cat carrier was collected"<<endl;

    if(objectives[OBJ_CAT_SECURED]){ 
        cout<<"[X] ";
    } 
    else{ 
        cout<<"[ ] ";
    }
    cout<<"Cat was secured"<<endl;


    if(objectives[OBJ_TRAIN_PREPARED]){ 
        cout<<"[X] ";
    } 
    else{ 
        cout<<"[ ] ";
    }
    cout<<"Train is prepared and it is boarded"<<endl;
    //Border on bottom 
    cout<<"-------------------------------------------"<<endl;

    if(objectives[OBJ_MISSION_WON]){ 
        cout<<"Status of mission: WON!"<<endl;
    } 
    else if(objectives[OBJ_MISSION_FAILED]){ 
        cout<<"Status of mission: LOST!"<<endl;
    }
    else{
        cout<<"Status of mission: In progress!"<<endl;
    }
}