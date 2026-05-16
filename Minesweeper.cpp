#include <iostream>
#include <random>

using namespace std;

struct Fill{
    int r;
    int c;
    Fill* next;
};


int** createMap(int row, int col)
{
    int** gameboard = new int*[row];

    for(int i = 0; i < row; i++)
    {
        gameboard[i] = new int[col];
    }
    return gameboard;
}

void addMine(int** mineMap, int r, int c, int d, mt19937& gen)
{
    uniform_int_distribution <> distr(0, (r*c)/d);
    for(int i = 0; i < r; i++) {
        for(int j = 0; j < c; j++) {
            mineMap[i][j] = (distr(gen) == 0);
        }
    }
    return;
}


void checkMine(int** mineMap, int** hintMap, int r, int c)
{
    for(int i = 0; i < r; i++)
    {
        for(int j = 0; j < c; j++)
        {
            int counter = 0;

            for(int x = -1; x <= 1; x++)
            {
                for(int y = -1; y <= 1; y++)
                {
                    int n_r = i + x;
                    int n_c = j + y;

                    if((n_r >= 0 && n_r < r && n_c >= 0 && n_c < c) ||
                    (n_r == 0 &&  n_c >= 0 && n_c < c)|| (n_c == 0 && n_r >= 0 && n_r < r))
                    {
                        if(x == 0 && y == 0)
                        {
                            continue;
                        }
                        if(mineMap[n_r][n_c] == 1)
                        {
                            counter++;
                        }
                    }
                }
                
            }
            hintMap[i][j] = counter;
        }
    }
}

void floodFill(int** mineMap, int r, int c, int x, int y, int head)
{
    
}



void displayMap(int** gameboard, int r, int c)
{   
    for(int i = 0; i < r; i++) {
        for(int j = 0; j < c; j++)
        {
            cout << gameboard[i][j] << " ";
        }
        cout << endl;
    }
    return;
}

void deleteMap(int** gameboard, int r)
{
    for(int i = 0; i < r; i++) {
        delete[] gameboard[i];
    }
    delete[] gameboard;
}

int main()
{
    random_device rd;
    mt19937 gen(rd());

    int r = 9, c = 9;

    int mine_value = 10;

    int** mineMap = createMap(r,c);
    addMine(mineMap, r, c, mine_value, gen);
    cout << "mineMap" << endl;
    displayMap(mineMap, r, c);

    cout << endl;

    int** hintMap = createMap(r,c);
    checkMine(mineMap, hintMap, r, c);
    cout << "hintMap" << endl;
    displayMap(hintMap, r, c);

    cout << endl;

    // int** revealState = createMap(r,c);
    // userInterface(revealState, r, c);
    // cout << "revealState" << endl;
    // displayMap(revealState, r, c);
    
    return 0;
}