
#pragma once

#include <SDL.h>

class StateManager;

class State
{
public:


    //! @brief Process events for this state.
    //! @returns True if this event should be propagated to the next state, otherwise event should be consumed.
    virtual bool ProcessEvent(SDL_Event& ev) = 0;


    virtual void Render() = 0;

};