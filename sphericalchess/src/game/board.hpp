
#pragma once

#include "piece.hpp"
#include "../core.hpp"

#include <vector>

//! @brief Used to hold the state of all [pieces](@ref Piece) of a chess board.
//! @see Piece
class Board
{
public:

    //! @brief Used to explain the current state of the board for a specific Piece::Team.
    //! @todo Add a "stalemate" option? Team cannot make a move that doesnt put the king in check, but king is not currently in check.
    enum class State
    {
        Playing,        //!< Team is able to make a move.
        Check,          //!< Team is in check.
        Checkmate,      //!< Team is checkmated, no playable move out of check is avaliable.
        Stalemate,      //!< Game has resulted in a draw.
    };

    static const int kDimension = 8; //!< The length of x and y axis of the board.

    Board();

    Piece&       PieceAt(const Vec2i& position)       { return At(position); }
    const Piece& PieceAt(const Vec2i& position) const { return At(position); }
    Piece&       PieceAt(int x, int y)                { return board[x][y]; }
    const Piece& PieceAt(int x, int y) const          { return board[x][y]; }


    void SaveState(); //!< @todo implement saving? Implement a "SaveFile" (using json) maybe? Need filesystem then...


    bool FindAnyPiece(Piece::Type type, Piece::Team team, Vec2i& outPosition) const;

    State ApplyActionIfValid(const Piece::Action& action);
    State CheckState(Piece::Team team);

    State GetCurrentTeamState() const { return currentTeamState; }

    bool CheckStalemate(Piece::Team team); // todo remove or implement ?

    const Piece::Action& GetLastAction() const;

    Piece::Team GetCurrentTeamTurn() const { return whiteActions.size() == blackActions.size() ? Piece::Team::White : Piece::Team::Black; }

    const std::vector<Piece::Action>& GetWhiteActions() const { return whiteActions; }
    const std::vector<Piece::Action>& GetBlackActions() const { return blackActions; }

private:

    State currentTeamState = State::Playing;

    std::vector<Piece::Action> whiteActions;
    std::vector<Piece::Action> blackActions;

    Piece board[kDimension][kDimension];

    Piece&       At(const Vec2i& position)       { return board[position.x][position.y]; }
    const Piece& At(const Vec2i& position) const { return board[position.x][position.y]; }



};