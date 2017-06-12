
#include "piece.hpp"

#include "board.hpp"

#include <unordered_set>
#include <functional>

#include <cassert>
#include <cstdint>

namespace
{
    struct Vec2iHash
    {
        using Type = std::uint64_t;
        using Hash = std::hash<Type>;


        Hash::result_type operator() (const Vec2i& v) const
        {
            using U = std::make_unsigned<decltype(Vec2i::x)>::type;
            constexpr U kMax = std::numeric_limits<U>::max();

            static_assert(sizeof(U) == 4, "Vec2i component needs to be 32-bit");

            Type a = (Type(v.x) & kMax) << 32;
            Type b = (Type(v.y) & kMax);

            return hash(a | b);
        }


    private:
        Hash hash;
    };

    using PositionSet = std::unordered_set<Vec2i, Vec2iHash>;

    //! @brief Helper that makes sure @p position wraps properly around the board and stays in bounds.
    Vec2i WrapPosition(Vec2i position)
    {
        
        // todo, should this be moved into Board? Should only ever be used by Piece though...

        // todo this can be simplified due to a refactor of GeneratePositionDirection() which changes the delta diration
        // instead of accumulating it

        static_assert(Board::kDimension % 2 == 0, "Board dimension must be even number."); // should only ever be 8x8 but just incase..

        static constexpr int kDimension = Board::kDimension;
        static constexpr int kHalf      = kDimension / 2;
        static constexpr int kTwice     = kDimension * 2;

        // map everything negative to positive in Y axis

        if(position.y < 0)
        {
            position.y = -position.y - 1;
            position.x += kHalf;
        }
        
        // Y axis oscillates at 2 * kDimension

        position.y %= kTwice;

        if(position.y >= kDimension)
        {
            // need to backtrack on the Y axis for the second half
            // and be on the other side for the X axis

            position.y = (kTwice - 1) - position.y;
            position.x += kHalf;
        }

        position.x %= kDimension;

        if(position.x < 0)
        {
            position.x += kDimension;
        }

        return position;
    }

    Vec2i HVNextDestination(const Vec2i& previousDestination, Vec2i& delta)
    {
        Vec2i next = previousDestination + delta;

        if(!Util::InRange(next.y, 0, Board::kDimension))
        {
            delta.y = -delta.y;
        }

        return WrapPosition(next);
    }

    Vec2i DiagonalNextDestination(const Vec2i& previousDestination, Vec2i& delta)
    {
        Vec2i next = previousDestination + delta;

        if(!Util::InRange(next.y, 0, Board::kDimension))
        {
            // diagonal crosses the pole in one dimension, as to stay on same color space

            next = previousDestination + Vec2i(0, delta.y);
            delta = -delta;
        }

        return WrapPosition(next);
    }

    template<typename F>
    Vec2i GeneratePositionDirection(
        const Board& board,
        const Vec2i& position,
        const Vec2i& end,
        Vec2i        delta,
        F            nextDestination,
        PositionSet& output)
    {

        const Piece& movingPiece = board.PieceAt(position);

        Vec2i destination = position;

        while((destination = nextDestination(Vec2i(destination), delta)) != end)
        {
            if(const Piece& piece = board.PieceAt(destination))
            {
                if(movingPiece.GetTeam() != piece.GetTeam())
                {
                    output.insert(destination);
                }

                return destination;
            }

            output.insert(destination);
        }

        return end;
    }

    void GeneratePositionsHorizontal(const Board& board, const Vec2i& position, PositionSet& output)
    {
        Vec2i destination = GeneratePositionDirection(board, position, position, Vec2i(1, 0), HVNextDestination, output);

        if(destination != position)
        {
            GeneratePositionDirection(board, position, destination, Vec2i(-1, 0), HVNextDestination, output);
        }
    }

    void GeneratePositionsVertical(const Board& board, const Vec2i& position, PositionSet& output)
    {
        Vec2i destination = GeneratePositionDirection(board, position, position, Vec2i(0, 1), HVNextDestination, output);
                            
        if(destination != position)
        {
            GeneratePositionDirection(board, position, destination, Vec2i(0, -1), HVNextDestination, output);
        }
    }

