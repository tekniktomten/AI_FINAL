#include "player.hpp"
#include <cstdlib>

namespace TICTACTOE
{

int whoWins(char* board) { // returns 1 if x wins, -1 if o wins and 0 otherwise
    //Check horizontal
		int xCH = 0;
        int xCV = 0;
		int oCH = 0;
        int oCV = 0;
		for(int i = 0;i < 4; i++) {
			for(int j = 0; j < 4;j++) {
				if(board[i*4+j]=='x') {
					++xCH;
				}
                else if(board[i*4+j]=='o') {
					++oCH;
				}
                if(board[j*4+i]=='x') {
					++xCV;
				}
                else if(board[j*4+i]=='o') {
					++oCV;
				}
			}
			if (xCH == 4) return 100;
            if (xCV == 4) return 100;
            if (oCH == 4) return -100;
            if (oCV == 4) return -100;
			xCH = 0;
            xCV = 0;
            oCH = 0;
            oCV = 0;
		}
		//Check diagonals
		int xCD0 = 0;
        int oCD0 = 0;
		int j = 0;
        int xCD1 = 0;
        int oCD1 = 0;
		for(int i = 0;i < 4; i++) {
				if(board[j*4+i]=='x') {
					++xCD0;
				}
                else if(board[j*4+i]=='o') {
					++oCD0;
				}
                if(board[(3-j)*4+i]=='x') {
					++xCD1;
				}
                else if(board[(3-j)*4+i]=='o') {
					++oCD1;
				}
			if(xCD0 == 4) return 100;
            if(xCD1 == 4) return 100;
            if(oCD0 == 4) return -100;
            if(oCD1 == 4) return -100;
			++j;
		}

    bool draw = true;
    for (int i = 0; i < 16; ++i) if (board[i] == '.') draw = false;
    if (draw) return -1; // maybe good?

    return 0;
}

int heuristic(char* board) {
    return 0;
}

int minimax(char* board, bool max, int alpha, int beta, int depth, int maxDepth) {
    int value = 0;
    int best = 0;
    int win = whoWins(board);
    if (win != 0) return win;
    if (depth > maxDepth) return heuristic(board);
    if (max) { // maximizing player
        best = -999;
        for(int i = 0; i < 16; ++i) {
            if (board[i] == '.') {
                board[i] = 'x';
                value = minimax(board, false, alpha, beta, depth + 1, maxDepth);
                //std::cerr << "returned" << value << std::endl;
                board[i] = '.';
                if (value > best) {
                    best = value;
                    if (value > alpha) alpha = value;
                }
            }
            if (beta <= alpha) break;
        }
    }
    else { // minimizing player
        best = 999;
        for(int i = 0; i < 16; ++i) {
            if (board[i] == '.') {
                board[i] = 'o';
                value = minimax(board, true, alpha, beta, depth + 1, maxDepth);
                //std::cerr << "returned" << value << std::endl;
                board[i] = '.';
                if (value < best) {
                    best = value;
                    if (value < beta) beta = value;
                }
            }
            if (beta <= alpha) break;
        }
    }
    return best;
}

GameState Player::play(const GameState &pState,const Deadline &pDue)
{
    //std::cerr << "Processing " << pState.toMessage() << std::endl;

    std::vector<GameState> lNextStates;
    pState.findPossibleMoves(lNextStates);


    if (lNextStates.size() == 0) return GameState(pState, Move());

    /*
     * Here you should write your clever algorithms to get the best next move, ie the best
     * next state. This skeleton returns a random move instead.
     */

    std::string s;
    char board_array[16];
    char* board = board_array;
    GameState* bestState;
    int value;
    int bestValue = -999;
    for (unsigned int i = 0; i < lNextStates.size(); ++i) {
        s = lNextStates[i].toMessage();
        for (int j = 0; j < 16; ++j) board[j] = s[j];
        value = minimax(board, false, -999, 999, 0, 3);
        std::cerr << value << std::endl;
        if (value > bestValue) {
            bestValue = value;
            bestState = &lNextStates[i];
        }
    }

    return *bestState;
}


/*namespace TICTACTOE*/ }
