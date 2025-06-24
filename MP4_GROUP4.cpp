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
int points[4] = {0,0,0,0};
bool correct[4] = {0,0,0,0};

barrier finishRound(4, [](){
    lock_guard<recursive_mutex> lock(printMtx);
    cout<<"\n[ROUND "<<roundNo<<" FINISHED]"<<endl;
    cout<<"Checking scores..."<<endl;
    for(int i=0; i<4; i++){
        if(correct[i]){
            cout<<"Player "<<i+1<<" has answered correctly!"<<endl;
        }else{
            cout<<"Player "<<i+1<<" has answered incorrectly!"<<endl;
        }
    }
    cout<<"\nCurrent Scores:"<<endl;
    for(int i=0; i<4; i++)
        cout<<"Player "<<i+1<<" -- "<<points[i]<<endl;
    
    roundNo++;
});

struct Question{
    string q;
    char ans;
    vector<string> ch;
};

vector<Question> questions = { 
    {"Which of the following would a magnet most likely attract?", 'a', {"a) Metal", "b) Plastic", "c) Wood", "d) The wrong person"}},
    {"What is the most populated country in the world?", 'b', {"a) China", "b) India", "c) United States", "d) Russia"}},
    {"Which insect shorted out an early supercomputer and inspired the term 'computer bug'?", 'd', {"a) Beetle", "b) Cockroach", "c) Fly", "d) Moth"}},
    {"Who sang the song 'Billy Jean'?", 'b', {"a) Michael B. Jordan", "b) Michael Jackson", "c) Michael Jordan", "d) Michael Wazowski"}},
    {"Who lives in a pineapple under the sea?", 'c', {"a) Patrick Star", "b) Phineas and Ferb", "c) Spongebob Squarepants", "d) Ariel"}},
    {"Who painted the Mona Lisa?", 'd', {"a) Vincent van Gogh", "b) Pablo Picasso", "c) Claude Monet", "d) Leanardo da Vinci"}},
    {"What is the deadliest animal in the world?", 'b', {"a) Scorpion", "b) Mosquito", "c) Crocodile", "d) Shark"}},
    {"The Earth is approximately how many miles away from the Sun?", 'c', {"a) 9.3 million", "b) 39 million", "c) 93 million", "d) 193 million"}},
    {"Who was the first female scientist to win a Nobel Prize?", 'b', {"a) Emily Blackwell", "b) Marie Curie", "c) Florence Nightingale", "d) Grace Hopper"}}
};

//PROTOTYPES
char askQ();
char lifeline();
    //lifeline menu uses rng <---------------------- TODO
void gameplay(int id, bool isAuto);
void start(int numPlayers, bool isAuto);
bool checkAns(int id, char ans); 
    //check if true and cout if player is right<---------------------- TODO
string checkHighest();


int main()
{
    int ch, numPlayers;

    while (true){
    cout<<"MENU"<<endl;
    cout<<"1. Automatic"<<endl; //OPTIONAL NALANG 
    cout<<"2. Manual"<<endl;
    cout<<"3. Exit"<<endl;

    cout << "Enter choice: ";
    cin>>ch;
    
        switch(ch){
            case 2: //Manual
                start(4, false);
                return 0; //program exits after the winner is announced 
            case 3:
                cout << "Exiting..." << endl;
                return 0;
            default:
                if (cin.fail()){
                    cin.clear();
                    cin.ignore(1000, '\n');
                    cout << "Error. Input should be a number." << endl;
                } else {
                    cout << "Invalid choice. Please choose from 1-3 only." << endl;
                }
        }
    }
}

char askQ(int id){ //<------------------- TODO call lifeline, add automated version too either in this function or in a diff function or sa gameplay function
    Question q = questions[roundNo-1];
    char ch = ' ';
    string answer;
    
    {
        lock_guard<recursive_mutex> lock(printMtx);
        cout << q.q << endl;
        for (auto& choice : q.ch) cout << choice << endl;
        cout << "[You have 15 seconds to answer]\n";
        cout << "Player " << id << "'s choice (Press 0 to use lifeline): ";
    }
    
    auto start = chrono::steady_clock::now(); //start timer
    

    cin >> answer; //answer will be marked wrong if entered after the time limit
    
    auto end = chrono::steady_clock::now();
    auto timeElapsed = chrono::duration_cast<chrono::seconds>(end - start).count();
    
    if (timeElapsed > 15){
        lock_guard<recursive_mutex> lock(printMtx);
        cout << "\nInvalid answer. Time limit exceeded!\n";
        return 'x'; //player's answer is automatically marked
    }
    
    ch = answer[0];
    //add lifeline here, lifeline will return the ch that this method will be returning <------------------- TODO
    return ch;
}

bool checkAns(int id, char ch) {
    char cor = questions[roundNo - 1].ans;
    if (ch == cor) {
        points[id - 1] += 10;
        correct[id - 1] = 1;
        return true;
    } else {
        correct[id - 1] = 0;
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
    //login use countdown and print when player has logged in <------------------------- TODO
    this_thread::sleep_for(chrono::seconds(1)); //temporary latch
    //"All players logged in!"
    for (int i = roundNo; i <= 3; ++i){
        {
            lock_guard<recursive_mutex> lock(printMtx);
            cout<<"\n[ROUND "<<roundNo<<" -- Player "<<id<<"'s turn]"<<endl;
            ch = askQ(id);

            //cout<<"Player "<<id<<" has chosen "<<ch<<"!"<<endl; print this in automated
            
        }
        //async
        future<bool> res = async(launch::async, checkAns, id, ch);
        finishRound.arrive_and_wait();
        
    }

}

string checkHighest(){
    int max = points[0];
    vector<int> top = {0};

    for (int i = 1; i < 4; ++i) {
        if (points[i] > max) {
            max = points[i];
            top.clear();
            top.push_back(i);
        } else if (points[i] == max) {
            top.push_back(i);
        }
    }

    string winners;
    for (size_t i = 0; i < top.size(); ++i) {
        winners += "Player " + to_string(top[i] + 1);
        if (i < top.size() - 1)
            winners += " and ";
    }

    return winners;
}

void start(int numPlayers, bool isAuto){
    vector<thread> threads;
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back(gameplay, i+1, isAuto);
    }

    this_thread::sleep_for(chrono::seconds(1)); //temporary latch
    
    //cout<<"All players logged in!"; print when latch is goods <------------------- TODO
    
    for (auto& t : threads) t.join();
    
    cout<<"The Player(s) with the highest score is.. "<<checkHighest()<<"!"<<endl;

}