    void GeneratePositionsDiagonal(const Board& board, const Vec2i& position, PositionSet& output)
    {
        Vec2i destination = GeneratePositionDirection(board, position, position, Vec2i(1, 1), DiagonalNextDestination, output);
                            
        if(destination != position)
        {
            GeneratePositionDirection(board, position, destination, Vec2i(-1, -1), DiagonalNextDestination, output);
        }


        destination = GeneratePositionDirection(board, position, position, Vec2i(-1, 1), DiagonalNextDestination, output);

        if(destination != position)
        {
            GeneratePositionDirection(board, position, destination, Vec2i(1, -1), DiagonalNextDestination, output);
        }
    }

    void VerifyPositions(const Board& board, const Vec2i& position, std::size_t num, const Vec2i deltas[], PositionSet& output)
    {
        const Piece& movingPiece = board.PieceAt(position);

        for(int i = 0; i < num; ++i)
        {
            Vec2i destination = WrapPosition(position + deltas[i]);

            if(const Piece& piece = board.PieceAt(destination))
            {
                if(movingPiece.GetTeam() != piece.GetTeam())
                {
                    output.insert(destination);
                }
            }
            else
            {
                output.insert(destination);
            }
        }
    }

    void ActionsAddFromPositions(
        const Board&             board,
        const Vec2i&             position,
        const PositionSet&       positionSet,
        Piece::ActionCollection& actions)
    {
        auto& piece = board.PieceAt(position);

        for(Vec2i destination : positionSet)
        {
            if(auto& captured = board.PieceAt(destination))
            {
                actions.push_back(Piece::Action::MakeCapture(piece, std::make_pair(position, destination), captured, destination));
            }
            else
            {
                actions.push_back(Piece::Action::MakeMove(piece, std::make_pair(position, destination)));
            }
        }
    }

    
}



const Piece::ActionCollection Piece::CalculatePossibleActions(const Board& board, Vec2i position) const
{
    switch(type)
    {
    case Type::Pawn:   return CalculatePossibleActionsPawn  (board, position);
    case Type::Rook:   return CalculatePossibleActionsRook  (board, position);
    case Type::Knight: return CalculatePossibleActionsKnight(board, position);
    case Type::Bishop: return CalculatePossibleActionsBishop(board, position);
    case Type::Queen:  return CalculatePossibleActionsQueen (board, position);
    case Type::King:   return CalculatePossibleActionsKing  (board, position);
    }

    return ActionCollection();
}

bool Piece::CanCaptureDestination(const Board& board, Vec2i position, Vec2i destination) const
{
    auto actions = CalculatePossibleActions(board, position);

    for(auto& action : actions)
    {
        if(action.type & Action::TypeBit_capture)
        {
            if(action.destination == destination)
            {
                return true;
            }
        }
    }

    return false;

}

Piece::ActionCollection Piece::CalculatePossibleActionsPawn(const Board& board, Vec2i position) const
{
    const int moveDirection = team == Team::White ? 1 : -1;
    const int startingRow   = team == Team::White ? 1 : 6;  
    const int upgradeRow    = team == Team::White ? 7 : 0;
    const int enPassantRow  = team == Team::White ? 4 : 3;  // Row this pawn has to be at in order to be able to do En Passant

    assert(position.y != upgradeRow); // Pawn should never be able to exist on the upgrade row, as it gets upgraded

    if(position.y == upgradeRow)
    {
        return ActionCollection();
    }

    ActionCollection actions;

    // Forward Movement //

    {
        Vec2i destination = { position.x, position.y + moveDirection };

        if(!board.PieceAt(destination))
        {
            actions.push_back(Action::MakeMove(*this, std::make_pair(position, destination), destination.y == upgradeRow));

            // pawn's special first move by 2 spaces

            if(!moved && position.y == startingRow)
            {
                destination = Vec2i(position.x, position.y + 2 * moveDirection);

                if(!board.PieceAt(destination))
                {
                    actions.push_back(Action::MakeMove(*this, std::make_pair(position, destination)));
                }
            }
        }
    }

    // Pawn Diagonal Capture //

    const Vec2i deltas[] =
    {
        Vec2i(1,  moveDirection),
        Vec2i(-1, moveDirection),
    };

    PositionSet positions;
    VerifyPositions(board, position, std::size(deltas), deltas, positions);

    for(const Vec2i& destination : positions)
    {
        if(const Piece& piece = board.PieceAt(destination))
        {
            actions.push_back(Action::MakeCapture(*this, std::make_pair(position, destination), piece, destination, destination.y == upgradeRow));
        }
    }

    // En Passant //

    if(position.y == enPassantRow)
    {
        auto& lastAction = board.GetLastAction();
        auto& lastPiece  = lastAction.piece;

        if(lastPiece.type == Type::Pawn && !lastPiece.moved && std::abs(lastAction.destination.y - lastAction.origin.y) == 2)
        {
            for(auto& delta : deltas)
            {
                Vec2i destination = WrapPosition(position + delta);

                if(destination.x == lastAction.origin.x)
                {
                    auto& piece = board.PieceAt(lastAction.destination);
                    actions.push_back(Action::MakeCapture(*this, std::make_pair(position, destination), piece, lastAction.destination));
                    break;
                }
            }
        }

    }

    return actions;
}

