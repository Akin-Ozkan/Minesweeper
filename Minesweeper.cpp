#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include <chrono>
#include <thread>

using namespace std;

#ifdef _WIN32 //This cursor block created by AI, it is not a project requirement.
    #include <conio.h> // getch() for Windows
    char catchButton() {
        return _getch();
    }
#else
    #include <termios.h> // Terminal settings for Linux
    #include <unistd.h>
    char catchButton() {
        char buf = 0;
        struct termios old = {0};
        if (tcgetattr(0, &old) < 0) perror("tcsetattr()");
        old.c_lflag &= ~ICANON;
        old.c_lflag &= ~ECHO;
        old.c_cc[VMIN] = 1;
        old.c_cc[VTIME] = 0;
        if (tcsetattr(0, TCSANOW, &old) < 0) perror("tcsetattr ~ICANON");
        if (read(0, &buf, 1) < 0) perror("read()");
        old.c_lflag |= ICANON;
        old.c_lflag |= ECHO;
        if (tcsetattr(0, TCSANOW, &old) < 0) perror("tcsetattr ICANON");
        return (buf);
    }
#endif

struct Fill{
    int x_c;
    int y_c;
    Fill* next;
};

void waitSeconds(int s);
int** createMap(int row, int col);
void addMine(int** mineMap, int r, int c, int d, mt19937& gen);
void checkMine(int** mineMap, int** hintMap, int r, int c);
void enqueue(struct Fill** top, struct Fill** tail, int y, int x);
void dequeue(Fill** top, Fill** tail);
int checkState(int** hintMap, int** revealState, int r, int c, int x, int y, Fill** top, Fill** tail);
int floodFill(int** hintMap, int** revealState, int r, int c, Fill** top, Fill** tail);
void displayMines(int** gameboard, int r, int c);
void displayMap(int** mineMap, int** hintMap, int** revealState, int r, int c, int cY, int cX);
void deleteMap(int** gameboard, int r);
int starter(int level);
void clearScreen();

