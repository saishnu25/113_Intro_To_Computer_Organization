/* 
* Saishnu Ramesh Kumar
* CSCI 113 - Introduction to Computer Organization (Lab 5)
* Simulation Programming for Booth's Multiplier
*/

/* 
** Global Documentation: 

1) This program is intended to simulate the booth's multiplier through a simulation. The program will take 2 16-bit values from
the textfile located within the same folder, MD and MQ respectively, and it will display the process the algorithm goes through. 

2) The standard library has only been used as this program used mainly arrays instead of vectors therefore the online library included
in this program is the iostream library. 

3) The input from data file was inserted in the main function of the program where it will get the data from three different textfiles that
are located in the same directory/folder as the program itself. They are labelled as such: testData1.txt, testData2.txt, and testData3.txt.

4) The method to run this program would be to go into the folder and open Command Prompt directly from there and you would enter:
'.cpp file name' < '.txt file name'. So if you would want to test the data from the first part, it would be: Lab_5 < testData1.txt.
Doing so will output the respective result. Remember to compile the program before entering the format listed above. 

5) This program was tested on Visual Studio Code and ran using the GPP compiler on Windows 11. 

*/

#include <iostream>

using namespace std;

//NOT Function
bool Not(bool a){
    return !a;
}

//AND Function
bool AND(bool a, bool b){
    return (a and b);
}

//OR Function
bool OR(bool a, bool b){
    return (a or b);
}

//XOR Function
bool XOR(bool a, bool b){
    return (a or b) and (!(a and b));
}

//Two Bit Decoder Function 
void twoBitDecoder(bool integer[], bool& a, bool& b, bool& c, bool& d){

    a = (AND(Not(integer[0]), Not(integer[1])));
    b = (AND(integer[1], Not(integer[0])));
    c = (AND(integer[0], Not(integer[1])));
    d = (AND(integer[1], integer[0]));
}

//2x1 Multiplexor - For building a one bit ALU
bool Multiplexor2x1(bool a, bool b, bool s){

    bool g = Not(s);
    return (OR(AND(g, a), AND(s, b)));
}

//4x1 Multiplexor - For building the one bit ALU for the 16 bit ALU
bool Multiplexor4x1(bool w, bool x, bool y, bool z, bool signal[]){

    bool a;
    bool b;
    bool c;
    bool d;

    twoBitDecoder(signal, a, b, c, d);
    return (OR(OR(AND(w, a), AND(x, b)), OR(AND(y, c), AND(z, d))));

}

//Full Adder Function
void fullAdder(bool a, bool b, bool cin, bool& cout, bool& sum){

    bool g = XOR(a, b);
    sum = XOR(cin, g);
    bool f = AND(a, b);
    bool r = XOR(a, b);
    bool j = AND(r, cin);
    cout = OR(f, j);
}

//One Bit ALU Function
void oneBitALU(bool a, bool b, bool binv, bool cin, bool op[], bool& cout, bool& r){

    bool sum;
    bool bi;
    bool k = Not(b);
    bi = Multiplexor2x1(b, k, binv);
    fullAdder(a, bi, cin, cout, sum);
    bool p = AND(a, b);
    bool t = OR(a, b);
    r = Multiplexor4x1(p, t, sum, 0, op);

}

//16 Bit ALU Function
void sixteenBitAlu(bool a[], bool b[], bool binv, bool op[], bool& of, bool r[]){

    bool c = 0; //Carry in value
    bool l = 0; //Last carry in

    oneBitALU(a[15], b[15], binv, binv, op, c, r[0]);

    //For loop will loop in between the 14 values in the middle
    for (unsigned int i = 14; i > 0; i--)
    {
        oneBitALU(a[i], b[i], binv, c, op, c, r[i]);
    }
    oneBitALU(a[0], b[0], binv, c, op, l, r[15]);
    of = XOR(c, l);
}

