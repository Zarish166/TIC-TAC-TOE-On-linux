#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
 
#define SIZE 3
 
// Struct for player data
typedef struct {
    char player;         // 'X' or 'O'
    int move;            // Player's move input (1-9)
    char (*board)[SIZE]; // Pointer to the game board
    pthread_mutex_t *mutex; // Mutex to lock the board
} PlayerData;
 
// Function prototypes
void displayBoard(char board[SIZE][SIZE]);
bool checkWin(char board[SIZE][SIZE], char player);
bool checkDraw(char board[SIZE][SIZE]);
void *getPlayerMove(void *arg);
 
int main() {
    // Dynamically allocate the board
    char (*board)[SIZE] = malloc(sizeof(char[SIZE][SIZE]));
    if (board == NULL) {
        printf("Memory allocation failed!\n");
        return 1;
    }
 
    // Initialize the board with numbers
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            board[i][j] = '1' + i * SIZE + j;
        }
    }
 
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);
 
    char currentPlayer = 'X';
    bool gameWon = false, gameDraw = false;
 
    printf("Welcome to Tic-Tac-Toe!\n");
    displayBoard(board);
 
    // Create threads for two players
    pthread_t thread1, thread2;
    PlayerData player1 = { 'X', 0, board, &mutex };
    PlayerData player2 = { 'O', 0, board, &mutex };
 
    // Game loop
    while (!gameWon && !gameDraw) {
        // Create threads for both players
        if (currentPlayer == 'X') {
            pthread_create(&thread1, NULL, getPlayerMove, &player1);
            pthread_join(thread1, NULL);
            gameWon = checkWin(board, 'X');
        } else {
            pthread_create(&thread2, NULL, getPlayerMove, &player2);
            pthread_join(thread2, NULL);
            gameWon = checkWin(board, 'O');
        }
 
        displayBoard(board);
        if (gameWon) {
            printf("Player %c wins!\n", currentPlayer);
            break;
        }
 
        gameDraw = checkDraw(board);
        if (gameDraw) {
            printf("It's a draw!\n");
            break;
        }
 
        // Switch player
        currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
    }
 
    free(board); // Free dynamically allocated memory
    pthread_mutex_destroy(&mutex); // Destroy the mutex
    return 0;
}
 
// Function to display the board
void displayBoard(char board[SIZE][SIZE]) {
    printf("\n");
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            printf(" %c ", board[i][j]);
            if (j < SIZE - 1) printf("|");
        }
        printf("\n");
        if (i < SIZE - 1) printf("---|---|---\n");
    }
    printf("\n");
}
 
// Function to check if a player has won
bool checkWin(char board[SIZE][SIZE], char player) {
    // Check rows and columns
    for (int i = 0; i < SIZE; i++) {
        if ((board[i][0] == player && board[i][1] == player && board[i][2] == player) || // Row
            (board[0][i] == player && board[1][i] == player && board[2][i] == player)) { // Column
            return true;
        }
    }
    // Check diagonals
    if ((board[0][0] == player && board[1][1] == player && board[2][2] == player) || // Main diagonal
        (board[0][2] == player && board[1][1] == player && board[2][0] == player)) { // Anti-diagonal
        return true;
    }
    return false;
}
 
// Function to check if the game is a draw
bool checkDraw(char board[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] != 'X' && board[i][j] != 'O') {
                return false;
            }
        }
    }
    return true;
}
 
// Function to get the player's move
void *getPlayerMove(void *arg) {
    PlayerData *playerData = (PlayerData *)arg;
    int move;
    bool validMove = false;
    while (!validMove) {
        pthread_mutex_lock(playerData->mutex); // Lock the board before making a move
 
        printf("Player %c, enter your move (1-9): ", playerData->player);
        scanf("%d", &move);
 
        if (move < 1 || move > 9) {
            printf("Invalid move! Please choose a number between 1 and 9.\n");
        } else {
            int row = (move - 1) / SIZE;
            int col = (move - 1) % SIZE;
            if (playerData->board[row][col] != 'X' && playerData->board[row][col] != 'O') {
                playerData->board[row][col] = playerData->player;
                validMove = true;
            } else {
                printf("Spot already taken! Choose another.\n");
            }
        }
        pthread_mutex_unlock(playerData->mutex); // Unlock the board after making the move
    }
    return NULL;
}