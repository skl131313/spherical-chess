
#pragma once

#include "../core.hpp"

#include <vector>
#include <tuple>
#include <memory>

class Board;

//! @brief Used to define a chess piece for the Board class.
class Piece
{
public:

    enum class Team
    {
        White,
        Black
    };

    //! @brief Used to define what type of piece is represented by a Piece.
    enum class Type
    {
        None = -1,      //!< Used to represent an empty location on the Board.

        Pawn,
        Bishop,
        Knight,
        Rook,
        Queen,
        King,

    };

    struct Action;
    using ActionCollection = std::vector<Action>;


    Piece() = default;
    Piece(Team team, Type type) : team(team), type(type)
    {
    }

    operator bool() const { return type != Type::None; }

    Type GetType() const { return type; }
    Team GetTeam() const { return team; }

    const ActionCollection CalculatePossibleActions(const Board& board, Vec2i position) const;
    bool CanCaptureDestination(const Board& board, Vec2i position, Vec2i destination) const;

private:

    Type type = Type::None;
    Team team;

    bool moved = false;


    ActionCollection CalculatePossibleActionsPawn  (const Board& board, Vec2i position) const;
    ActionCollection CalculatePossibleActionsRook  (const Board& board, Vec2i position) const;
    ActionCollection CalculatePossibleActionsKnight(const Board& board, Vec2i position) const;
    ActionCollection CalculatePossibleActionsBishop(const Board& board, Vec2i position) const;
    ActionCollection CalculatePossibleActionsQueen (const Board& board, Vec2i position) const;
    ActionCollection CalculatePossibleActionsKing  (const Board& board, Vec2i position) const;
};

struct Piece::Action
{
    enum TypeBit
    {
        TypeBit_move    = 0,        //!< The base case, no bit set is just a basic move.
        TypeBit_castle  = 1 << 0,   //!< Special case, moves two pieces, expects all other bits to be zero if this one is set.
        TypeBit_capture = 1 << 1,   //!< Indicates a piece is captured, can't be used with TypeBit_castle.
        TypeBit_upgrade = 1 << 2,   //!< Indicates the piece should be upgraded, can't be used with TypeBit_castle.
    };

    TypeBit type;

    Piece piece;            //!< Piece that is to be moved and/or upgraded if TypeBit_upgrade is set.
    Vec2i origin;           //!< Position of piece to move, every action moves a piece to #destination.
    Vec2i destination;      //!< Position for the piece to be moved to.

    struct
    {
        Piece piece;        //!< Either state of captured piece or the rook in castle move, for TypeBit_capture or TypeBit_castle respectively.
        Vec2i origin;       //!< Either position of the captured piece or the Rook to move, for TypeBit_capture or TypeBit_castle respectively.
        Vec2i destination;  //!< Only used by TypeBit_castle, the destination to move the rook.
    } additional;           //!< Additional information only used by TypeBit_castle and TypeBit_capture.

    static Action MakeMove   (const Piece& piece, std::pair<Vec2i, Vec2i> move, bool upgrade = false);
    static Action MakeCapture(const Piece& piece, std::pair<Vec2i, Vec2i> move, const Piece& capture, Vec2i captureOrigin, bool upgrade = false);
    static Action MakeCastle (const Piece& king, std::pair<Vec2i, Vec2i> move, const Piece& rook, std::pair<Vec2i, Vec2i> rookMove);

private:

    friend class Board;

    void Apply  (Board& board) const;
    void Reverse(Board& board) const;

    static void MovePiece(Board& board, const Piece& piece, Vec2i origin, Vec2i destination);

};

