#include "player.hpp"

#include <bits/stdc++.h>
#include <cstdlib>
#include <unordered_map>

#define Xfour 1000
#define Xthree 20
#define Xtwo 1
#define Ofour -1000
#define Othree -20
#define Otwo -1
#define draw 0

#define DEPTH 2

#define cSquares 64

namespace TICTACTOE3D
{
    std::unordered_map<std::string, int> cache;

    GameState Player::play(const GameState &pState,const Deadline &pDue)
    {

        std::vector<GameState> lNextStates;
        pState.findPossibleMoves(lNextStates);

        if (lNextStates.empty()) return GameState(pState, Move());

        Deadline startTime = TICTACTOE3D::Deadline::now();

        int bestPossible = INT_MIN;
        int bestIndex = -1;
        char board[64];

        int alpha = INT_MIN;
        int beta = INT_MAX;

        for(int i = 0; i < lNextStates.size(); ++i) {
            //Transform GameState to char
            std::string s = lNextStates[i].toMessage();
            for (int j = 0; j < 64; ++j) board[j] = s[j];

            int v = alphabeta(board, DEPTH, alpha, beta, CELL_O, i);
            if(v > bestPossible) {
                bestPossible = v;
                bestIndex = i;
            }
        }

        std::cerr << "algo: " << TICTACTOE3D::Deadline::now() - startTime << std::endl;

        return lNextStates[bestIndex];

        /*
         * Here you should write your clever algorithms to get the best next move, ie the best
         * next state. This skeleton returns a random move instead.
         */

        //return lNextStates[rand() % lNextStates.size()];
    }

    /**
     * Executes minimax algorithm with alpha-beta prunning until \depth.
     *
     * \param state Gamestate to start minimax
     * \param depth depth of the tree to investigate
     * \param alpha
     * \param beta
     * \param player represents the player to do the next move (turn's player)
     * \param pCell cell of the 3D board (as a 1D array) where the previous player has placed the last piece
     * \return value (calculated by a heuristic) of gamestate \state
     *
     * The heuristic is:
     * - If depth = 0:
     *      - If end of the game and X wins: 1000
     *      - If end of the game and O wins: -1000
     *      - If end of the game and draw: 0
     *      - Otherwise: evaluate state with evaluation(state)
     */
    int alphabeta(char *state, int depth, int alpha, int beta, Cell player, int pCell) {
        // First part of the heuristic: the max depth has been reached
        if(depth == 0 || isEOG(state)) {
            if (Special_Move(state, pCell, CELL_X) == 1) { return Xfour; }  // Xwins
            else if(Special_Move(state, pCell, CELL_O) == 1) { return Ofour; } // Owins
            else if(isEOG(state)) return draw;
            return evaluation(state);

            /**
             * Store states so they're not recomputed
             *
                if(cache.count(state) > 0) {return cache[state]; }
                else {
                    int ev = evaluation(state);
                    cache[state] = ev;
                    return ev;
                }
            **/
        }
        // Perform minimax with alpha-beta pruning
        else {
            if (player == CELL_X) {
                // Our turn -> maximize value

                int v = INT_MIN;
                // For each cell that is empty, we can do a movement
                for(int i = 0; i < 64; ++i) {
                    if(state[i] == '.') {
                        // Do movement by player X
                        state[i] = 'x';
                        v = std::max(v, alphabeta(state, depth - 1, alpha, beta, CELL_O, i));
                        state[i] = '.';
                        alpha = std::max(alpha, v);
                        if (beta <= alpha) {
                            break;
                        }
                    }
                }

                return v;
            }
            else {
                // Opponent's turn -> minimize value

                // Find possible moves from this state for player O
                int v = INT_MAX;
                // For each cell that is empty, we can do a movement
                for(int i = 0; i < 64; ++i) {
                    if(state[i] == '.') {
                        // Do movement by player O
                        state[i] = 'o';
                        v = std::min(v, alphabeta(state, depth - 1, alpha, beta, CELL_X, i));
                        state[i] = '.';
                        beta = std::min(beta, v);
                        if (beta <= alpha) {
                            break;
                        }
                    }
                }

                return v;
            }
        }
    }

