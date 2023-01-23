#include <bits/stdc++.h>
using namespace std;


// constants

const int NUM_SHUFFLE = 100;
const int OP[4][2] = {{1,0},{0,1},{-1,0},{0,-1}};
array<array<int,3>,3> GOAL = {{{1,2,3},{4,5,6},{7,8,0}}};


// state definition

struct state{
    state* parent;
    array<array<int,3>,3> game;
    int gn, hn;
    state() : parent(nullptr), gn(0), hn(0){
        for(int i = 0; i < 3; ++i)
            for(int j = 0; j < 3; ++j)
                this->game[i][j] = GOAL[i][j];
    }
    state(state* parent, array<array<int,3>,3> game, int gn, int hn){
        this->parent = parent;
        this->gn = gn;
        this->hn = hn;
        for(int i = 0; i < 3; ++i)
            for(int j = 0; j < 3; ++j)
                this->game[i][j] = game[i][j];
    }
};


// utility

bool check(int x, int y){
    return x  < 3 && x >= 0 && y  < 3 && y >= 0;
}

void shuffle(array<array<int,3>,3>& game){
    int x = 2, y = 2;
    for(int i = 0; i < NUM_SHUFFLE; ++i){
        int choice = rand() % 4;
        while(!check(x+OP[choice][0],y+OP[choice][1])) choice = rand() % 4;
        swap(game[x][y],game[x+OP[choice][0]][y+OP[choice][1]]);
        x = x + OP[choice][0];
        y = y + OP[choice][1];
    }
}

void printgame(const array<array<int,3>,3>& game){
    for(int i = 0; i < 3; ++i){
        cout << "\t";
        for(int j = 0; j < 3; ++j){
            cout << game[i][j] << " ";
        }
        cout << endl;
    }
}

array<int,2> get_tile(const array<array<int,3>,3>& game,int target){
    for(int i = 0; i < 3; ++i)
        for(int j = 0; j < 3; ++j)
            if(game[i][j] == target) return {i,j};
    cout << "ERROR: blank not found" << endl;
    exit(1);
}


// heuristics

int uniform_cost(const array<array<int,3>,3>& game){
    return 0;
}

int astar_misplaced_tile(const array<array<int,3>,3>& game){
    int num_misplaced = 0;
    for(int i = 0; i < 3; ++i)
        for(int j = 0; j < 3; ++j)
            if(game[i][j] != GOAL[i][j]) num_misplaced++;
    return 2 * num_misplaced;  // return 2 * num_misplaced instead of num_misplaced; penalizes harder for having more misplaced tiles
}

int astar_euclidean_distance(const array<array<int,3>,3>& game){
    int edist = 0;
    for(int i = 0; i < 3; ++i){
        for(int j = 0; j < 3; ++j){
            int num = game[i][j];
            array<int,2> cd1 = get_tile(game,num);
            array<int,2> cd2 = get_tile(GOAL,num);
            edist += abs(cd1[0]-cd2[0]) + abs(cd1[1]-cd2[1]);
        }
    }
    return edist;
}

// main search

size_t pq_max_size = 0;
int num_iterations = 0;

bool isgoal(state* s){
    for(int i = 0; i < 3; ++i)
        for(int j = 0; j < 3; ++j)
            if(s->game[i][j] != GOAL[i][j]) return false;
    return true;
}

state* search(state* initial_state,int (*heuristic)(const array<array<int,3>,3>&)){
    auto comp = [](const state* a, const state* b){
        return (a->gn + a->hn) > (b->gn + b->hn);
    };
    priority_queue<state*,vector<state*>,decltype(comp)> pq(comp);
    set<array<array<int,3>,3>> inserted;
    pq.push(initial_state);
    while(!pq.empty()){
        num_iterations++;
        state* curr = pq.top();
        array<int,2> blanktile = get_tile(curr->game,0);
        pq.pop();
        if(isgoal(curr)){
            cout << "SUCCESS" << endl;
            return curr;
        }
        for(int i = 0; i < 4; ++i){
            array<array<int,3>,3> new_game;
            if(check(blanktile[0]+OP[i][0],blanktile[1]+OP[i][1])){
                new_game = curr->game;
                swap(new_game[blanktile[0]][blanktile[1]],new_game[blanktile[0]+OP[i][0]][blanktile[1]+OP[i][1]]);
                if(!inserted.count(new_game)){
                    state* new_state = new state(curr, new_game, curr->gn + 1, heuristic(new_game));  // add heuristic in, probably use function pointer
                    pq.push(new_state);
                    inserted.insert(new_game);
                }
            }
        }
        pq_max_size = max(pq_max_size,pq.size());
    }
    cout << "FAILURE" << endl;
    return nullptr;
}


// backtracking

int recover(state* s){
    stack<array<array<int,3>,3>> game_stack;
    while(s){
        game_stack.push(s->game);
        s = s->parent;
    }
    int move_count = 0, depth = game_stack.size()-1;
    while(!game_stack.empty()){
        cout << "move " << move_count++ << ":" << endl;
        printgame(game_stack.top());
        game_stack.pop();
    }
    return depth;
}

int main(){
    srand(time(0));
    int game_choice, search_choice;
    state* initial_state = new state();
    cout << "Select 1) random, or 2) custom: ";
    cin >> game_choice;
    if(game_choice == 1)
        shuffle(initial_state->game);
    else{
        string input;
        cout << "Enter board configuration as a string (eg. 867254301): ";
        cin >> input;
        for(int i = 0; i < 3; ++i)
            for(int j = 0; j < 3; ++j)
                initial_state->game[i][j] = input[i*3+j] - '0';
    }
    cout << "Select 1) uniform cost, 2) a* misplaced tile, 3) a* euclidean: ";
    cin >> search_choice;
    int (*heuristic)(const array<array<int,3>,3>&);
    switch (search_choice){
        case 1:
            heuristic = uniform_cost;
            break;
        case 2:
            heuristic = astar_misplaced_tile;
            break;
        case 3:
            heuristic = astar_euclidean_distance;
            break;
        default:
            cout << "invalid choice" << endl;
            exit(0);
            break;
    }
    state* sol = search(initial_state,heuristic);
    int depth = recover(sol);
    cout << "frontier max size: " << pq_max_size << endl;
    cout << "iteration count: " << num_iterations << endl;
    cout << "depth: " << depth << endl;
    return 0;
}