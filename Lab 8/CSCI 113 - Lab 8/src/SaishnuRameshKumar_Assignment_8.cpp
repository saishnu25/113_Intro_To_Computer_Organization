/*
* Saishnu Ramesh Kumar (300758706)
* CSCI 113 - Assignment 8
* Memory Simulation Program 
*/

/*
* Global Documentation: 
* 1) This CPP File simulates the memory operations for a memory hierarchy program. It uses a 2-way set
*   cache and 128 words and memory blocks that contains 1 word per block. 
*
* 2) The registers in this file only contain 8 registers, $s0 to $s7
*
* 3) The Input/Output is handled through the input put. The input is read through an external file called Input-object-code. 
*
* 4) This program has been programmed using Visual Studio Code and you would be able to execute it via
    CMD Prompt by first compiling and then running the program. 
*/

#include <iostream>
#include <fstream>
#include <string>
#include <math.h>

using namespace std;

//Block struct
struct Block{
    bool valid = false;
    bool history = false;
    int tag = 0;
    int data = 0;
};

//String to Integer Function
int stringToInt(string str, size_t size){
    int result = 0;
    for(int i = 0; i < size; i++){
        result += pow(2,i) * (int)(str.at(size - i - 1) - (int)('0'));
    }

    return result;
}

//Bit Print Function
void bitPrint(int value){
    int mask = 1;
    mask <<= 31; //Shfting left by 31 positions

    for(int i = 1; i <= 32; i++){
        //Checks most left bit
        if((value & mask) == 0) {
            cout << "0";
        }
        else{
            cout<< "1";
        }
        value <<= 1; //Shifts left by 1 
    }
}

//Main Function 
int main(){

    int registerFile[8] = {0,0,0,0,0,0,0,0}; //Initializing the memory
    int memory[128]; //Initalizing the cache 

    for(int i = 0; i < 128; i++){
        memory[i] = i + 5;
    }

    Block block[8][2]; //Initializing the block

    //Opening the external file
    ifstream extFile("Input-object-code");

    cout << "Instruction" << endl << "=============" << endl;

    //Main Loop 
    while(!extFile.eof()){ //Loop until the end of the file 
        string objCode;
        extFile >> objCode;
        if(extFile.eof()){
            break;
        }
        cout << objCode;

        string opCode;
        for(int i = 0; i < 6; i++){
            opCode += objCode.at(i);
        }

        string rs;
        for(int i = 0; i < 5; i++){
            rs += objCode.at(6 + i);
        }

        string rt;
        for(int i = 0; i < 5; i++){
            rt += objCode.at(11 + i);
        }

        rt.at(0) = '0';
        rt.at(1) = '0';
        
        string offset;
        for(int i = 0; i < 16; i++){
            offset += objCode.at(16 + i);
        }

        //Computing the word address
        int wordAddress = (stringToInt(rs, 5) + stringToInt(offset, 16)) / 4;

        //Computing the cache set and tag of the memory address
        int cacheSet = wordAddress % 8;
        int tag = wordAddress / 8;

        if(stringToInt(opCode, 6) == 35){
            //Cache 1 - Hit: Loading the cache 1 data into its destination register
            if(block[cacheSet][0].valid && block[cacheSet][0].tag == tag){
                cout << "\t hit" << endl;
                registerFile[stringToInt (rt, 5)] = block[cacheSet][0].data;
                block[cacheSet][0].history = true;
                block[cacheSet][1].history = false;
            }
            
            //Cache 1 - Miss
            else{
                //Cache 2 - Hit: Loading the cache 2 data into its destination register
                if(block[cacheSet][1].valid && block[cacheSet][1].tag == tag){
                    cout << "\t hit" << endl;
                    registerFile[stringToInt(rt, 5)] = block[cacheSet][1].data;
                    block[cacheSet][0].history = false;
                    block[cacheSet][1].history = true;
                }

                //Cache 2 - Miss: Updates the memory and stores the new value in the cache, then moves to the register
                else{
                    cout << "\t miss" <<endl;
                    int victimIDs = 0;

                    if(block[cacheSet][0].history && !block[cacheSet][1].history){
                        victimIDs = 1;
                    }

                    if(block[cacheSet][victimIDs].valid){
                        memory[(block[cacheSet][victimIDs].tag * 8) + cacheSet] = block[cacheSet][victimIDs].data;
                    }
                    
                    block[cacheSet][victimIDs].data = memory[wordAddress];
                    block[cacheSet][victimIDs].valid = true;
                    block[cacheSet][victimIDs].tag = tag;
                    registerFile[stringToInt(rt, 5)] = block[cacheSet][victimIDs].data;
                    block[cacheSet][victimIDs].history = true;
                    block[cacheSet][1 - victimIDs].history = false;
                }
            }
        }

        //Store word: Writing data into the memory 
        else if(stringToInt(opCode, 6) == 43){
            //Cache 1 - Hit: Stores the register data into cache block
            if(block[cacheSet][0].valid && block[cacheSet][0].tag == tag){
                cout << "\t hit" << endl;
                block[cacheSet][0].data = registerFile[stringToInt(rt, 5)];
                block[cacheSet][0].history = true;
                block[cacheSet][1].history = false;
            }

            //Cache 1 - Miss
            else{
                if(block[cacheSet][1].valid && block [cacheSet][1].tag == tag){
                    cout << "\t hit" << endl;
                    block[cacheSet][1].data = registerFile[stringToInt (rt, 5)];
                    block[cacheSet][0].history = false;
                    block[cacheSet][1].history = true;
                }

                //Cache 2 - Miss: Stores the register data to the memory (Word Address)
                else{
                    cout << "\t miss" << endl;
                    memory[wordAddress] = registerFile[stringToInt(rt, 5)];
                }
            }
        }

        else{
            cout << "Not lw or sw!" << endl;
        }
    }

    //Print out contents of registerFile, memory, and cache
    cout << endl;
    cout << "Registers" << endl << "=========" << endl;
    for(int i = 0; i < 8; i++){
        cout << i << " : ";
        bitPrint(registerFile[i]);
        cout << endl;
    }

    cout << endl;
    cout << "Cache" << endl << "===== \n" << endl;
    cout << "Block        Valid            Tag                          Data" << endl;
    cout << "-----        -----            ---                          ----" << endl;

    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 2; j++){
            cout << "block [" << i << "][" << j << "]: ";
            cout << block[i][j].valid << " ";
            bitPrint(block[i][j].tag);
            cout << " ";
            bitPrint(block[i][j].data);
            cout << endl;
        }
    }

    cout << endl;
    cout << "Memory" << endl << "======" << endl;
    
    for(int i = 0; i < 64; i++){
        cout << i << ": ";
        bitPrint(memory[i]);
        cout << "      " << i + 64 << ": ";
        bitPrint(memory[i + 64]);
        cout << endl;
    }

    //Closing the external file
    extFile.close();
    return 0;

}