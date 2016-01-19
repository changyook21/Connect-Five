//#define DEBUG_MODE

//==============================================================================
//==============================================================================
//==============================================================================
// Pre-processor
//==============================================================================
//==============================================================================
//==============================================================================

//------------------------------------------------------------------------------
// #include: to use API
//------------------------------------------------------------------------------
#include <iostream>
#include <iomanip>      /* setw(), setfill(), left, right */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

#if defined(__WIN32__) || defined(_WIN32) || defined(WIN32) || defined(__WINDOWS__) || defined(__TOS_WIN__)
#include <windows.h>
#else  /* presume POSIX */
#include <unistd.h>
#endif 

using namespace std;

//------------------------------------------------------------------------------
// #define: Constants
//------------------------------------------------------------------------------
#define PIECE_EMPTY '.'
#define PIECE_BLACK '@'
#define PIECE_WHITE 'O'

#define TURN_BLACK true
#define TURN_WHITE false

#define WIN_NUM_PIECES 5

#define EXIT_CODE -1

// winCheck() related
#define NUM_DIRS 4

#define ROW_DIR 0
#define COL_DIR 1

// winCheck() related
int DIRS[2][NUM_DIRS] = {
    { 0, -1, -1, -1}, // ROW_DIRS
    {-1,  0, -1,  1}  // COL_DIRS
};

// computer player related
#define EASY 0        // IQ 50
#define NORMAL 1      // IQ 80
#define HARD 2        // > IQ 80

#define EXIT_CODE -1

#define HORZ 0
#define VERT 1
#define DIAG1 2
#define DIAG2 3

#define NEGATIVE 0
#define POSITIVE 1

//------------------------------------------------------------------------------
// Type Definition: to define struct, class..
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Prototypes
//------------------------------------------------------------------------------
char **createBoard(int rowSize, int colSize);
void destroyBoard(char **board, int rowSize, int colSize);
void initBoard(char **board, int rowSize, int colSize);
void loadBoard(char **board, int rowSize, int colSize);
void printBoard(char **board, int rowSize, int colSize);

bool winCheck(char **board, int rowSize, int colSize, int row, int col);
int countNumPieces(char **board, int rowSize, int colSize, int row, int col, int rowDir, int colDir);
int getDirMaxCount(char **board, int rowSize, int colSize, int row, int col);
bool findOneEndDirMaxCount(char **board, int rowSize, int colSize, int row, int col,
                           int maxDir, int randWay, int *nextRow, int *nextCol);

bool comPlayerIQ50(char **board, int rowSize, int colSize, int *row, int *col);
bool comPlayerIQ80(char **board, int rowSize, int colSize, int *row, int *col);
bool comPlayerIQ100(char **board, int rowSize, int colSize, int *row, int *col);

//------------------------------------------------------------------------------
// Global Variables: avoid to use global variables.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Test Cases
//------------------------------------------------------------------------------
#include "testcases.cpp"

