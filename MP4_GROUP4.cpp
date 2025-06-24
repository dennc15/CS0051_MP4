#include <stdio.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <barrier>
#include <future>
#include <mutex>
#include <latch>
#include <vector>

using namespace std;

mutex printMtx;

struct Question{
    string q;
    char ans;
    vector<string> ch;
};

void gameplay(int id, latch& login, bool isAuto){
    {
        lock_guard<mutex> lock(printMtx);
        cout<<"Player "<< id<<" Logging in.."<<endl;
    }
    //login use countdown and print when player has logged in
    this_thread::sleep_for(chrono::seconds(1)); //temporary latch
    //"All players logged in!"
    

}

void start(int numPlayers, bool isAuto){
    latch login(numPlayers);

    vector<thread> players;
    for(int i = 0; i < 4; ++i) players.emplace_back(gameplay, i+1, ref(login), isAuto); //start threads
    this_thread::sleep_for(chrono::seconds(1)); //temporary latch
    
    cout<<"All players logged in!";


    for (auto& t : players) t.join(); //join threads
}


void askQ(){ //asks questions and calls lifeline

}

char lifeline(){
    //lifeline menu uses rng
}

int main()
{
    int ch, numPlayers;

    cout<<"MENU"<<endl;
    cout<<"1. Automatic"<<endl;
    cout<<"2. Manual"<<endl;

    //add error handling and exit
    cin>>ch;

    switch(ch){
        case 2: 
            cout<<"Num players: ";
            cin>>numPlayers;
            start(numPlayers, false);
            
    }
}

