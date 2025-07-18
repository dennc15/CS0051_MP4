#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <string>
#include <mutex>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <algorithm>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

using namespace std;

// Constants
const int PORT = 8080;
const int MAX_PLAYERS = 3;  // Changed to 3 players
const int ROUNDS_TOTAL = 3;

// Card ranks and suits
vector<string> ranks = {
    "3", "4", "5", "6", "7", "8", "9", "10", "Jack", "Queen", "King", "Ace", "2"
};

vector<string> suits = {
    "Diamonds", "Clubs", "Hearts", "Spades"
};

class PusoyServer {
private:
    int server_fd;
    vector<int> client_sockets;
    vector<string> player_names;
    mutex print_mutex;
    mutex game_mutex;
    
    // Game state
    vector<int> round_wins;
    vector<pair<pair<int, int>, string>> cards_drawn;
    int current_round;
    bool game_active;
    
public:
    PusoyServer() : current_round(1), game_active(false) {
        client_sockets.resize(MAX_PLAYERS, -1);
        player_names.resize(MAX_PLAYERS);
        round_wins.resize(MAX_PLAYERS, 0);
        cards_drawn.resize(MAX_PLAYERS);
        srand(time(0));
    }
    
    ~PusoyServer() {
        cleanup();
    }
    
    void cleanup() {
        for (int sock : client_sockets) {
            if (sock != -1) {
                close(sock);
            }
        }
        if (server_fd != -1) {
            close(server_fd);
        }
    }
    
    bool initialize() {
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd == -1) {
            perror("Socket creation failed");
            return false;
        }
        