//Arithmetic Shift Function - It will bring the two registers and return the respective registers after it is done shifting 
void arithmeticShift(bool arr[], bool arr2[], bool r[], bool r2[], bool& MqOne){   
    int n;
    int temp = arr[n - 1]; 
    MqOne = arr2[15]; 

    //First register shift
    for (int i = n - 1;i > 0;i--){
        arr[i] = arr[i - 1];
    }
    if (arr[1] == 1) {
        arr[0] = 1;
    }
    else arr[0] = 0;

    //Second register shift
    for (int i = n - 1;i > 0;i--){
        arr2[i] = arr2[i - 1];
    }
    arr2[0] = temp;

    //Making a copy of first register shift for returning 
    for (int i = 0; i < 16; i++){
        r[i] = arr[i];
    }

    //Making a copy of second register shift for returning
    for (int i = 0; i < 16; i++){
        r2[i] = arr2[i];
    }
}

//Display Register Function
void displayRegister(bool md[], bool ac[], bool mq[], bool Mqone, int cycle[]){

    //Display the cycle count
    cout << "      ";
    for (int j = 0; j <= 3; j++){
        cout << cycle[j];
    }  
    cout << "   ";

    //Display MD register
    for (int i = 0; i < 16; i++){
        cout << md[i];
    }

    //Display AC register
    cout << "   ";
    for (int i = 0; i < 16; i++){
        cout << ac[i];
    }

    //Display MQ register
    cout << "   ";
    for (int i = 0; i < 16; i++){
        cout << mq[i];
    }

    cout << "       ";
    cout << Mqone;
    cout << endl;
}

//Booth's Algorithm Function
void boothsAlgorithm(bool MD[], bool AC[], bool MQ[], bool Mqone){
    
    int cycle[4] = { 1,1,1,1 }; //Cycle counter
    bool zero[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }; // Clearing AC
    displayRegister(MD, AC, MQ, Mqone, cycle);
    cout << endl;
    bool op[2] = { 1,0 };
    bool of = 0; // Overflow
    int counter = 0; //Counter to keep track of the cycle
    for (int i = 0; i < 16; i++){ // Loop to cycle through all of the bits

        //All possibe cases for the algorithm
        //When MQ and MQ-1 == 0
        if ((MQ[15] == 0 && Mqone == 0)){
            sixteenBitAlu(AC, zero, 0, op, of, AC);
        }
        //When MQ == 0 and MQ-1 == 1
        else if ((MQ[15] == 0 && Mqone == 1)){
            sixteenBitAlu(AC, MD, 0, op, of, AC);
        }
        //When MQ == 1 and MQ-1 == 0
        else if ((MQ[15] == 1 && Mqone == 0)){
            sixteenBitAlu(AC, MD, 1, op, of, AC);
        }
        //When MQ and MQ-1 == 1
        else if ((MQ[15] == 1 && Mqone == 1)){
            sixteenBitAlu(AC, zero, 0, op, of, AC);
        }
        counter++;

        //Update the cycle counter
        if (counter == 2){
            cycle[3] = 0;
        }
        if (counter == 4){
            cycle[2] = 0;
        }
        if (counter == 6){
            cycle[1] = 0;
        }
        if (counter == 8){
            cycle[0] = 0;
        }

        displayRegister(MD, AC, MQ, Mqone, cycle); // Displays the registers before the shift is done
        arithmeticShift(AC, MQ, AC, MQ, Mqone);
        displayRegister(MD, AC, MQ, Mqone, cycle); // Displays the register after the shift
        cout << endl;
    }
}


//Main Function
int main(){
        bool AC[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        bool MD[16], MQ[16];
        bool mqone = 0;

        //Reading in input from file here
        string line1, line2;
        
        //Get the data of MD (line1) and MQ (line2)
        getline(cin, line1);
        getline(cin, line2);

        //Store these into MD and MQ respectively
        for (int i = 0; i < 16; i++){
            if(line1[i] == '1'){
                MD[i] = true;
            }
            else{
                MD[i] = false;
            }

            if(line2[i] == '1'){
                MQ[i] = true;
            }
            else{
                MQ[i] = false;
            }
        }

        cout << "Cycle-Counter:" << "       " << " MD: " << "             " << " AC: " << "             " << " MQ: " << "         " << "MQ-1:" << endl;
        cout << endl;
        boothsAlgorithm(MD, AC, MQ, mqone);
        cout << endl; 

    return 0;   

}