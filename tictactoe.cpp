#include <iostream>
#include <string>
#include <limits>     // for numeric_limits
#include <algorithm>  // for std::max / std::min
using namespace std;

/* ===================== State ===================== */
char board[3][3];
string player1, player2;          // player2 == "Computer" in PvC
char playerSymbol = 'X', computerSymbol = 'O';

/* ===================== Helpers ===================== */
void resetBoard() {
    char n = '1';
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            board[i][j] = n++;
}

void displayBoard() {
    cout << "\n";
    cout << "    |     |    \n";
    cout << "  " << board[0][0] << " |  " << board[0][1] << "  | " << board[0][2] << "\n";
    cout << "----|-----|----\n";
    cout << "  " << board[1][0] << " |  " << board[1][1] << "  | " << board[1][2] << "\n";
    cout << "----|-----|----\n";
    cout << "  " << board[2][0] << " |  " << board[2][1] << "  | " << board[2][2] << "\n";
    cout << "    |     |    \n\n";
}

bool isWinner(char s) {
    for (int i = 0; i < 3; ++i)
        if ((board[i][0] == s && board[i][1] == s && board[i][2] == s) ||
            (board[0][i] == s && board[1][i] == s && board[2][i] == s))
            return true;
    if ((board[0][0] == s && board[1][1] == s && board[2][2] == s) ||
        (board[0][2] == s && board[1][1] == s && board[2][0] == s))
        return true;
    return false;
}

bool isBoardFull() {
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            if (board[i][j] != 'X' && board[i][j] != 'O') return false;
    return true;
}

/* Safe numeric input */
int askIntInRange(const string& prompt, int lo, int hi) {
    int x;
    while (true) {
        cout << prompt;
        if (cin >> x && x >= lo && x <= hi) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return x;
        }
        cout << "Invalid input. Try again.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

/* ===================== Human Moves ===================== */
bool placeAtCell(int cell, char symbol) {
    int r = (cell - 1) / 3, c = (cell - 1) % 3;
    if (board[r][c] == 'X' || board[r][c] == 'O') return false;
    board[r][c] = symbol;
    return true;
}

void playerMove(const string& name, char symbol) {
    while (true) {
        int choice = askIntInRange(name + " (" + symbol + ") - choose position (1-9): ", 1, 9);
        if (placeAtCell(choice, symbol)) break;
        cout << "Cell already taken. Choose a different position.\n";
    }
}

/* ===================== Minimax AI ===================== */
int evaluate() {
    if (isWinner(computerSymbol)) return 10;
    if (isWinner(playerSymbol))   return -10;
    return 0;
}

int minimax(bool isMax, int depth) {
    int val = evaluate();
    if (val == 10)  return val - depth; // faster win better
    if (val == -10) return val + depth; // slower loss better
    if (isBoardFull()) return 0;

    if (isMax) {
        int best = -1000;
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                if (board[i][j] != 'X' && board[i][j] != 'O') {
                    char keep = board[i][j];
                    board[i][j] = computerSymbol;
                    best = max(best, minimax(false, depth + 1));
                    board[i][j] = keep;
                }
        return best;
    } else {
        int best = 1000;
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                if (board[i][j] != 'X' && board[i][j] != 'O') {
                    char keep = board[i][j];
                    board[i][j] = playerSymbol;
                    best = min(best, minimax(true, depth + 1));
                    board[i][j] = keep;
                }
        return best;
    }
}

pair<int,int> bestMove() {
    int bestVal = -1000;
    pair<int,int> best = {-1,-1};
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            if (board[i][j] != 'X' && board[i][j] != 'O') {
                char keep = board[i][j];
                board[i][j] = computerSymbol;
                int moveVal = minimax(false, 0);
                board[i][j] = keep;
                if (moveVal > bestVal) {
                    bestVal = moveVal;
                    best = {i, j};
                }
            }
    return best;
}

void computerMove() {
    auto mv = bestMove();
    int r = mv.first, c = mv.second;
    int pos = r * 3 + c + 1;
    board[r][c] = computerSymbol;
    cout << "Computer (" << computerSymbol << ") chose: " << pos << "\n";
}

/* ===================== Main ===================== */
int main() {
    cout << "Choose Mode:\n  1) Player vs Player\n  2) Player vs Computer (Minimax AI)\n";
    int mode = askIntInRange("Enter 1 or 2: ", 1, 2);

    if (mode == 1) {
        cout << "Enter Player 1 name: ";
        getline(cin, player1);
        cout << "Enter Player 2 name: ";
        getline(cin, player2);
    } else {
        cout << "Enter your name: ";
        getline(cin, player1);
        player2 = "Computer";
        cout << "Choose your symbol (X/O): ";
        char s;
        while (true) {
            if (cin >> s) {
                s = toupper(s);
                if (s == 'X' || s == 'O') break;
            }
            cout << "Please enter X or O: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        playerSymbol   = s;
        computerSymbol = (s == 'X') ? 'O' : 'X';
    }

    char playAgain = 'Y';
    while (playAgain == 'Y' || playAgain == 'y') {
        resetBoard();

        // X always starts
        char currentSymbol = 'X';
        string currentPlayerName = (mode == 1)
            ? player1
            : ((currentSymbol == playerSymbol) ? player1 : player2);

        while (true) {
            displayBoard();

            if (mode == 1) {
                // PvP
                playerMove(currentPlayerName, currentSymbol);
            } else {
                // PvC
                if (currentPlayerName == player1 && currentSymbol == playerSymbol) {
                    playerMove(player1, playerSymbol);
                } else {
                    computerMove();
                }
            }

            // Check end state for the symbol that just played
            if (isWinner(currentSymbol)) {
                displayBoard();
                cout << currentPlayerName << " wins!\n";
                break;
            }
            if (isBoardFull()) {
                displayBoard();
                cout << "It's a draw!\n";
                break;
            }

            // Switch turn
            currentSymbol = (currentSymbol == 'X') ? 'O' : 'X';
            if (mode == 1) {
                currentPlayerName = (currentPlayerName == player1) ? player2 : player1;
            } else {
                currentPlayerName = (currentPlayerName == player1) ? player2 : player1;
            }
        }

        cout << "Play again? (y/n): ";
        cin >> playAgain;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    cout << "Thanks for playing!\n";
    return 0;
}
