#ifndef _TICTACTOE3D_PLAYER_HPP_
#define _TICTACTOE3D_PLAYER_HPP_

#include "constants.hpp"
#include "deadline.hpp"
#include "move.hpp"
#include "gamestate.hpp"
#include <vector>

namespace TICTACTOE3D
{

class Player
{
public:
    ///perform a move
    ///\param pState the current state of the board
    ///\param pDue time before which we must have returned
    ///\return the next state the board is in after our move
    GameState play(const GameState &pState, const Deadline &pDue);
};

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
    int alphabeta(char *state, int depth, int alpha, int beta, Cell player, int pCell);

    /**
     * Evaluation function of a state of the game.
     * It is calculated by counting the # 2 in a rows, # 3 in a rows and # 4 in a rows of player X and return a total value
     * weighting each of the rows by a constant.
     *
     * @param state
     * @return value of the state
     */
    int evaluation(const char *state);

    /**
     * @param state the board
     * @return true if board is full of pieces (game has ended), false otherwise
     */
    bool isEOG(const char* state);

    // Functions ported from GameState to work with the representation of the board as a 1D array (char *)
    // Needed to increase performance to pass Kattis time limit
    ///returns the row corresponding to a cell index
    int cellToRow(int pCell);

    ///returns the col corresponding to a cell index
    int cellToCol(int pCell);

    ///returns the lay corresponding to a cell index
    int cellToLay(int pCell);

    /**
     * \param mCell the board
     * \param pR
     * \param pC
     * \param pL
     * \return the content of the board at cell[pR, pC, pL]
     */
    uint8_t at(const char *mCell, int pR, int pC, int pL);

    /**
     *
     * \param mCell the board
     * \param pPos
     * \return the content of the board at cell[nPos] (interpreting the 3D board as a 1D array)
     */
    uint8_t at( const char *mCell, int pPos);

    /**
     * Analyse movement of \pPlayer on cell \pCell to check if it's a special movement (X winner, O winner or draw)
     * \param mCell the board
     * \param pCell the cell where the movement has been made (the piece has been placed)
     * \param pPlayer player who has done the movement
     * \return 1 if \pPlayer has won with this movement, 2 if the movement has been a draw or 0 if it's not a special movement
     */
    int Special_Move(const char *mCell, int pCell, Cell pPlayer);



/*namespace TICTACTOE3D*/ }

#endif