int main()
{
    random_device rd;
    mt19937 gen(rd());

    fstream file("user.txt", ios::app);
    file.close();

    string active_user = "";
    string active_pass = "";
    bool game_loaded = false;

    int loaded_reveal = 0;
    int level = 1;
    int r, c, mine_percent;
    int  fail_login = 0;

    while(1)
    {
        fstream file("user.txt", ios::in | ios::out);

        if(file.is_open())
        {
            cout << endl << "User registration file is opened. Warning! User Name system is not case-sensitive." << endl;
        }

        int if_login;
        cout << "1)Register\n2)Log in\n";
        cin >> if_login;

        if(if_login == 2)
        {
            file.seekg(0);
            if(file.peek() == ifstream::traits_type::eof())
            {
                cout << endl << "User not found, please create a new user." << endl;
                continue;
            }
            else
            {
                cin.ignore();
                string input_n, input_p;
                cout << "User Name: ";
                getline(cin, input_n);
                cout << "Password: ";
                getline(cin, input_p);

                file.seekg(0);
                string n, p;
                getline(file, n);
                getline(file, p);

                if(input_n == n && input_p == p)
                {
                    cout << endl << "Login Successful!" << endl;
                    active_user = n;
                    active_pass = p;
                    
                    file >> level;

                    if(file >> r >> c >> loaded_reveal >> mine_percent)
                    {
                        game_loaded = true; 
                        cout << "Saved game found! Loading the game" << endl;
                    }

                    else
                    {
                        cout << "No saved game. Starting a new game." << endl;
                    }
                    waitSeconds(2);
                    file.close();
                    break;
                }

                else
                {
                    cout << endl << "Invalid username or password!" << endl;
                    waitSeconds(1);
                    file.close();
                    fail_login++;
                    if(fail_login == 3)
                    {
                        file.close();
                        fstream file("user.txt", ios::trunc);
                        file.close();
                        cout << "Too many invalid tries, user deleted." << endl;
                        waitSeconds(1);
                        fail_login = 0;
                    }
                    continue;
                }   
            }
        }
        else
        {
            cin.ignore();
            file.close();
            file.open("user.txt", ios::in | ios::out);
            
            string n, p, registered_n;
            cout << "User Name: ";
            getline(cin, n);
            getline(file, registered_n);

            if(n == registered_n)
            {
                cout << "This user is already registered. Log in or create a new account." << endl;
                file.close();
                waitSeconds(2);
                continue;
            }
            file.close();
            file.open("user.txt", ios::in | ios::out | ios::trunc);
            file << n << endl;
            cout << "Password: ";
            getline(cin, p);
            file << p << endl;
            file << 1 << endl;
            file.close();

            active_user = n;
            active_pass = p;
            break;
        }
    }

    while(1)
    {
        clearScreen();

        int** mineMap = nullptr;
        int** hintMap = nullptr;
        int** revealState = nullptr;

        int reveal = 0;
        int last_reveal = 0;

        if(!game_loaded)
        {
            level = starter(level);
            switch(level)
            {
            case 1: r = 10; c = 10; mine_percent = 10; break;
            case 2: r = 15; c = 15; mine_percent = 16; break;
            case 3: r = 20; c = 20; mine_percent = 21; break;
            }

            mineMap = createMap(r,c);
            addMine(mineMap, r, c, mine_percent, gen);
            hintMap = createMap(r,c);
            checkMine(mineMap, hintMap, r, c);
            revealState = createMap(r,c);

            last_reveal = (r*c) - ((r*c)*mine_percent/100);
            reveal = 0;
        }

        else
        {
            ifstream load("user.txt");
            load.seekg(0);
            string skip;
            getline(load, skip);
            getline(load, skip);
            getline(load, skip);
            getline(load, skip);

            mineMap = createMap(r, c);
            hintMap = createMap(r, c);
            revealState = createMap(r, c);

            for(int i = 0; i < r; i++)
            {
                for(int j = 0; j < c; j++)
                {
                    load >> mineMap[i][j];
                }
            }

            for(int i = 0; i < r; i++)
            {
                for(int j = 0; j < c; j++)
                {
                    load >> hintMap[i][j];
                }
            }

            for(int i = 0; i < r; i++)
            {
                for(int j = 0; j < c; j++)
                {
                    load >> revealState[i][j];
                }
            }

            load.close();

            reveal = loaded_reveal;
            last_reveal = (r * c) - ((r * c) * mine_percent / 100);
            game_loaded = false;
        }

        int cursorY = 0;
        int cursorX = 0;
        
        bool continue_game = true;

        while(continue_game)
        {
            clearScreen();


            displayMap(mineMap, hintMap, revealState, r, c, cursorY, cursorX);


            cout << "Movement: WASD | Flag: F | Open cell: Space" << endl;
            char button = catchButton();

            switch(button)
            {
                case 'w': case 'W':
                    if (cursorY > 0) cursorY--;
                    break;
                case 's': case 'S':
                    if (cursorY < r - 1) cursorY++;
                    break;
                case 'a': case 'A':
                    if (cursorX > 0) cursorX--;
                    break;
                case 'd': case 'D':
                    if (cursorX < c - 1) cursorX++;
                    break;
                case 'f': case 'F':
                    if(revealState[cursorY][cursorX] == 0)
                    {       
                        revealState[cursorY][cursorX] = 2;
                    }
                    else if(revealState[cursorY][cursorX] == 2)
                    {
                        revealState[cursorY][cursorX] = 0;
                    }
                    break;
                case ' ':

                    if(mineMap[cursorY][cursorX] == 1)
                    {
                        clearScreen();
                        cout << "Game Over!" << endl;
                        displayMines(mineMap, r, c);
                        waitSeconds(2);
                        cout << endl << endl;

                        ofstream cleanFile("user.txt", ios::trunc);
                        cleanFile << active_user << endl << active_pass << endl;
                        cleanFile << level << endl;
                        cleanFile.close();
                        
                        continue_game = false;
                    }

                    else
                    {
                        if(revealState[cursorY][cursorX] == 1)
                        {
                            cout << "Block already opened." << endl;
                            waitSeconds(2);
                            continue;
                        }
                        else if(revealState[cursorY][cursorX] == 2)
                        {
                            cout << "Block has a flag." << endl;
                            waitSeconds(2);
                            continue;
                        }
                        Fill* top = new Fill;
                        Fill* tail = top;
                        top->x_c = cursorX;
                        top->y_c = cursorY;
                        top->next = nullptr;
                        reveal += floodFill(hintMap, revealState, r, c, &top, &tail);
                        if(last_reveal != reveal)
                        {
                            displayMap(mineMap, hintMap, revealState, r, c, cursorY, cursorX);
                        }
                        else
                        {
                            cout << "Win!" << endl;
                            displayMap(mineMap, hintMap, revealState, r, c, cursorY, cursorX);
                            waitSeconds(2);

                            if(level <= 2)
                            {
                                level++;
                            }

                            ofstream cleanFile("user.txt", ios::trunc);
                            cleanFile << active_user << endl << active_pass << endl;
                            cleanFile << level << endl;

                            continue_game = false;
                        }
                    }
                    break;
                case 'e': case 'E':
                {
                    clearScreen();
                    cout << "Saving game and safely exiting..." << endl;
                
                    ofstream saveFile("user.txt", ios::trunc);
                    if(saveFile.is_open())
                    {
                        saveFile << active_user << endl << active_pass << endl;
                        saveFile << level << endl;
                        saveFile << r << " " << c << " " << reveal << " " << mine_percent << endl;
                                                
                        for(int i=0; i<r; i++) {
                            for(int j=0; j<c; j++) saveFile << mineMap[i][j] << " ";
                            saveFile << endl;
                        }
                        for(int i=0; i<r; i++) {
                            for(int j=0; j<c; j++) saveFile << hintMap[i][j] << " ";
                            saveFile << endl;
                        }
                        for(int i=0; i<r; i++) {
                            for(int j=0; j<c; j++) saveFile << revealState[i][j] << " ";
                            saveFile << endl;
                        }
                        saveFile.close();
                    }
                    deleteMap(mineMap, r);
                    deleteMap(hintMap, r);
                    deleteMap(revealState, r);
                    return 0;
                }
            }
        }

        deleteMap(mineMap, r);
        deleteMap(hintMap, r);
        deleteMap(revealState, r);
    }
}

