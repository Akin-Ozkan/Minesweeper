#include <iostream>
#include <random>

using namespace std;

struct Fill{
    int x_c;
    int y_c;
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

void enqueue(struct Fill** top, struct Fill** tail, int y, int x)
{
    Fill* temp = new Fill;

    temp->x_c = x;
    temp->y_c = y;
    temp->next = nullptr;

    if(*top == nullptr)
    {
        *top = *tail = temp;
        return;
    }

    (*tail)->next = temp;
    *tail = temp;
    
    
    return;
}

void dequeue(Fill** top, Fill** tail)
{
    if (*top == nullptr)
    {
        return;
    }

    Fill* temp = *top;
    
    if(*top == *tail)
    {
        *top = *tail = nullptr;
    }
    else *top = temp->next;
    delete temp;
    return;
}

void floodFill(int** hintMap, int** revealState, int r, int c, Fill** top, Fill** tail)
{
    while( (*top) != nullptr )
    {
        int currentX = (*top)->x_c;
        int currentY = (*top)->y_c;

        if(currentX != 0 && hintMap[currentY][currentX - 1] == 0)
        {
            if(revealState[currentY][currentX - 1] == 0)
            {
                revealState[currentY][currentX - 1] = 1;
                enqueue(top, tail, currentY, currentX - 1);
            }
        }

        if(currentY != 0 && hintMap[currentY - 1][currentX] == 0 )
        {
            if(revealState[currentY - 1][currentX] == 0)
            {
                revealState[currentY - 1][currentX] = 1;
                enqueue(top, tail, currentY - 1, currentX);
            }
        }

        if(currentX != (c - 1) && hintMap[currentY][currentX + 1] == 0 )
        {
           if(revealState[currentY][currentX + 1] == 0)
            {
                revealState[currentY][currentX + 1] = 1;
                enqueue(top, tail, currentY, currentX + 1);
            }
        }

        if(currentY != (r - 1) && hintMap[currentY + 1][currentX] == 0 )
        {
           if(revealState[currentY + 1][currentX] == 0)
            {
                revealState[currentY + 1][currentX] = 1;
                enqueue(top, tail, currentY + 1, currentX);
            }
        }

        dequeue(top, tail);
    }
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

    Fill* top = new Fill;
    top->x_c = 4;
    top->y_c = 5;
    top->next = nullptr;
    Fill* tail = top;
    
    // int** revealState = createMap(r,c);
    // userInterface(revealState, r, c);
    // cout << "revealState" << endl;
    // displayMap(reveal((*head)->x_c)]State, r, c);
    
    return 0;
}