//==============================================================================
//==============================================================================
//==============================================================================
// main()
//==============================================================================
//==============================================================================
//==============================================================================
int main(int argc, const char * argv[]) {
    srand((unsigned)time(NULL));
    
    //--------------------------------------------------------------------------
    // check if the user wants computer player.
    //--------------------------------------------------------------------------
    bool comPlayer = false;
    
    char userInput = '\0';
    cout << "Computer Player? (y/N): ";
    cin >> userInput;
    
    if (userInput == 'y' || userInput == 'Y') {
        comPlayer = true;
    }

    //--------------------------------------------------------------------------
    // decide difficulty.
    //--------------------------------------------------------------------------
    int difficulty = EASY;
    while (comPlayer) {
        int userNum = -1;
        cout << "Difficulty? (0) Easy / (1) Normal / (2) Hard / (-1) to exit: ";
        cin >> userNum;
        
        if (userNum >= EASY && userNum <= HARD) {
            difficulty = userNum;
            break;
        }
        
        if (userNum == EXIT_CODE) {
            break;
        }
    }
    
    //--------------------------------------------------------------------------
    // init variables
    //--------------------------------------------------------------------------
    int rowSize = ROW_SIZE;
    int colSize = COL_SIZE;
    int totalNumPieces = rowSize * colSize;
    
    char **board = createBoard(rowSize, colSize);
    
#ifdef TEST_CASE_MODE
    // testcase mode
    loadBoard(board, rowSize, colSize);
#else
    // normal play mode
    initBoard(board, rowSize, colSize);
#endif
    
    printBoard(board, rowSize, colSize);

    int numPieces = 0;

    bool turn = TURN_BLACK;
    
    int row = -1;
    int col = -1;
    
    //--------------------------------------------------------------------------
    // Main (Game) Loop
    //--------------------------------------------------------------------------
    while (true) { // infinite loop
        
        if (comPlayer && turn == TURN_WHITE) {
            if (difficulty == EASY) {
                comPlayerIQ50(board, rowSize, colSize, &row, &col);
            }
            else if (difficulty == NORMAL) {
                comPlayerIQ80(board, rowSize, colSize, &row, &col);
            }
            else if (difficulty == HARD) {
                comPlayerIQ100(board, rowSize, colSize, &row, &col);
            }
            
            #if defined(__WIN32__) || defined(_WIN32) || defined(WIN32) || defined(__WINDOWS__) || defined(__TOS_WIN__)
            Sleep(1000 + (rand() % 200) * 10); // random delay 1 - 3 seconds
            #else  /* presume POSIX */
            usleep(1000000 + (rand() % 200) * 10000); // random delay 1 - 3 seconds
            #endif
        }
        else {
            // get the user coord
            while (true) { // infinite loop
                cout << "Row    (-1 to exit) : "; // prompt
                cin >> row;

                if (row == EXIT_CODE) {
                    break;
                }
                
                cout << "Column (-1 to exit) : ";
                cin >> col;

                if (col == EXIT_CODE) {
                    break;
                }
                
                if (row >= 0 && row < rowSize && col >= 0 && col < colSize &&
                    board[row][col] == PIECE_EMPTY) {
                    break;
                }
                
                cout << "error: invalid coord!!" << endl;
            }
            
            if (row == EXIT_CODE || col == EXIT_CODE) {
                break;
            }
        }
        
        if (turn == TURN_BLACK) {
            board[row][col] = PIECE_BLACK;
        }
        else { // if (turn == TURN_WHITE) {
            board[row][col] = PIECE_WHITE;
        }
        
        printBoard(board, rowSize, colSize);
        if (comPlayer && difficulty == EASY) {
            cout << "EASY: " << endl;
        }
        else if (comPlayer && difficulty == NORMAL) {
            cout << "NORMAL: " << endl;
        }
        else if (comPlayer && difficulty == HARD) {
            cout << "HARD: " << endl;
        }
        
        if (winCheck(board, rowSize, colSize, row, col)) {
            if (turn == TURN_BLACK) {
                cout << "Black wins!!" << endl;
            }
            else {
                cout << "White wins!!" << endl;
            }
            break;
        }
        
        numPieces++;
        
        if (numPieces == totalNumPieces) {
            cout << "The board is full!! Draw!!" << endl;
            break;
        }
        
        turn = !turn; // toggle
    }
    //--------------------------------------------------------------------------
    // end of Main (Game) Loop
    //--------------------------------------------------------------------------

    cout << "Bye!" << endl;
    
    destroyBoard(board, rowSize, colSize);
    
    return 0;
}

//==============================================================================
//==============================================================================
//==============================================================================
// User Defined Functions
//==============================================================================
//==============================================================================
//==============================================================================

bool comPlayerIQ50(char **board, int rowSize, int colSize, int *row, int *col) {
    while (true) { // infinite loop
        int randRowDir = rand() % 3 - 1; // -1, 0, or 1
        int randColDir = rand() % 3 - 1; // -1, 0, or 1
        
        if (board[*row+randRowDir][*col+randColDir] == PIECE_EMPTY) {
            *row = *row+randRowDir;
            *col = *col+randColDir;
            return true;
        }
    }
    return false;
}