void waitSeconds(int s)
{
    this_thread::sleep_for(chrono::seconds(s));
}

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

                    if(n_r >= 0 && n_r < r && n_c >= 0 && n_c < c)
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

int checkState(int** hintMap, int** revealState, int r, int c, int x, int y, Fill** top, Fill** tail)
{
    int counter = 0;
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
                    counter++;
                    if(hintMap[n_r][n_c] == 0) enqueue(top, tail, n_r, n_c);
                }
            }
        } 
    }
    return counter;    
}

int floodFill(int** hintMap, int** revealState, int r, int c, Fill** top, Fill** tail)
{
    int counter = 0;
    revealState[(*top)->y_c][(*top)->x_c] = 1;
    counter++;

    if(hintMap[(*top)->y_c][(*top)->x_c] != 0)
    {
        dequeue(top, tail);
        return counter;
    }

    while( (*top) != nullptr )
    {
        int currentX = (*top)->x_c;
        int currentY = (*top)->y_c;

        counter += checkState(hintMap, revealState, r, c, currentX, currentY, top, tail);
        dequeue(top, tail);
    }
    return counter;
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

void displayMap(int** mineMap, int** hintMap, int** revealState, int r, int c, int cY, int cX)
{
    for(int i = 0; i < r; i++)
    {
        for(int j = 0; j < c; j++)
        {
            if(i == cY && j == cX)
            {
                cout << ">";
            }
            else cout << " ";

            if(revealState[i][j] == 1)
            {
                cout << hintMap[i][j];
            }
            else if(revealState[i][j] == 2)
            {
                cout << "F";
            }
            else
            {
                cout << ".";
            }
            if(i == cY && j == cX)
            {
                cout << "<";
            }
            else cout << " ";

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
        if(level < x) cout << "You can choose maximum level " << level << endl << endl << endl;
        else return x;
    }
}

void clearScreen()
{
    cout << "\033[2J\033[H" << flush;   
}