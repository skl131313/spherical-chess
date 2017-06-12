
#include "board.hpp"

#include "piece.hpp"

#include <cassert>



Board::Board()
{

    Piece::Type army[] =
    {
        Piece::Type::Rook,
        Piece::Type::Knight,
        Piece::Type::Bishop,
        Piece::Type::Queen,
        Piece::Type::King,
        Piece::Type::Bishop,
        Piece::Type::Knight,
        Piece::Type::Rook
    };


    for(int i = 0; i < kDimension; ++i)
    {
        board[i][0] = Piece(Piece::Team::White, army[i]);
        board[i][1] = Piece(Piece::Team::White, Piece::Type::Pawn);

        board[i][6] = Piece(Piece::Team::Black, Piece::Type::Pawn);
        board[i][7] = Piece(Piece::Team::Black, army[i]);

    }
    
}

bool Board::FindAnyPiece(Piece::Type type, Piece::Team team, Vec2i& outPosition) const
{
    for(int i = 0; i < kDimension; ++i)
    {
        for(int j = 0; j < kDimension; ++j)
        {
            if(auto& piece = board[i][j])
            {
                if(type == piece.GetType() && team == piece.GetTeam())
                {
                    outPosition = Vec2i(i, j);
                    return true;
                }
            }
        }
    }

    return false;
}

auto Board::ApplyActionIfValid(const Piece::Action& action) -> State
{
    action.Apply(*this);

    auto state = CheckState(action.piece.GetTeam());

    if(state == State::Check || state == State::Checkmate)
    {
        action.Reverse(*this);
    }
    else
    {
        if(GetCurrentTeamTurn() == Piece::Team::White)
        {
            assert(action.piece.GetTeam() == Piece::Team::White);
            whiteActions.push_back(action);
            currentTeamState = CheckState(Piece::Team::Black);
        }
        else
        {
            assert(action.piece.GetTeam() == Piece::Team::Black);
            blackActions.push_back(action);
            currentTeamState = CheckState(Piece::Team::White);
        }
    }
    

    return state;
}

Board::State Board::CheckState(Piece::Team team)
{
    Vec2i kingPosition;

    if(!FindAnyPiece(Piece::Type::King, team, kingPosition))
    {
        return State::Checkmate; // should never happen, but if we can't find the king then the games over..
    }

    // todo move out, dont keep this lambda
    auto FindKingCheck = [](const Board& board, const Vec2i& kingPosition)
    {
        auto& kingPiece = board.PieceAt(kingPosition);

        assert(kingPiece.GetType() == Piece::Type::King);

        for(int i = 0; i < kDimension; ++i)
        {
            for(int j = 0; j < kDimension; ++j)
            {
                if(auto& piece = board.PieceAt(i, j))
                {
                    if(piece.GetTeam() == kingPiece.GetTeam())
                    {
                        continue;
                    }

                    if(piece.CanCaptureDestination(board, Vec2i(i,j), kingPosition))
                    {
                        return State::Check;
                    }
                }
            }
        }

        return State::Playing;
    };


    State result = FindKingCheck(*this, kingPosition);

    // todo try to unlambda this ?
    // todo rename this..., we use it fore stalemate below (not in check)
    auto FindMoveOutOfCheck = [&]()
    {
        auto& kingPiece = At(kingPosition);

        {
            auto kingActions = kingPiece.CalculatePossibleActions(*this, kingPosition);

            for(auto& action : kingActions)
            {
                action.Apply(*this);

                auto result = FindKingCheck(*this, action.destination);

                action.Reverse(*this);

                if(result == State::Playing)
                {
                    return true;
                }
            }
        }

        for(int i = 0; i < kDimension; ++i)
        {
            for(int j = 0; j < kDimension; ++j)
            {
                if(auto& piece = PieceAt(i, j))
                {
                    if(piece.GetTeam() == kingPiece.GetTeam())
                    {
                        if(piece.GetType() == Piece::Type::King)
                        {
                            continue; // king is special case handled above, skip it here
                        }

                        auto actions = piece.CalculatePossibleActions(*this, Vec2i(i, j));

                        for(auto& action : actions)
                        {
                            action.Apply(*this);

                            auto result = FindKingCheck(*this, kingPosition);

                            action.Reverse(*this);

                            if(result == State::Playing)
                            {
                                return true;
                            }
                        }

                    }
                }
            }
        }

        return false;
    };

    if(result == State::Check)
    {
        if(!FindMoveOutOfCheck())
        {
            result = State::Checkmate;
        }
    }
    else
    {
        if(!FindMoveOutOfCheck())
        {
            result = State::Stalemate;
        }
    }

    return result;
}

bool Board::CheckStalemate(Piece::Team team)
{
    assert(0);
    return false;
}

const Piece::Action& Board::GetLastAction() const
{
    return whiteActions.size() != blackActions.size() ? whiteActions.back() : blackActions.back();
}