Piece::ActionCollection Piece::CalculatePossibleActionsRook(const Board& board, Vec2i position) const
{
    ActionCollection actions;
    PositionSet positionSet;

    GeneratePositionsHorizontal(board, position, positionSet);
    GeneratePositionsVertical(board, position, positionSet);

    ActionsAddFromPositions(board, position, positionSet, actions);

    return actions;
}

Piece::ActionCollection Piece::CalculatePossibleActionsKnight(const Board& board, Vec2i position) const
{
    const Vec2i actionDeltas[] =
    {
        {  1,  2 }, {  2,  1 },
        {  1, -2 }, {  2, -1 },
        { -1, -2 }, { -2, -1 },
        { -1,  2 }, { -2,  1 },
    };

    ActionCollection actions;
    PositionSet positionSet;

    VerifyPositions(board, position, std::size(actionDeltas), actionDeltas, positionSet);

    ActionsAddFromPositions(board, position, positionSet, actions);


    return actions;
}

Piece::ActionCollection Piece::CalculatePossibleActionsBishop(const Board& board, Vec2i position) const
{
    ActionCollection actions;
    PositionSet positionSet;

    GeneratePositionsDiagonal(board, position, positionSet);

    ActionsAddFromPositions(board, position, positionSet, actions);

    return actions;
}

Piece::ActionCollection Piece::CalculatePossibleActionsQueen(const Board& board, Vec2i position) const
{
    ActionCollection actions;
    PositionSet positionSet;

    GeneratePositionsDiagonal(board, position, positionSet);
    GeneratePositionsHorizontal(board, position, positionSet);
    GeneratePositionsVertical(board, position, positionSet);

    ActionsAddFromPositions(board, position, positionSet, actions);

    return actions;
}

