
#pragma once

#include "state.hpp"

#include <SDL.h>

#include <vector>
#include <memory>


class StateManager
{
public:
    
    using StatePtr = std::unique_ptr<State>;

    StateManager() = default;
    StateManager(const StateManager&) = delete;
    
    void AddState(StatePtr&& state)
    {
        stateChangeType = StateChangeType::Add;
        desiredState = std::move(state);
    }

    void SwapState(StatePtr&& state)
    {
        stateChangeType = StateChangeType::Swap;
        desiredState = std::move(state);
    }

    void UpdateStateChange();
    void Update();
    
    void ProcessEvent(SDL_Event& ev);

    void Render();

    State* GetTopState() { return states.back().get(); } //!< @todo remove this function

private:


    enum class StateChangeType
    {
        None,
        Swap,
        Add
    };

    StateChangeType stateChangeType = StateChangeType::None;
    StatePtr desiredState;

    std::vector<StatePtr> states;

};