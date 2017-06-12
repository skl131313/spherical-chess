
#include "statemanager.hpp"

void StateManager::UpdateStateChange()
{
    switch(stateChangeType)
    {
    case StateChangeType::Add:
    {
        states.push_back(std::move(desiredState));
        break;
    }
    case StateChangeType::Swap:
    {
        states.pop_back();
        states.push_back(std::move(desiredState));
        break;
    }
    }

    stateChangeType = StateChangeType::None;
}

void StateManager::Update()
{

}

void StateManager::ProcessEvent(SDL_Event& ev)
{
    for(auto it = states.rbegin(); it != states.rend(); ++it)
    {
        if((*it)->ProcessEvent(ev) == false)
        {
            break;
        }
    }
}

void StateManager::Render()
{
    for(auto& state : states)
    {
        state->Render();
    }
}