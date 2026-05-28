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
    for(int i = 0; i < r; i++) {
        for(int j = 0; j < c; j++) {
            mineMap[i][j] = 0;
        }
    }

    int targetMines = (r * c) * d / 100; 
    int placedMines = 0;

    uniform_int_distribution<> randRow(0, r - 1);
    uniform_int_distribution<> randCol(0, c - 1);

    while(placedMines < targetMines)
    {
        int row = randRow(gen);
        int col = randCol(gen);

        if(mineMap[row][col] == 0)
        {
            mineMap[row][col] = 1;
            placedMines++;
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
    return;
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

void checkState(int** hintMap, int** revealState, int r, int c, int x, int y, Fill** top, Fill** tail)
{
    for(int i = -1; i <= 1; i++)
    {
        for(int j = -1; j <= 1; j++)
        {

            int n_r = y + i;
            int n_c = x + j;

            if(n_r >= 0 && n_r < r && n_c >= 0 && n_c < c)
            {
                if(i == 0 && j == 0) continue;

                if(revealState[n_r][n_c] == 0)
                {
                    revealState[n_r][n_c] = 1;
                    if(hintMap[n_r][n_c] == 0) enqueue(top, tail, n_r, n_c);
                }
            }
        } 
    }
}




void floodFill(int** hintMap, int** revealState, int r, int c, Fill** top, Fill** tail)
{
    int counter = 0;
    revealState[(*top)->y_c][(*top)->x_c] = 1;

    if(hintMap[(*top)->y_c][(*top)->x_c] != 0)
    {
        dequeue(top, tail);
        return;
    }

    while( (*top) != nullptr )
    {
        int currentX = (*top)->x_c;
        int currentY = (*top)->y_c;

        checkState(hintMap, revealState, r, c, currentX, currentY, top, tail);
        dequeue(top, tail);
    }
    return;
}



void displayMines(int** gameboard, int r, int c)
{   
    for(int i = 0; i < r; i++) {
        for(int j = 0; j < c; j++)
        {
            if(gameboard[i][j] == 1)
            {
                cout << "*" << " ";
            }
            else cout << gameboard[i][j] << " ";
            
        }
        cout << endl;
    }
    return;
}

void displayMap(int** mineMap, int** hintMap, int** revealState, int r, int c)
{
    for(int i = 0; i < r; i++)
    {
        for(int j = 0; j < c; j++)
        {
            if(revealState[i][j] == 1)
            {
                cout << hintMap[i][j];
            }
            else if(revealState[i][j] == 2)
            {
                cout << "F";
            }
            else cout << ".";
            cout << " ";
        }
        cout << endl;
    }
    cout << endl << endl;
}

void deleteMap(int** gameboard, int r)
{
    for(int i = 0; i < r; i++) {
        delete[] gameboard[i];
    }
    delete[] gameboard;
}

int starter(int level)
{
    while(1) // Level lock
    {
        int x;
        cout << ":::::Minesweeper:::::" << endl << endl;
        cout << "Choose level: " << endl << "Level 1: Easy" << endl << "Level 2: Normal" << endl << "Level 3: Hard" << endl << "Level ";
        cin >> x;
        if(level < x) cout << "You can choose level less than" << level << endl << endl << endl;
        else return x;
    }
}

void clearScreen()
{
    
}

int main()
{
    random_device rd;
    mt19937 gen(rd());


    int level = 1;
    int r, c, mine_percent;

    while(1)
    {
        cout << "\033[2J\033[H" << flush;
        level = starter(level);
        switch(level)
        {
        case 1:
            r = 10;
            c = 10;
            mine_percent = 10;
            break;
        case 2:
            r = 15;
            c = 15;
            mine_percent = 16;
            break;
        case 3:
            r = 20;
            c = 20;
            mine_percent = 21;
            break;
        }

        int** mineMap = createMap(r,c);
        addMine(mineMap, r, c, mine_percent, gen);
        int** hintMap = createMap(r,c);
        checkMine(mineMap, hintMap, r, c);
        int** revealState = createMap(r,c);

        while(1)
        {
            cout << "\033[2J\033[H" << flush;
            int last_reveal = (r*c) - (r*c) * (mine_percent/100);
            int x, y, p, counter = 0;

            displayMap(mineMap, hintMap, revealState, r, c);

            cout << "x y p: ";
            cin >> x;
            cin >> y;
            cin >> p;

            if(p == 0)
            {
                revealState[y][x] = 2;
            }
            else if(p == 1)
            {
                if(mineMap[y][x] == 1)
                {
                    cout << "Game Over!" << endl;
                    displayMines(mineMap, r, c);
                    cout << endl << endl;
                    break;
                }
                else
                {
                    Fill* top = new Fill;
                    Fill* tail = top;
                    top->x_c = x;
                    top->y_c = y;
                    top->next = nullptr;
                    floodFill(hintMap, revealState, r, c, &top, &tail);
                    if(t_reveal != reveal)
                    {
                        displayMap(mineMap, hintMap, revealState, r, c);
                    }
                    
                }
            }
        }

        deleteMap(mineMap, r);
        deleteMap(hintMap,r);
        deleteMap(revealState, r);
    }
   
    return 0;
}

// 1- %10- 10x10
// 2- %16- 15x15
// 3- %21- 20x20