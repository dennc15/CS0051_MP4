/*
log in users

round 1:
Question
a
b
c
d

0 - lifeline

Player 1:
choice: a

Player 2:
choice: b

Player 3:
choice: c

Player 4:
choice: 0

lifeline
-----------
1 - ask a friend
2 - 50/50
3 - ask the audience

Choice: 1 



Friend: "I think its a"

//reprompt question
a
b
c
d

//pressing 0 leads to resubmission na instead of lifeline
Choice: d 

Host: the correct answer is...
Host: Letter d

Player 1 answered incorrectly!
Player 2 answered incorrentyl!
...
Player 4 answered correctly!


add points



[computing scores..]
1st - Player 1



Division of parts:
    1 - Simulation, Threads, async, barrier etc. bea
    2 - Q&A, scoring, timer (chrono) denise
    3 - Login (Latches), Lifeline, 
    
    Overall - add mutexes for printing

functions:
    simulation //initialize threads here----
        thread t1(gameplayAutomated, 1);
        t1.join();
    void gameplayAutomated //no input needed----
    void gameplayManual //user input for all players ---- {
        questions = [
            ["Q: sdaada","a"],
            ["Q2: sdadsdad", "b"]
        ] 
        for (q : question){
            ans = question(q)
            bool tf = async(checking, ans)
            print checking ans
            if(tf){
                plus points
                print player # is correct
            }
            else{
                print player # is incorrect
            }
        }
    }
    void Question(string [] q) //calls lifeline here
        print q[0]
        choice:
        //start timer
        if(ch == 0){
            ch = lifeline();
        }
        //repeat question if answer is not valid
        
        return ch;
    
    string Lifeline() //use rng
    bool checking(answer)
    void addScore(){
        if checking()
    }

    dk if we should use string or char for answer
*/