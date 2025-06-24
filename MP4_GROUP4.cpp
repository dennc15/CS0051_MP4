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

recursive_mutex printMtx;
int roundNo = 1;

struct Question{
    string q;
    char ans;
    vector<string> ch;
};

vector<Question> questions = { //SAMPLE QUESTIONS
    {"What is 2 + 2?", 'a', {"a) 4", "b) 3", "c) 5", "d) 22"}},
    {"Capital of France?", 'b', {"a) Rome", "b) Paris", "c) London", "d) Berlin"}},
    {"Which planet is known as the Red Planet?", 'c', {"a) Venus", "b) Jupiter", "c) Mars", "d) Mercury"}},
    {"What is the boiling point of water at sea level?", 'd', {"a) 50°C", "b) 90°C", "c) 110°C", "d) 100°C"}},
    {"Who wrote 'Romeo and Juliet'?", 'a', {"a) William Shakespeare", "b) Charles Dickens", "c) Jane Austen", "d) Mark Twain"}}
};

void gameplay(int id, latch& login, barrier& finishRound, bool isAuto){
    int ch;
    
    {
        lock_guard<mutex> lock(printMtx);
        cout<<"Player "<< id<<" Logging in.."<<endl;
    }
    //login use countdown and print when player has logged in
    this_thread::sleep_for(chrono::seconds(1)); //temporary latch
    //"All players logged in!"
    for (int i = roundNo; i <= 3; ++i){
        {
            lock_guard<recursive_mutex> printMtx;
            ch = askQ();
            cout<<"Player "<<id<<" has chosen "<<ch<<"!"<<endl;
        }
        finishRound.arrive_and_wait();
        
    }

}

void start(int numPlayers, bool isAuto){
    latch login(numPlayers);
    barrier finishRound(numPlayers, [](){
       cout<<"[ROUND "<<roundNo<<" FINISHED]"<<endl;;
       cout<<"Checking scores..."<<endl;
    });


    vector<thread> players;
    for(int i = 0; i < numPlayers; ++i) players.emplace_back(gameplay, i+1, ref(login), ref(finishRound), isAuto); //start threads
    this_thread::sleep_for(chrono::seconds(1)); //temporary latch
    
    cout<<"All players logged in!";
    


    for (auto& t : players) t.join(); //join threads
}


char askQ(){ //asks questions and calls lifeline
    char ch;
    Question q = questions[roundNo-1];
    cout<<q.q<<endl;
    for (auto& choice : q.ch)cout<<choice<<endl;
    cout<<"Choice: ";
    cin>>ch;
    //add lifeline here, lifeline will return the ch that this method will be returning
    return ch;
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

