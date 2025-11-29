#include <stdio.h>

#ifndef ROWS
#define ROWS 6
#endif

#ifndef COLS
#define COLS 7
#endif

#define CONNECT_N 4

/* Tokens */
#define EMPTY '.'
#define TOKEN_P1 'X'
#define TOKEN_P2 'O'

#define HUMAN 1
#define COMPUTER 2

int isColumnFull(char[][COLS], int, int, int);

int isBoardFull(char[][COLS], int, int);

int isInBounds(int, int, int, int);

/* Return index of row where token will land, or -1 if column full */
int getFreeRow(char[][COLS], int, int, int);

/* Place token in column (0-based). Return row index or -1 if illegal */
int makeMove(char[][COLS], int, int, int, char);

int checkVictory(char[][COLS], int, int, int, int, char);

/* Human player: asks repeatedly until a valid non-full column is chosen (0-based) */
int humanChoose(char[][COLS], int, int);

/* Computer*/
int computerChoose(char[][COLS], int, int, char, char);

void runConnectFour(char[][COLS], int, int, int, int);

void initBoard(char[][COLS], int, int);

void printBoard(char[][COLS], int, int);

int getPlayerType(int);

int CheckSequenceOfThree(char[][COLS], char[COLS], int, int, int, int, char);


int main() {
    char board[ROWS][COLS];
    printf("Connect Four (%d rows x %d cols)\n\n", ROWS, COLS);
    int p1Type = getPlayerType(1);
    int p2Type = getPlayerType(2);
    initBoard(board, ROWS, COLS);
    printBoard(board, ROWS, COLS);
    runConnectFour(board, ROWS, COLS, p1Type, p2Type);
    return 0;
}
void initBoard(char board[][COLS], int rows, int cols) {
    for (int r = 0; r < rows; r++) 
        for (int c = 0; c < cols; c++) {
            board[r][c] = EMPTY;
        }
}

void printBoard(char board[][COLS], int rows, int cols) {
    printf("\n");
    for (int r = 0; r < rows; r++) {
        printf("|");
        for (int c = 0; c < cols; c++) {
            putchar(board[r][c]);
            printf("|");
        }
        printf("\n");
    }
    for (int c = 1; c <= cols; c++) {
        printf(" %d", c % 10);
    }
    printf("\n\n");
}

