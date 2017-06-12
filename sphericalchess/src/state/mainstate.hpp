
#pragma once

#include "state.hpp"

#include "../game/board.hpp"
#include "../core.hpp"

struct Resources;

class MainState : public State
{
public:

    explicit MainState(StateManager& stateManager, Resources& resource) : stateManager(stateManager), resource(resource)
    {
    }

    bool ProcessEvent(SDL_Event& ev) override;
    void Render() override;


// private: // todo re-add private

    struct Mouse
    {
        enum class State
        {
            None,
            Moving,
            Selection,
            Deselection,
        };

        struct Selection
        {
            enum class State
            {
                None,
                Piece,
                Board,
            };

            State state = State::None;
            Vec2i position;
            
        };

        State state = State::None;
        Vec2i delta;

        Selection selection;

        void SetStateNone()
        {
            state = State::None;
            selection.state = Selection::State::None;
        }

        void SetStateMoving()
        {
            state = State::Moving;
            delta = Vec2i(0);
        }

        void SetStateDeselection()
        {
            state = State::Deselection;
            delta = Vec2i(0);
        }

        void SetStateSelection(Selection::State select, Vec2i position)
        {
            state = State::Selection;
            delta = Vec2i(0);

            selection.state = select;
            selection.position = position;
        }

    };

    struct SelectedPiece
    {
        bool selected = false;
        Vec2i position;
        Piece::ActionCollection actions;

        void ClearSelected()
        {
            selected = false;
            actions.clear();
        }
    };

    StateManager& stateManager;
    Resources&    resource;

    Board board;

    float cameraZoom = -6.0f;
    Mat3 rotation = Mat3(1.0f);

    Mat4 perspectiveMatrix;
    Mat4 perspectiveInverseMatrix;

    Mat4 projview;
    Mat4 inverseProjView;

    Mouse         mouse;
    SelectedPiece selectedPiece;

    Vec2i hoverPosition = Vec2i(1, 0); // todo remove

    Piece::Team teamTurn = Piece::Team::White;

    std::vector<std::pair<std::string, int>> messages;


    std::pair<Vec3, Vec3> CalculateMouseWorldLine(const Vec2i& screenPosition) const;
        
    void AddMessage(const std::string& message, int time);

};