Piece::ActionCollection Piece::CalculatePossibleActionsKing(const Board& board, Vec2i position) const
{
    const Vec2i actionDeltas[] =
    {
        { -1,  1 }, { 0,  1 }, { 1,  1 },
        { -1,  0 },            { 1,  0 },
        { -1, -1 }, { 0, -1 }, { 1, -1 },
    };

    ActionCollection actions;
    PositionSet positionSet;

    VerifyPositions(board, position, std::size(actionDeltas), actionDeltas, positionSet);
    ActionsAddFromPositions(board, position, positionSet, actions);

    // Castling //

    auto& king = board.PieceAt(position);
    
    auto AddRookCastleActions = [](const Board& board, Vec2i kingPosition, Vec2i rookPosition, ActionCollection& actions) -> void
    {
        auto& king = board.PieceAt(kingPosition);
        auto& rook = board.PieceAt(rookPosition);

        assert(king.type == Type::King);

        if(king.type != Type::King || king.moved)
        {
            return;
        }

        if(rook.type != Type::Rook || rook.moved)
        {
            return;
        }

        if(kingPosition.y != rookPosition.y)
        {
            return;
        }

        assert(kingPosition.x == 4); // 4 = King's starting position in x axis

        if(kingPosition.x != 4)
        {
            return;
        }


        auto CheckSpaceBetweenEmpty = [](const Board& board, int xKing, int xRook, int y, int direction) -> bool
        {
            assert(std::abs(direction) == 1);

            const Vec2i rookPosition = { xRook, y };
            
            for(Vec2i position = { xKing + direction, y }; (position = WrapPosition(position)) != rookPosition; position.x += direction)
            {
                if(board.PieceAt(position))
                {
                    return false;
                }
            }

            return true;
        };


        for(int direction : { 1, -1 })
        {
            if(CheckSpaceBetweenEmpty(board, kingPosition.x, rookPosition.x, kingPosition.y, direction))
            {
                auto kingMove = std::make_pair(kingPosition, kingPosition + direction * Vec2i(2, 0));
                auto rookMove = std::make_pair(rookPosition, kingPosition + direction * Vec2i(1, 0));
                actions.push_back(Action::MakeCastle(king, kingMove, rook, rookMove));
            }
        }

    };

        
    AddRookCastleActions(board, position, Vec2i(0,                     position.y), actions);
    AddRookCastleActions(board, position, Vec2i(Board::kDimension - 1, position.y), actions);

    return actions;
}

Piece::Action Piece::Action::MakeMove(const Piece& piece, std::pair<Vec2i, Vec2i> move, bool upgrade)
{
    Action action;

    action.piece       = piece;
    action.origin      = move.first;
    action.destination = move.second;

    action.type = upgrade ? TypeBit_upgrade : TypeBit_move;

    return action;
}

Piece::Action Piece::Action::MakeCapture(
    const Piece&            piece,
    std::pair<Vec2i, Vec2i> move,
    const Piece&            capture,
    Vec2i                   captureOrigin,
    bool                    upgrade)
{
    Action action;

    action.piece       = piece;
    action.origin      = move.first;
    action.destination = move.second;

    action.additional.piece  = capture;
    action.additional.origin = captureOrigin;

    action.type = upgrade ? TypeBit(TypeBit_capture | TypeBit_upgrade) : TypeBit_capture;

    return action;
}

Piece::Action Piece::Action::MakeCastle(
    const Piece&            king,
    std::pair<Vec2i, Vec2i> move,
    const Piece&            rook,
    std::pair<Vec2i, Vec2i> rookMove)
{
    Action action;

    action.piece       = king;
    action.origin      = move.first;
    action.destination = move.second;

    action.additional.piece       = rook;
    action.additional.origin      = rookMove.first;
    action.additional.destination = rookMove.second;

    action.type = TypeBit_castle;

    return action;
}

void Piece::Action::Apply(Board& board) const
{
    // capture first incase destination is same as captured piece (most common case)

    if(type & TypeBit_capture)
    {
        board.PieceAt(additional.origin) = Piece();
    }

    MovePiece(board, (type & TypeBit_upgrade) ? Piece(piece.team, Piece::Type::Queen) : piece, origin, destination);

    if(type & TypeBit_castle)
    {
        assert(destination != additional.destination);
        assert(type == TypeBit_castle);

        MovePiece(board, additional.piece, additional.origin, additional.destination);
    }
    
}

void Piece::Action::Reverse(Board& board) const
{
    // reverse move first incase destination is same as the captured piece (most common case)

    board.PieceAt(destination) = Piece();
    board.PieceAt(origin)      = piece;

    if(type & TypeBit_capture)
    {
        board.PieceAt(additional.origin) = additional.piece;
    }

    if(type & TypeBit_castle)
    {
        assert(destination != additional.destination);
        assert(type == TypeBit_castle);

        board.PieceAt(additional.destination) = Piece();
        board.PieceAt(additional.origin)      = additional.piece;
    }

}

void Piece::Action::MovePiece(Board& board, const Piece& piece, Vec2i origin, Vec2i destination)
{
    board.PieceAt(origin) = Piece();
    board.PieceAt(destination) = piece;
    board.PieceAt(destination).moved = true;
}
