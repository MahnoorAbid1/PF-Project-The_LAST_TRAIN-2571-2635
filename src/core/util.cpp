#include "util.h"
using namespace std;

// Primitive helper utilities, clamping, and screen control
int clampInt(int val, int minVal, int maxVal);{
int clampInt(int val, int minVal, int maxVal){
    if (val > maxVal){
        return maxVal;
    }
    if (val < minVal){
        return minVal;
    }
    return val;
    return val; 
}
energy = clampInt(energy + 25, 0, 100);
// eg: energy = clampInt(energy + 25, 0, 100) energy defined in main_core.cpp
// eg: energy = clampInt(energy + 25, 0, 100) energy defined in main_core file

void trimWhitespace(char* str) { //removes spaces, tabs, nextline char
    len = 0;
    while (str[len] != '/0'){
        len++;
    }
    while (len > 0 && (str[len-1] == ' ' || str[len - 1] == '\n' || str[len-1] == '\t' || str[len - 1] == '\r' )){
        str[len-1] = '\0'; //sets last character to null terminator
        len--;
    }
}

bool stringEquals(const char* a, const char* b){
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0'){
        if (a[i] != b[i])
            {return false;}
        i++}

    return a[i] == b[i];

}

void printHeader(const char* title) {   //prints title between two lines of ***
    cout << "*****************************" << endl;
    cout << "   " << title << endl;
    cout << "*****************************" << endl;
}

void clearScreen(){
    //left for later
    //#include <cstdlib>
    //void clearScreen() {
    //#ifdef _WIN32
        //system("cls");
    //#else
        //system("clear");
    //#endif
}