bool comPlayerIQ80(char **board, int rowSize, int colSize, int *row, int *col) {
    bool foundEmpty = false;
    for (int i = *row-1; i <= *row+1 && !foundEmpty; i++) {
        for (int j = *col-1; j <= *col+1 && !foundEmpty; j++) {
            if (board[i][j] == PIECE_EMPTY) {
                foundEmpty = true;
            }
        }
    }
    
    if (!foundEmpty) {
        return comPlayerIQ50(board, rowSize, colSize, row, col);
    }
    
    while (true) { // infinite loop
        int randRowDir = rand() % 3 - 1; // -1, 0, or 1
        int randColDir = rand() % 3 - 1; // -1, 0, or 1
        
        if (board[*row+randRowDir][*col+randColDir] == PIECE_EMPTY) {
            *row = *row+randRowDir;
            *col = *col+randColDir;
            return true;
        }
    }

    return false;
}

bool comPlayerIQ100(char **board, int rowSize, int colSize, int *row, int *col) {
#ifdef DEBUG_MODE
    cout << "comPlayerIQ100():" << endl;
#endif
    
    int maxDir = getDirMaxCount(board, rowSize, colSize, *row, *col);
    
#ifdef DEBUG_MODE
    if (maxDir == HORZ) {
        cout << "maxDir: HORZ" << endl;
    }
    else if (maxDir == VERT) {
        cout << "maxDir: VERT" << endl;
    }
    else if (maxDir == DIAG1) {
        cout << "maxDir: DIAG1" << endl;
    }
    else if (maxDir == DIAG2) {
        cout << "maxDir: DIAG2" << endl;
    }
     //exit(1);
#endif
     
    int nextRow = -1;
    int nextCol = -1;
    int randWay = rand() % 2; // NEGATIVE = 0 / POSITIVE = 1
#ifdef DEBUG_MODE
    cout << "comPlayerIQ100(): if (findOneEndDirMaxCount(board, rowSize, colSize, *row, *col, maxDir, randWay, &nextRow, &nextCol)) {" << endl;
#endif
    if (findOneEndDirMaxCount(board, rowSize, colSize, *row, *col, maxDir, randWay, &nextRow, &nextCol)) {
        *row = nextRow;
        *col = nextCol;
#ifdef DEBUG_MODE
        cout << "comPlayerIQ100(): return true;" << endl;
#endif
        return true;
    }
    
#ifdef DEBUG_MODE
    cout << "comPlayerIQ100(): return comPlayerIQ80(board, rowSize, colSize, row, col);" << endl;
#endif
    return comPlayerIQ80(board, rowSize, colSize, row, col);
}

bool findOneEndDirMaxCount(char **board, int rowSize, int colSize, int row, int col,
                           int maxDir, int randWay, int *nextRow, int *nextCol) {
#ifdef DEBUG_MODE
    cout << "findOneEndDirMaxCount(): started" << endl;
#endif
    int rowDir = DIRS[ROW_DIR][maxDir];
    int colDir = DIRS[COL_DIR][maxDir];
    
    if (randWay == POSITIVE) {
        rowDir *= -1;
        colDir *= -1;
    }
    
    for (int i = 0; i < 2; i++) {
        int curRow = row+rowDir;
        int curCol = col+colDir;
#ifdef DEBUG_MODE
        cout << "findOneEndDirMaxCount(): i=" << i << " / curRow=" << curRow << " curCol=" << curCol << endl;
#endif

        while (curRow >= 0 && curRow < rowSize && curCol >= 0 && curCol < colSize &&
               board[curRow][curCol] == board[row][col]) {
            curRow += rowDir;
            curCol += colDir;
        }
        
        // if the current position is empty.
        if (curRow >= 0 && curRow < rowSize && curCol >= 0 && curCol < colSize &&
            board[curRow][curCol] == PIECE_EMPTY) {
            *nextRow = curRow;
            *nextCol = curCol;
            return true;
        }
        
        // set opposite way.((negative to positive) or (positive to negative))
        rowDir *= -1;
        colDir *= -1;
    }
    
#ifdef DEBUG_MODE
    cout << "findOneEndDirMaxCount(): end" << endl;
#endif
    return false;
}

