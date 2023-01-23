#include <bits/stdc++.h>
using namespace std;


// constants

const int NUM_SHUFFLE = 100;
const int OP[4][2] = {{1,0},{0,1},{-1,0},{0,-1}};
const int GOAL[3][3] = {{1,2,3},{4,5,6},{7,8,0}};


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

array<int,2> getblank(const array<array<int,3>,3>& game){
    for(int i = 0; i < 3; ++i)
        for(int j = 0; j < 3; ++j)
            if(game[i][j] == 0) return {i,j};
    cout << "ERROR: blank not found" << endl;
    exit(1);
}

// main search

size_t pq_max_size = 0;
int num_explored = 0;

bool isgoal(state* s){
    for(int i = 0; i < 3; ++i)
        for(int j = 0; j < 3; ++j)
            if(s->game[i][j] != GOAL[i][j]) return false;
    return true;
}

state* search(state* initial_state){
    auto comp = [](const state* a, const state* b){
        return (a->gn + a->hn) > (b->gn + b->hn);
    };
    priority_queue<state*,vector<state*>,decltype(comp)> pq(comp);
    set<array<array<int,3>,3>> explored;
    pq.push(initial_state);
    while(!pq.empty()){
        state* curr = pq.top();
        array<int,2> blanktile = getblank(curr->game);
        pq.pop();
        if(isgoal(curr)){
            return curr;
        }
        if(explored.count(curr->game)) continue;
        explored.insert(curr->game);
        num_explored++;
        for(int i = 0; i < 4; ++i){
            if(check(blanktile[0]+OP[i][0],blanktile[1]+OP[i][1])){
                array<array<int,3>,3> new_game = curr->game;
                swap(new_game[blanktile[0]][blanktile[1]],new_game[blanktile[0]+OP[i][0]][blanktile[1]+OP[i][1]]);
                state* new_state = new state(curr, new_game, curr->gn + 1, 0);  // add heuristic in, probably use function pointer
                pq.push(new_state);
            }
        }
        pq_max_size = max(pq_max_size,pq.size());
    }
    cout << "FAILURE" << endl;
    return nullptr;
}


// backtracking

void recover(state* s){
    stack<array<array<int,3>,3>> game_stack;
    while(s){
        game_stack.push(s->game);
        s = s->parent;
    }
    int move_count = 0;
    while(!game_stack.empty()){
        cout << "move " << move_count++ << ":" << endl;
        printgame(game_stack.top());
        game_stack.pop();
    }
}

int main(){
    srand(time(0));
    state* initial_state = new state();
    shuffle(initial_state->game);
    state* sol = search(initial_state);
    recover(sol);
    cout << "frontier max size: " << pq_max_size << endl;
    cout << "num_nodes_explored: " << num_explored << endl;
    return 0;
}