        int opt = 1;
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
            perror("Setsockopt failed");
            return false;
        }
        
        struct sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(PORT);
        
        if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
            perror("Bind failed");
            return false;
        }
        
        if (listen(server_fd, MAX_PLAYERS) < 0) {
            perror("Listen failed");
            return false;
        }
        
        cout << "Server started on port " << PORT << endl;
        cout << "Waiting for " << MAX_PLAYERS << " players to connect..." << endl;
        
        return true;
    }
    
    void send_message(int client_socket, const string& message) {
        string msg = message + "\n";
        send(client_socket, msg.c_str(), msg.length(), 0);
    }
    
    void broadcast_message(const string& message) {
        for (int i = 0; i < MAX_PLAYERS; ++i) {
            if (client_sockets[i] != -1) {
                send_message(client_sockets[i], message);
            }
        }
    }
    
    string receive_message(int client_socket) {
        char buffer[1024] = {0};
        int valread = read(client_socket, buffer, 1024);
        if (valread <= 0) {
            cout << "Client disconnected or error reading from socket" << endl;
            return "";
        }
        
        string message(buffer, valread);
        // Remove newline characters
        message.erase(remove(message.begin(), message.end(), '\n'), message.end());
        message.erase(remove(message.begin(), message.end(), '\r'), message.end());
        
        return message;
    }
    
    void accept_connections() {
        struct sockaddr_in address;
        int addrlen = sizeof(address);
        
        for (int i = 0; i < MAX_PLAYERS; ++i) {
            int new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
            if (new_socket < 0) {
                perror("Accept failed");
                continue;
            }
            
            client_sockets[i] = new_socket;
            
            // Get player name
            send_message(new_socket, "ENTER_NAME");
            string name = receive_message(new_socket);
            player_names[i] = name.empty() ? "Player " + to_string(i + 1) : name;
            
            cout << player_names[i] << " connected (Player " << i + 1 << ")" << endl;
            
            string welcome_msg = "WELCOME:" + player_names[i] + ":" + to_string(i + 1);
            send_message(new_socket, welcome_msg);
            
            // Notify all players about new connection
            string notification = "PLAYER_JOINED:" + player_names[i] + ":" + to_string(i + 1) + ":" + to_string(i + 1) + "/" + to_string(MAX_PLAYERS);
            broadcast_message(notification);
        }
        
        cout << "All players connected! Starting game..." << endl;
        broadcast_message("GAME_START");
    }
    
    int generate_random_rank() {
        return rand() % 13 + 1;
    }
    
    int generate_random_suit() {
        return rand() % 4 + 1;
    }
    
    string display_card(int rank_num, int suit_val) {
        return ranks[rank_num - 1] + " of " + suits[suit_val - 1];
    }
    
    void play_round() {
        cout << "Starting Round " << current_round << endl;
        
        broadcast_message("ROUND_START:" + to_string(current_round));
        this_thread::sleep_for(chrono::milliseconds(500));
        
        // Reset cards for this round
        for (int i = 0; i < MAX_PLAYERS; ++i) {
            cards_drawn[i] = {{0, 0}, "Pass"};
        }
        
        // Get each player's choice
        for (int i = 0; i < MAX_PLAYERS; ++i) {
            if (client_sockets[i] != -1) {
                cout << "Waiting for " << player_names[i] << "'s choice..." << endl;
                
                // Send turn message
                send_message(client_sockets[i], "YOUR_TURN:" + to_string(current_round));
                
                // Wait for response with timeout handling
                string choice = "";
                int attempts = 0;
                while (choice.empty() && attempts < 3) {
                    choice = receive_message(client_sockets[i]);
                    if (choice.empty()) {
                        cout << "No response from " << player_names[i] << ", retrying..." << endl;
                        this_thread::sleep_for(chrono::milliseconds(1000));
                        attempts++;
                    }
                }
                
                if (choice.empty()) {
                    cout << "Player " << player_names[i] << " did not respond, treating as pass" << endl;
                    choice = "2";
                }
                
                cout << player_names[i] << " chose: " << choice << endl;
                
                if (choice == "1") {
                    // Draw a card
                    int rank_num = generate_random_rank();
                    int suit_val = generate_random_suit();
                    cards_drawn[i] = {{rank_num, suit_val}, display_card(rank_num, suit_val)};
                    
                    string card_msg = "CARD_DRAWN:" + player_names[i] + ":" + display_card(rank_num, suit_val);
                    broadcast_message(card_msg);
                    
                    cout << player_names[i] << " drew: " << display_card(rank_num, suit_val) << endl;
                } else {
                    // Pass
                    cards_drawn[i] = {{0, 0}, "Pass"};
                    string pass_msg = "PLAYER_PASSED:" + player_names[i];
                    broadcast_message(pass_msg);
                    
                    cout << player_names[i] << " passed" << endl;
                }
                
                // Small delay between players
                this_thread::sleep_for(chrono::milliseconds(500));
            }
        }
        
        // Small delay before evaluation
        this_thread::sleep_for(chrono::milliseconds(1000));
        
        // Evaluate round winner
        evaluate_round();
    }
    
    void evaluate_round() {
        broadcast_message("EVALUATING");
        this_thread::sleep_for(chrono::milliseconds(1000));
        
        cout << "\nEvaluating Round " << current_round << "..." << endl;
        
        // Show all cards
        for (int i = 0; i < MAX_PLAYERS; ++i) {
            if (cards_drawn[i].first.first > 0) {
                cout << "[" << player_names[i] << "] has " << cards_drawn[i].second << endl;
            } else {
                cout << "[" << player_names[i] << "] passed the turn" << endl;
            }
        }
        
        // Find the winner
        int winner = -1;
        pair<int, int> best = {0, 0};
        
        for (int i = 0; i < MAX_PLAYERS; ++i) {
            if (cards_drawn[i].first.first > 0 && cards_drawn[i].first > best) {
                best = cards_drawn[i].first;
                winner = i;
            }
        }
        
        if (winner != -1) {
            round_wins[winner]++;
            string winner_msg = "ROUND_WINNER:" + player_names[winner] + ":" + to_string(winner + 1);
            broadcast_message(winner_msg);
            
            cout << "Round " << current_round << " winner: " << player_names[winner] << endl;
        } else {
            broadcast_message("ROUND_DRAW");
            cout << "Round " << current_round << " ended in a draw" << endl;
        }
        
        // Increment round counter
        current_round++;
    }
    
    void evaluate_game_winner() {
        lock_guard<mutex> lock(game_mutex);
        
        broadcast_message("GAME_FINISHED");
        
        // Send round results
        for (int i = 0; i < MAX_PLAYERS; ++i) {
            string result = "PLAYER_ROUNDS:" + player_names[i] + ":" + to_string(round_wins[i]);
            broadcast_message(result);
        }
        
        // Find game winner(s)
        int most_rounds_won = *max_element(round_wins.begin(), round_wins.end());
        vector<int> game_winners;
        
        for (int i = 0; i < MAX_PLAYERS; ++i) {
            if (round_wins[i] == most_rounds_won) {
                game_winners.push_back(i);
            }
        }
        
        if (most_rounds_won == 0) {
            broadcast_message("GAME_DRAW");
        } else if (game_winners.size() == 1) {
            string winner_msg = "GAME_WINNER:" + player_names[game_winners[0]];
            broadcast_message(winner_msg);
        } else {
            string tie_msg = "GAME_TIE:";
            for (int i = 0; i < game_winners.size(); ++i) {
                tie_msg += player_names[game_winners[i]];
                if (i < game_winners.size() - 1) tie_msg += ",";
            }
            broadcast_message(tie_msg);
        }
    }
    
    void run_game() {
        accept_connections();
        
        game_active = true;
        
        cout << "\n=== STARTING PUSOY CLASH GAME ===" << endl;
        cout << "Players: ";
        for (int i = 0; i < MAX_PLAYERS; ++i) {
            cout << player_names[i];
            if (i < MAX_PLAYERS - 1) cout << ", ";
        }
        cout << endl;
        
        // Add delay before starting
        this_thread::sleep_for(chrono::seconds(2));
        
        // Play all rounds
        for (int round = 1; round <= ROUNDS_TOTAL; ++round) {
            cout << "\n========== ROUND " << round << " ==========" << endl;
            current_round = round;  // Set current round before playing
            play_round();
            
            // Show current scores after each round
            cout << "\nCurrent Scores:" << endl;
            for (int i = 0; i < MAX_PLAYERS; ++i) {
                cout << player_names[i] << ": " << round_wins[i] << " round(s) won" << endl;
            }
        }
        
        // Evaluate final winner
        evaluate_game_winner();
        
        game_active = false;
        
        // Keep server running for a bit to allow clients to see results
        this_thread::sleep_for(chrono::seconds(5));
        
        broadcast_message("SERVER_SHUTDOWN");
    }
};

int main() {
    PusoyServer server;
    
    if (!server.initialize()) {
        cerr << "Failed to initialize server" << endl;
        return 1;
    }
    
    try {
        server.run_game();
    } catch (const exception& e) {
        cerr << "Server error: " << e.what() << endl;
        return 1;
    }
    
    cout << "Server shutting down..." << endl;
    return 0;
}