// Returns the direction which has the maximum number of pieces on the line.
int getDirMaxCount(char **board, int rowSize, int colSize, int row, int col) {
    int counter[NUM_DIRS] = {0};
    
    for (int i = 0; i < NUM_DIRS; i++) {
        counter[i] = countNumPieces(board, rowSize, colSize, row, col, DIRS[ROW_DIR][i], DIRS[COL_DIR][i]);
    }
    
    int maxDir = HORZ;
    int maxCount = counter[maxDir];
    for (int i = 1; i < NUM_DIRS; i++) {
        int curCount = counter[i];
        if (maxCount < curCount) {
            maxDir = i;
            maxCount = curCount;
        }
    }
    
    return maxDir;
}

bool winCheck(char **board, int rowSize, int colSize, int row, int col) {
    for (int i = 0; i < NUM_DIRS; i++)
        if (countNumPieces(board, rowSize, colSize, row, col, DIRS[ROW_DIR][i], DIRS[COL_DIR][i]) == WIN_NUM_PIECES)
            return true;

    return false;
}

int countNumPieces(char **board, int rowSize, int colSize, int row, int col, int rowDir, int colDir) {
    int count = 1;
    
    for (int i = 0; i < 2; i++) {
        int curRow = row+rowDir;
        int curCol = col+colDir;
        
        while (curRow >= 0 && curRow < rowSize && curCol >= 0 && curCol < colSize &&
               board[curRow][curCol] == board[row][col]) {
            count++;
            
            curRow += rowDir;
            curCol += colDir;
        }
        
        rowDir *= -1;
        colDir *= -1;
    }

    return count;
}

char **createBoard(int rowSize, int colSize) {
    char **board = new char *[rowSize];
    
    for (int i = 0; i < rowSize; i++) {
        board[i] = new char[colSize];
    }

    return board;
}

void destroyBoard(char **board, int rowSize, int colSize) {
    if (board != NULL) {
        // destroy the board
        for (int i = 0; i < rowSize; i++) {
            delete [] board[i];
        }
        
        delete [] board;
    }
}

// pre-condition: board != NULL
void initBoard(char **board, int rowSize, int colSize) {
    for (int i = 0; i < rowSize; i++) {
        for (int j = 0; j < colSize; j++) {
            board[i][j] = PIECE_EMPTY;
        }
    }
}

void loadBoard(char **board, int rowSize, int colSize) {
    for (int i = 0; i < rowSize; i++) {
        for (int j = 0; j < colSize; j++) {
#ifdef TEST_CASE_MODE
            board[i][j] = TEST_CASE[i][j];
#endif
        }
    }
}

// pre-condition: board != NULL
void printBoard(char **board, int rowSize, int colSize) {
    cout << "========================================" << endl;

    cout << "   ";
    for (int j = 0; j < colSize; j++) {
        cout << (j / 10);
    }
    cout << endl;

    cout << "   ";
    for (int j = 0; j < colSize; j++) {
        cout << (j % 10);
    }
    cout << endl;

    cout << "  +";
    for (int j = 0; j < colSize; j++) {
        cout << "-";
    }
    cout << "+" << endl;

    for (int i = 0; i < rowSize; i++) {
        cout << setw(2) << setfill('0') << i << "|";
        for (int j = 0; j < colSize; j++) {
            cout << board[i][j];
        }
        cout << "|" << setw(2) << setfill('0') << i << endl;
    }
    
    cout << "  +";
    for (int j = 0; j < colSize; j++) {
        cout << "-";
    }
    cout << "+" << endl;

    cout << "   ";
    for (int j = 0; j < colSize; j++) {
        cout << (j / 10);
    }
    cout << endl;
    
    cout << "   ";
    for (int j = 0; j < colSize; j++) {
        cout << (j % 10);
    }
    cout << endl;
}

