#include <bits/stdc++.h>
using namespace std;

// constants
const int NUM_SHUFFLE = 100;
const int op[4][2] = {{1,0},{0,1},{-1,0},{0,-1}};
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
bool shuffle_check(int x, int y, int choice){
    return x + op[choice][0] < 3 && x + op[choice][0] >=0 && 
            y + op[choice][1] < 3 && y + op[choice][1] >= 0;
}

void shuffle(array<array<int,3>,3> game){
    int x = 2, y = 2;
    for(int i = 0; i < NUM_SHUFFLE; ++i){
        int choice = rand() % 4;
        while(!shuffle_check(x,y,choice)) choice = rand() % 4;
        swap(game[x][y],game[x+op[choice][0]][y+op[choice][1]]);
        x = x + op[choice][0];
        y = y + op[choice][1];
    }
}

void printgame(array<array<int,3>,3> game){
    cout << "--------state--------" << endl;
    for(int i = 0; i < 3; ++i){
        cout << "\t";
        for(int j = 0; j < 3; ++j){
            cout << game[i][j] << " ";
        }
        cout << endl;
    }
}


// main search

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
    set<array<array<int,3>,3>> seen;
    pq.push(initial_state);
    while(!pq.empty()){
        state* curr = pq.top();
        pq.pop();
        if(isgoal(curr)){
            cout << "SUCCESS" << endl;
            printgame(curr->game);
            return curr;
        }
        // todo: expand nodes
    }
    cout << "FAILURE" << endl;
    return nullptr;
}

int main(){
    srand(time(0));
    state* initial_state = new state();
    shuffle(initial_state->game);
    printgame(initial_state->game);
    
    return 0;
}