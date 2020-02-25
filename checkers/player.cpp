#include "player.hpp"
#include <cstdlib>
#include <algorithm> // for shuffling the list
#include <random>
#include <cstdint>

namespace checkers
{

bool deep;

int8_t posValue[32] = {3,3,3,3,
                       3,1,1,3,
                       3,1,1,3,
                       3,5,5,3,
                       3,5,5,3,
                       3,1,1,3,
                       3,1,1,3,
                       6,6,6,6};

int8_t heuristic(const GameState* board, uint8_t player) {
    std::string s = board->toMessage();
    int8_t red = 0;
    int8_t white = 0;
    uint8_t index = 0;
    for (char c : s) {
        ++index;
        if (c == 'w') white += 1 * posValue[index];
        else if (c == 'W') white += 2 * posValue[index];
        else if (c == 'r') red += 1 * posValue[31 - index];
        else if (c == 'R') red += 2 * posValue[31 - index];
    }
    if (player == CELL_WHITE) return (white - red);
    if (player == CELL_RED) return (red - white);
}

int8_t whoWins(const GameState* board, uint8_t player) {
    if (player == CELL_WHITE) { // white
        if (board->isWhiteWin()) return 100;
        else if (board->isRedWin()) return -100;
    }
    else if (player == CELL_RED) { // red
        if (board->isRedWin()) return 100;
        else if (board->isWhiteWin()) return -100;
    }
    return 0;
}

int8_t minimax(const GameState* board, uint8_t depth, uint8_t player, int8_t alpha, int8_t beta, double deadline) {
    if (Deadline::now().getSeconds() > deadline) throw std::exception(); // keep track of time
    if (board->isEOG()) return whoWins(board, player);
    if (depth == 0) return deep ? 0 : heuristic(board, player);

    int8_t value = 0;
    int8_t bestValue = 0;

    std::vector<GameState> pBoards;
    board->findPossibleMoves(pBoards);
    int8_t moves = pBoards.size();
    auto rng = std::default_random_engine {};
    std::shuffle(std::begin(pBoards), std::end(pBoards), rng);

    if (board->getNextPlayer() == player) { // maximizing player
        bestValue = -127;
        for (int8_t i = 0; i < moves; ++i) {
            value = minimax(&(pBoards[i]), depth - 1, player, alpha, beta, deadline);
            if (value > bestValue) {
                bestValue = value;
                if (value > alpha) alpha = value;
            }
            if (beta <= alpha) break;
        }
    }
    else { // minimizing player
        bestValue = 127;
        for (int8_t i = 0; i < moves; ++i) {
            value = minimax(&(pBoards[i]), depth - 1, player, alpha, beta, deadline);
            if (value < bestValue) {
                bestValue = value;
                if (value < beta) beta = value;
            }
            if (beta <= alpha) break;
        }
    }
    return bestValue;
}

GameState Player::play(const GameState &board,const Deadline &pDue) {
    double deadline = pDue.getSeconds() - 0.0001;
    uint8_t player = board.getNextPlayer();
    //std::cerr << "PLAYER " << int(player);

    std::vector<GameState> pBoards;
    board.findPossibleMoves(pBoards);

    int8_t moves = pBoards.size();

    if (moves == 0) return GameState(board, Move());

    auto rng = std::default_random_engine {};
    std::shuffle(std::begin(pBoards), std::end(pBoards), rng);

    int8_t value = 0;
    int8_t bestValue = -127;
    int8_t bestIndex = 0;
    int8_t choise = 0;
    int8_t i = 0;
    uint8_t depth = 6;
    int8_t alpha = -127;
    int8_t beta = 127;
    deep = false;
    try {
        for (;;++depth) {
            bestValue = -127;
            for (uint8_t j = 0; j < moves; ++j) {
                i = (choise + j) % moves; // start with the move that was best at the previous depth
                value = minimax(&(pBoards[i]), depth, player, alpha, beta, deadline);
                if (value > bestValue) {
                    bestValue = value;
                    bestIndex = i;
                    if (value > alpha) alpha = value;
                    if (value == 100) {choise = bestIndex; throw std::exception();}
                }
                if (beta <= alpha) break;
            }
            choise = bestIndex;
            deep = true;
            if(depth == 6) depth++;
        }
    }
    catch (std::exception e) {std::cerr << "Reached depth " << int (depth) << std::endl;}

    return pBoards[choise];
}

/*namespace checkers*/ }


