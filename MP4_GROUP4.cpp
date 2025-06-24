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
latch login(4);
barrier finishRound(4, [](){
    lock_guard<recursive_mutex> lock(printMtx);
    cout<<"[ROUND "<<roundNo<<" FINISHED]"<<endl;
    cout<<"Checking scores..."<<endl;
    for(int i=0; i<4; i++){
        if(correct[i]){
            cout<<"Player "<<i+1<<" has answered correctly!"<<endl;
        }else{
            cout<<"Player "<<i+1<<" has answered incorrectly!"<<endl;
        }
    }
    cout<<"\nCurrent Scores:"<<endl;
    for(int i=0; i<4; i++)cout<<"Player "<<i+1<<"-- "points[i]<<endl;
    

    
});

struct Question{
    string q;
    char ans;
    vector<string> ch;
};

int points[4] = {0,0,0,0};
bool correct[4] = {0,0,0,0};


vector<Question> questions = { //SAMPLE QUESTIONS
    {"What is 2 + 2?", 'a', {"a) 4", "b) 3", "c) 5", "d) 22"}},
    {"Capital of France?", 'b', {"a) Rome", "b) Paris", "c) London", "d) Berlin"}},
    {"Which planet is known as the Red Planet?", 'c', {"a) Venus", "b) Jupiter", "c) Mars", "d) Mercury"}},
    {"What is the boiling point of water at sea level?", 'd', {"a) 50°C", "b) 90°C", "c) 110°C", "d) 100°C"}},
    {"Who wrote 'Romeo and Juliet'?", 'a', {"a) William Shakespeare", "b) Charles Dickens", "c) Jane Austen", "d) Mark Twain"}}
};

//PROTOTYPES
char askQ();
char lifeline();
    //lifeline menu uses rng <---------------------- TODO
void gameplay(int id, bool isAuto);
void start(int numPlayers, bool isAuto);
bool checkAns(int id, char ans); 
    //check if true and cout if player is right<---------------------- TODO


int main()
{
    int ch, numPlayers;

    cout<<"MENU"<<endl;
    cout<<"1. Automatic"<<endl;
    cout<<"2. Manual"<<endl;

    //add error handling and exit
    cin>>ch;

    switch(ch){
        case 2: //Manual
            start(4, false);
            
    }
}

char askQ(int id){ //asks questions and calls lifeline, add automated version too either in this function or in a diff function or sa gameplay function
    char ch;
    Question q = questions[roundNo-1];
    cout<<q.q<<endl;
    for (auto& choice : q.ch)cout<<choice<<endl;
    cout<<"Player "<<id<<"'s choice: ";
    cin>>ch;
    //add lifeline here, lifeline will return the ch that this method will be returning
    return ch;
}

bool checkAns(int id) {
    char correct = questions[roundNo - 1].ans;
    if (answer == correct) {
        points[id - 1] += 10;
        return true;
    } else {
        return false;
    }
}

void gameplay(int id, bool isAuto){
    char ch;
    int plPoints = points[id-1];
    {
        lock_guard<recursive_mutex> lock(printMtx);
        cout<<"Player "<< id<<" Logging in.."<<endl;
    }
    //login use countdown and print when player has logged in
    this_thread::sleep_for(chrono::seconds(1)); //temporary latch
    //"All players logged in!"
    for (int i = roundNo; i <= 3; ++i){
        {
            lock_guard<recursive_mutex> lock(printMtx);
            cout<<"[ROUND "<<roundNo<<" -- Player "<<id<<"'s turn]";
            ch = askQ(id);

            cout<<"Player "<<id<<" has chosen "<<ch<<"!"<<endl;
            
        }
        //async
        future<bool> res = async(launch::async, checkAns, id);
        finishRound.arrive_and_wait();
        
    }

}

void start(int numPlayers, bool isAuto){
    vector<thread> threads;
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back(gameplay, i+1, isAuto);
    }

    this_thread::sleep_for(chrono::seconds(1)); //temporary latch
    
    cout<<"All players logged in!";
    
    for (auto& t : threads) t.join();

}