int getPlayerType(int playerNumber) {
    char ch;
    while (1) {
        printf("Choose type for player %d: h - human, c - computer: ", playerNumber);
        int n = scanf(" %c", &ch);
        if (n != 1) {
            printf("Input error. Try again.\n");
            while (getchar() != '\n'); // clear input buffer
            continue;
        }
        if (ch == 'h' || ch == 'H') return HUMAN;
        if (ch == 'c' || ch == 'C') return COMPUTER;

        printf("Invalid selection. Enter h or c.\n");
        while (getchar() != '\n'); // clear rest of input
    }
}
int makeMove(char board[][COLS], int ROWS, int COLS, int col, char token){
    if (col < 0 || col >= COLS) 
        return -1;
    int row = getFreeRow(board, ROWS, COLS, col);
    if (row == -1) 
        return -1;
    board[row][col] = token;
    return row;
}
void runConnectFour(char board[][COLS], int ROWS, int COLS, int p1Type, int p2Type){
    int PlayerNumber = 1;
    int ptype;
    char token;
    while(1){
        token = (PlayerNumber == 1) ? TOKEN_P1 : TOKEN_P2;
        ptype = (PlayerNumber == 1) ? p1Type : p2Type;
        printf("Player %d (%c) turn.\n", PlayerNumber, token);

        int ChosenColumn = -1;
        if(ptype == HUMAN){
            ChosenColumn = humanChoose(board,ROWS,COLS);
        }
        else{
            ChosenColumn = computerChoose(board,ROWS,COLS,token,(token == TOKEN_P1) ? TOKEN_P2 : TOKEN_P1);
            printf("Computer chose column %d\n", ChosenColumn);
        }

        int RowChanged = makeMove(board,ROWS,COLS,ChosenColumn,token);
        printBoard(board,ROWS,COLS);

        if(checkVictory(board,ROWS,COLS,RowChanged,ChosenColumn,token)){
            printf("Player %d (%c) wins!",PlayerNumber, token);
            break;
        }
        if(isBoardFull(board,ROWS,COLS)){
            printf("Board full and no winner. It's a tie!");
            break;
        }
        PlayerNumber = (PlayerNumber == 1) ? 2 : 1;
    }
}
int humanChoose(char board[][COLS], int ROWS, int COLS) {
     int col = 0;
    char c;

    while(1) {
        printf("Enter column (1-%d): ", COLS);
        col = 0;

        do {
            scanf("%c", &c);
        } while(c == ' ' || c == '\t' || c == '\n');

        while(c >= '0' && c <= '9') {
            col = col * 10 + (c - '0');
            scanf("%c", &c);
        }

        if(col < 1 || col > COLS) {
            printf("Invalid column. Choose between 1 and %d.\n", COLS);
            continue;
        }

        if(isColumnFull(board, ROWS, COLS, col - 1)) {
            printf("Column %d is full. Choose another column.\n", col);
            continue;
        }

        break;
    }

    return col - 1; 
}
int computerChoose(char board[][COLS], int ROWS, int COLS, char mytoken, char othertoken){
    int OrderRule[COLS];
    int centerLeft,centerRight;
    if(COLS%2){
        int center = COLS/2;
        int i = 0;
        OrderRule[i++] = center;
        for(int d = 1; i < COLS; d++){
            int left = center - d;
            int right = center + d;
            if(left >= 0)
                OrderRule[i++] = left;
            if(right < COLS)
                OrderRule[i++] = right;
        }
    }
    else{
        int cl = COLS/2 -1;
        int cr = COLS/2;
        int i = 0;
        OrderRule[i++] = cl;
        OrderRule[i++] = cr;
        for(int d = 1; i < COLS; d++){
            int left = cl - d;
            int right = cr + d;
            if(left >= 0)
                OrderRule[i++] = left;
            if(right < COLS)
                OrderRule[i++] = right;
        }  
    }
    //check winning move
    for(int i = 0; i < COLS; i++){
        int c = OrderRule[i];
        if(isColumnFull(board,ROWS,COLS,c))
            continue;
        int r = getFreeRow(board,ROWS,COLS,c);
        board[r][c] = mytoken;
        int win = checkVictory(board,ROWS,COLS,r,c,mytoken);
        board[r][c] = EMPTY;
        if(win)
            return c;
    }
    //block op win
    for(int i = 0; i < COLS; i++){
        int c = OrderRule[i];
        if(isColumnFull(board,ROWS,COLS,c))
            continue;
        int r = getFreeRow(board,ROWS,COLS,c);
        board[r][c] = othertoken;
        int win = checkVictory(board,ROWS,COLS,r,c,othertoken);
        board[r][c] = EMPTY;
        if(win)
            return c;
    }
    //create 3
    for(int i = 0; i < COLS; i++){
        int c = OrderRule[i];
        if(isColumnFull(board,ROWS,COLS,c))
            continue;
        int r = getFreeRow(board,ROWS,COLS,c);
        board[r][c] = mytoken;
        int maxseq = CheckSequenceOfThree(board, OrderRule, ROWS, COLS, r, c, mytoken);
        board[r][c] = EMPTY;
        if(maxseq)
            return c;
    }
    //block 3
     for(int i = 0; i < COLS; i++){
        int c = OrderRule[i];
        if(isColumnFull(board,ROWS,COLS,c))
            continue;
        int r = getFreeRow(board,ROWS,COLS,c);
        board[r][c] = othertoken;
        int maxseq = CheckSequenceOfThree(board, OrderRule, ROWS, COLS, r, c, othertoken);
        board[r][c] = EMPTY;
        if(maxseq)
            return c;
    }

    for(int i = 0; i<COLS; i++){
        int c = OrderRule[i];
        if(!isColumnFull(board,ROWS,COLS,c))
            return c;
    }
    return 0;
}
    int checkVictory(char board[][COLS], int ROWS, int COLS, int lastRow, int lastCol, char token) {
     if (lastRow < 0 || lastCol < 0) return 0;
    const int moveoptions[4][2] = {{0, 1},{1, 0}, {1, 1},{1, -1}};

    for (int d = 0; d < 4; d++) {
        int dr = moveoptions[d][0], dc = moveoptions[d][1];
        int count = 1; 

        int r = lastRow + dr, c = lastCol + dc;
        while (r >= 0 && r < ROWS && c >= 0 && c < COLS && board[r][c] == token) {
            count++; r += dr; c += dc;
        }
        
        r = lastRow - dr; c = lastCol - dc;
        while (r >= 0 && r < ROWS && c >= 0 && c < COLS && board[r][c] == token) {
            count++; r -= dr; c -= dc;
        }

        if (count >= CONNECT_N) return 1;
    }
    return 0;
}
int CheckSequenceOfThree(char board[][COLS], char OrderRule[COLS], int ROWS, int COLS, int lastRow, int lastCol, char token){
    if (lastRow < 0 || lastCol < 0) return 0;
    const int moveoptions[4][2] = {{0, 1},{1, 0}, {1, 1},{1, -1}};

    for (int d = 0; d < 4; d++) {
        int dr = moveoptions[d][0], dc = moveoptions[d][1];
        int count = 1; 

        int r = lastRow + dr, c = lastCol + dc;
        while (r >= 0 && r < ROWS && c >= 0 && c < COLS && board[r][c] == token) {
            count++; r += dr; c += dc;
        }
        
        r = lastRow - dr; c = lastCol - dc;
        while (r >= 0 && r < ROWS && c >= 0 && c < COLS && board[r][c] == token) {
            count++; r -= dr; c -= dc;
        }

        if (count == 3) return 1;
    }
    return 0;
} 