    /**
     * Evaluation function of a state of the game.
     * It is calculated by counting the # 2 in a rows, # 3 in a rows and # 4 in a rows of player X and return a total value
     * weighting each of the rows by a constant.
     *
     * @param state
     * @return value of the state (#3XinRow*10+@2XinRow*1) - (#3OinRow*20)
     */
    int evaluation(const char *state) {
        //TODO: add to check if four in line and modify test in alpha-beta

        int value = 0;

        // Rows of board (they correspond to cells of Gamestate.hpp):
        //     0 1 2 3, 4 5 6 7, 8 9 10 11, ... , 56 57 58 59, 60 61 62 63
        int Xrows = 0, Orows = 0;

        for(int layer = 0; layer < 4; ++layer) {
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    if (state[layer * 16 + i * 4 + j] == 'x') { ++Xrows; }
                    else if (state[layer * 16+ i * 4 + j] == 'o') { ++Orows; }
                }

                if (Orows == 0) {
                    if (Xrows == 3) { value += Xthree; }
                    else if (Xrows == 2) { value += Xtwo; }
                } else if (Xrows == 0) {
                    if (Orows == 3) { value += Othree; }
                    else if (Orows == 2) { value += Otwo; }
                }
                Xrows = 0;
                Orows = 0;
            }
        }

        //Columns of board (they correspond to cells of Gamestate.hpp):
        //    0 1 2 3, 4 5 6 7, 8 9 10 11, ... , 56 57 58 59, 60 61 62 63
        int Xcols = 0, Ocols = 0;
        for(int layer = 0; layer < 4; ++layer) {
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    if (state[layer * 16 + i + j*4] == 'x') { ++Xcols; }
                    else if (state[layer * 16+ i + j*4] == 'o') { ++Ocols; }
                }

                if (Ocols == 0) {
                    if (Xcols == 3) { value += Xthree; }
                    else if (Xcols == 2) { value += Xtwo; }
                } else if (Xcols == 0) {
                    if (Ocols == 3) { value += Othree; }
                    else if (Ocols == 2) { value += Otwo; }
                }
                Xcols = 0;
                Ocols = 0;
            }
        }

        //Verticals of board (they correspond to cells of Gamestate.hpp):
        //      0 16 32 48, 1 17 33 49, ... , 14 30 46 62, 15 31 47 63
        int Xvert = 0, Overt = 0;
        for(int vertical = 0; vertical < 16; ++vertical) {
            for(int height = 0; height < 64; height += 16) {
                if(state[vertical+height] == 'x') { ++Xvert; }
                else if(state[vertical+height] == 'o') { ++Overt; }
            }

            if (Overt == 0) {
                if (Xvert == 3) { value += Xthree; }
                else if (Xvert == 2) { value += Xtwo; }
            }
            else if (Xvert == 0) {
                if (Overt == 3) { value += Othree; }
                else if (Overt == 2) { value += Otwo; }
            }
            Xvert = 0;
            Overt = 0;
        }

        // Layer diagonals (they correspond to cells of Gamestate.hpp):
        //       0 5 10 15, 3 6 9 12, 16 21 26 31, 19 22 25 28, ...
        int Xdiag = 0, Odiag = 0;
        for(int layer = 0; layer < 4; ++layer) {
            //First diagonal: top-left to bottom-right
            if(state[layer*16+0] == 'x') { ++Xdiag; }
            else if(state[layer*16+0] == 'o') { ++Odiag; }

            if(state[layer*16+5] == 'x') { ++Xdiag; }
            else if(state[layer*16+5] == 'o') { ++Odiag; }

            if(state[layer*16+10] == 'x') { ++Xdiag; }
            else if(state[layer*16+10] == 'o') { ++Odiag; }

            if(state[layer*16+15] == 'x') { ++Xdiag; }
            else if(state[layer*16+15] == 'o') { ++Odiag; }

            if (Odiag == 0) {
                if (Xdiag == 3) { value += Xthree; }
                else if (Xdiag == 2) { value += Xtwo; }
            }
            else if (Xdiag == 0) {
                if (Odiag == 3) { value += Othree; }
                else if (Odiag == 2) { value += Otwo; }
            }
            Xdiag = 0;
            Odiag = 0;

            // Second diagonal: top-right to bottom-left
            if(state[layer*16+3] == 'x') { ++Xdiag; }
            else if(state[layer*16+3] == 'o') { ++Odiag; }

            if(state[layer*16+6] == 'x') { ++Xdiag; }
            else if(state[layer*16+6] == 'o') { ++Odiag; }

            if(state[layer*16+9] == 'x') { ++Xdiag; }
            else if(state[layer*16+9] == 'o') { ++Odiag; }

            if(state[layer*16+12] == 'x') { ++Xdiag; }
            else if(state[layer*16+12] == 'o') { ++Odiag; }

            if (Odiag == 0) {
                if (Xdiag == 3) { value += Xthree; }
                else if (Xdiag == 2) { value += Xtwo; }
            }
            else if (Xdiag == 0) {
                if (Odiag == 3) { value += Othree; }
                else if (Odiag == 2) { value += Otwo; }
            }
            Xdiag = 0;
            Odiag = 0;
        }

        //Diagonals (rows) first: 0 17 34 51, 4 21 38 55, ...
        //Diagonals (rows) second: 3 18 33 48, 7 22 37 52
        for(int i = 0; i < 16; i += 4) {
            if(state[i+0] == 'x') { ++Xdiag; }
            else if(state[i+0] == 'o') { ++Odiag; }

            if(state[i+17] == 'x') { ++Xdiag; }
            else if(state[i+17] == 'o') { ++Odiag; }

            if(state[i+34] == 'x') { ++Xdiag; }
            else if(state[i+34] == 'o') { ++Odiag; }

            if(state[i+51] == 'x') { ++Xdiag; }
            else if(state[i+51] == 'o') { ++Odiag; }

            if (Odiag == 0) {
                if (Xdiag == 3) { value += Xthree; }
                else if (Xdiag == 2) { value += Xtwo; }
            }
            else if (Xdiag == 0) {
                if (Odiag == 3) { value += Othree; }
                else if (Odiag == 2) { value += Otwo; }
            }
            Xdiag = 0;
            Odiag = 0;

            if(state[i+3] == 'x') { ++Xdiag; }
            else if(state[i+3] == 'o') { ++Odiag; }

            if(state[i+18] == 'x') { ++Xdiag; }
            else if(state[i+18] == 'o') { ++Odiag; }

            if(state[i+33] == 'x') { ++Xdiag; }
            else if(state[i+33] == 'o') { ++Odiag; }

            if(state[i+48] == 'x') { ++Xdiag; }
            else if(state[i+48] == 'o') { ++Odiag; }

            if (Odiag == 0) {
                if (Xdiag == 3) { value += Xthree; }
                else if (Xdiag == 2) { value += Xtwo; }
            }
            else if (Xdiag == 0) {
                if (Odiag == 3) { value += Othree; }
                else if (Odiag == 2) { value += Otwo; }
            }
            Xdiag = 0;
            Odiag = 0;
        }

        //Diagonals (columns) first: 0 20 40 60, 1 21 41 61, ...
        //Diagonals (columns) second: 12 24 36 48, 13 25 37 49, ...
        for(int i = 0; i < 4; ++i) {
            if(state[i+0] == 'x') { ++Xdiag; }
            else if(state[i+0] == 'o') { ++Odiag; }

            if(state[i+20] == 'x') { ++Xdiag; }
            else if(state[i+20] == 'o') { ++Odiag; }

            if(state[i+40] == 'x') { ++Xdiag; }
            else if(state[i+40] == 'o') { ++Odiag; }

            if(state[i+60] == 'x') { ++Xdiag; }
            else if(state[i+60] == 'o') { ++Odiag; }

            if (Odiag == 0) {
                if (Xdiag == 3) { value += Xthree; }
                else if (Xdiag == 2) { value += Xtwo; }
            }
            else if (Xdiag == 0) {
                if (Odiag == 3) { value += Othree; }
                else if (Odiag == 2) { value += Otwo; }
            }
            Xdiag = 0;
            Odiag = 0;

            if(state[i+12] == 'x') { ++Xdiag; }
            else if(state[i+12] == 'o') { ++Odiag; }

            if(state[i+24] == 'x') { ++Xdiag; }
            else if(state[i+24] == 'o') { ++Odiag; }

            if(state[i+36] == 'x') { ++Xdiag; }
            else if(state[i+36] == 'o') { ++Odiag; }

            if(state[i+48] == 'x') { ++Xdiag; }
            else if(state[i+48] == 'o') { ++Odiag; }

            if (Odiag == 0) {
                if (Xdiag == 3) { value += Xthree; }
                else if (Xdiag == 2) { value += Xtwo; }
            }
            else if (Xdiag == 0) {
                if (Odiag == 3) { value += Othree; }
                else if (Odiag == 2) { value += Otwo; }
            }
            Xdiag = 0;
            Odiag = 0;
        }

        // Main Cube Diagonals (four diagonals that span accross three dimensions)
        // First diagonal
        Xdiag = 0;
        Odiag = 0;
        // We count the pieces of X and O
        if (state[0] == 'x') { ++Xdiag; }
        else if (state[0] == 'o') { ++Odiag; }

        if (state[21] == 'x') { ++Xdiag; }
        else if (state[21] == 'o') { ++Odiag; }

        if (state[42] == 'x') { ++Xdiag; }
        else if (state[42] == 'o') { ++Odiag; }

        if (state[63] == 'x') { ++Xdiag; }
        else if (state[63] == 'o') { ++Odiag; }

        // Because the diagonal is of size 4, we only count 2, 3 or 4 in a diagonal if in that diagonal there're only
        // pieces of one player. If there's a piece of the other player, a winner move won't appear in that diagonal.
        if (Odiag == 0) {
            if (Xdiag == 2) { value += Xtwo; }
            else if (Xdiag == 3) { value += Xthree; }
            else if (Xdiag == 4) { value += Xfour; }
        }
        else if (Xdiag == 0) {
            if (Odiag == 2) { value += Otwo; }
            else if (Odiag == 3) { value += Othree; }
            else if (Odiag == 4) { value += Ofour; }
        }

        // Second diagonal
        Xdiag = 0;
        Odiag = 0;
        // We count the pieces of X and O
        if (state[3] == 'x') { ++Xdiag; }
        else if (state[3] == 'o') { ++Odiag; }

        if (state[22] == 'x') { ++Xdiag; }
        else if (state[22] == 'o') { ++Odiag; }

        if (state[41] == 'x') { ++Xdiag; }
        else if (state[41] == 'o') { ++Odiag; }

        if (state[60] == 'x') { ++Xdiag; }
        else if (state[60] == 'o') { ++Odiag; }

        // Because the diagonal is of size 4, we only count 2, 3 or 4 in a diagonal if in that diagonal there're only
        // pieces of one player. If there's a piece of the other player, a winner move won't appear in that diagonal.
        if (Odiag == 0) {
            if (Xdiag == 2) { value += Xtwo; }
            else if (Xdiag == 3) { value += Xthree; }
            else if (Xdiag == 4) { value += Xfour; }
        }
        else if (Xdiag == 0) {
            if (Odiag == 2) { value += Otwo; }
            else if (Odiag == 3) { value += Othree; }
            else if (Odiag == 4) { value += Ofour; }
        }

        // Third diagonal
        Xdiag = 0;
        Odiag = 0;
        // We count the pieces of X and O
        if (state[12] == 'x') { ++Xdiag; }
        else if (state[12] == 'o') { ++Odiag; }

        if (state[25] == 'x') { ++Xdiag; }
        else if (state[25] == 'o') { ++Odiag; }

        if (state[38] == 'x') { ++Xdiag; }
        else if (state[38] == 'o') { ++Odiag; }

        if (state[51] == 'x') { ++Xdiag; }
        else if (state[51] == 'o') { ++Odiag; }

        // Because the diagonal is of size 4, we only count 2, 3 or 4 in a diagonal if in that diagonal there're only
        // pieces of one player. If there's a piece of the other player, a winner move won't appear in that diagonal.
        if (Odiag == 0) {
            if (Xdiag == 2) { value += Xtwo; }
            else if (Xdiag == 3) { value += Xthree; }
            else if (Xdiag == 4) { value += Xfour; }
        }
        else if (Xdiag == 0) {
            if (Odiag == 2) { value += Otwo; }
            else if (Odiag == 3) { value += Othree; }
            else if (Odiag == 4) { value += Ofour; }
        }

        // Fourth diagonal
        Xdiag = 0;
        Odiag = 0;
        // We count the pieces of X and O
        if (state[15] == 'x') { ++Xdiag; }
        else if (state[15] == 'o') { ++Odiag; }

        if (state[26] == 'x') { ++Xdiag; }
        else if (state[26] == 'o') { ++Odiag; }

        if (state[37] == 'x') { ++Xdiag; }
        else if (state[37] == 'o') { ++Odiag; }

        if (state[48] == 'x') { ++Xdiag; }
        else if (state[48] == 'o') { ++Odiag; }

        // Because the diagonal is of size 4, we only count 2, 3 or 4 in a diagonal if in that diagonal there're only
        // pieces of one player. If there's a piece of the other player, a winner move won't appear in that diagonal.
        if (Odiag == 0) {
            if (Xdiag == 2) { value += Xtwo; }
            else if (Xdiag == 3) { value += Xthree; }
            else if (Xdiag == 4) { value += Xfour; }
        }
        else if (Xdiag == 0) {
            if (Odiag == 2) { value += Otwo; }
            else if (Odiag == 3) { value += Othree; }
            else if (Odiag == 4) { value += Ofour; }
        }

        // We return the value of the state as a weighted sum of the # of 2, 3 or 4 in a rows of player X
        return value;
    };

    ///returns the row corresponding to a cell index
    int cellToRow(int pCell)
    {
        return (((pCell) >> 2)-(4*cellToLay(pCell)));
    }

    ///returns the col corresponding to a cell index
    int cellToCol(int pCell)
    {
        int lC = ((pCell) & 3);
        return lC;
    }

    ///returns the lay corresponding to a cell index
    int cellToLay(int pCell)
    {
        return pCell/16;
    }

    uint8_t at(const char *mCell, int pR, int pC, int pL)
    {
        if (pR < 0 || pR > 3 || pC < 0 || pC > 3|| pL < 0 || pL > 3)
            return CELL_INVALID;
        char cell = mCell[(pR * 4 + pC)+(16*pL) ];
        if(cell == '.') return 0;
        else if(cell == 'x') return 1;
        else return 2;
    }

    uint8_t at( const char *mCell, int pPos)
    {
        assert(pPos >= 0);
        assert(pPos < cSquares);
        if(mCell[pPos] == '.') return 0;
        else if(mCell[pPos] == 'x') return 1;
        else return 2;
    }

    /**
     * @param state the board
     * @return true if board is full of pieces (game has ended), false otherwise
     */
    bool isEOG(const char* state) {
        for(int i = 0; i < 64; ++i) {
            if(state[i] == '.') return false;
        }

        return true;
    }

    /**
     * Analyse movement of \pPlayer on cell \pCell to check if it's a special movement (X winner, O winner or draw)
     * \param mCell the board
     * \param pCell the cell where the movement has been made (the piece has been placed)
     * \param pPlayer player who has done the movement
     * \return 1 if \pPlayer has won with this movement, 2 if the movement has been a draw or 0 if it's not a special movement
     */
    int Special_Move(const char* board, int pCell, Cell pPlayer)
    {
        int lR=cellToRow(pCell)+1;
        int lC=cellToCol(pCell)+1;
        int lL=cellToLay(pCell)+1;
        //check if winning move:
        int checkWin3D[6][6][6];
        //fill check 3D array
        for (int x=0;x<4;x++) {
            for (int y=0;y<4;y++) {
                for (int z=0;z<4;z++) {
                    checkWin3D[x+1][y+1][z+1]= at(board, x, y, z);
                }
            }
        }

        for (int i=0;i<6;i++) {
            for (int j=0;j<6;j++) {
                checkWin3D[0][i][j]=7; checkWin3D[5][i][j]=7;
                checkWin3D[i][j][0]=7; checkWin3D[i][j][5]=7;
                checkWin3D[i][0][j]=7; checkWin3D[i][5][j]=7;}
        }

        //make the move temporaly:
        checkWin3D[lR][lC][lL]=pPlayer;

        int x[4],y[4],z[4];        //variables to store winning coordinates
        x[0]=lR;y[0]=lC;z[0]=lL;   //sets 0 of each to last move

        int Pcount;

        for (int a=1;a<14;a++){    //iterates through all possible directions in 3D space
            Pcount=1;                      //reset counter
            int lR2=lR; int lC2=lC; int lL2=lL;

            for (;checkWin3D[lR2][lC2][lL2]!=7;){

                //look in every derection until out of field ('7')
                switch (a) {
                    case 1:lR2--;break;
                    case 2:lR2--;lC2--;lL2--;break;
                    case 3:lR2--;lC2--;break;
                    case 4:lR2--;lC2--;lL2++;break;
                    case 5:lR2--;lL2--;break;
                    case 6:lR2--;lL2++;break;
                    case 7:lR2--;lC2++;lL2--;break;
                    case 8:lR2--;lC2++;break;
                    case 9:lR2--;lC2++;lL2++;break;
                    case 10:lL2--;break;
                    case 11:lC2--;lL2--;break;
                    case 12:lC2--;break;
                    case 13:lC2--;lL2++;break;
                }
                if (checkWin3D[lR2][lC2][lL2]==pPlayer) {
                    x[Pcount]=lR2;y[Pcount]=lC2;z[Pcount]=lL2;
                    Pcount++;}

            }

            lR2=lR;lC2=lC;lL2=lL;   //reset placeholder variables


            for (;checkWin3D[lR2][lC2][lL2]!=7;) {


                switch (a) {
                    case 1:lR2++;break;                ///////////////////////
                    case 2:lR2++;lC2++;lL2++;break;      //
                    case 3:lR2++;lC2++;break;           // and
                    case 4:lR2++;lC2++;lL2--;break;      // then back
                    case 5:lR2++;lL2++;break;           //  in the
                    case 6:lR2++;lL2--;break;           //  opposite
                    case 7:lR2++;lC2--;lL2++;break;      //  direction
                    case 8:lR2++;lC2--;break;           //
                    case 9:lR2++;lC2--;lL2--;break;      //
                    case 10:lL2++;break;               //
                    case 11:lC2++;lL2++;break;          //
                    case 12:lC2++;break;               //
                    case 13:lC2++;lL2--;break;          ///////////////////////
                }

                if (checkWin3D[lR2][lC2][lL2]==pPlayer) {
                    x[Pcount]=lR2;y[Pcount]=lC2;z[Pcount]=lL2;
                    Pcount++;
                }
            }

            if (Pcount>3) {for (int b=0;b<4;b++) {checkWin3D[x[b]][y[b]][z[b]]=4;}
                return 1;}
        }


        //Check Draw
        int cpieces=0;
        for (int k = 0; k < cSquares; ++k)
        {
            // Try move
            if(at(board, k)!='.')
            {
                cpieces+=1;
            }
        }

        if(cpieces==cSquares-1 )
            return 2;


        return 0;
    }

/*namespace TICTACTOE3D*/ }
