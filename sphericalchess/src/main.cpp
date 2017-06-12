//! @mainpage
//!
//! These pages contain the API documentation of Spherical Chess.
//!
//!
//!
//! @section Licenses
//!
//! @subsection SDL2
//! @todo SDL2 license
//!
//! @subsection NodePng
//! @todo nopepng license
//!
//! @subsection Json
//! @todo Json.hpp license
//!
//! @subsection Glad
//! @todo glad license
//!
//! @author Mark


#include "SDL_main.h"
#include "SDL.h"

#include "game/resources.hpp"

#include "state/statemanager.hpp"
#include "state/mainstate.hpp"


#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <cassert>


void DebugCallback(const char *name, void *funcptr, int len_args, ...)
{
    GLenum code = glad_glGetError();

    if (code != GL_NO_ERROR)
    {

        const char* errorName = "no name for error";

        switch(code)
        {
        case GL_INVALID_ENUM:                  errorName = "GL_INVALID_ENUM"; break;
        case GL_INVALID_VALUE:                 errorName = "GL_INVALID_VALUE"; break;
        case GL_INVALID_OPERATION:             errorName = "GL_INVALID_OPERATION"; break;
        case GL_OUT_OF_MEMORY:                 errorName = "GL_OUT_OF_MEMORY"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: errorName = "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
        }


        fprintf(stderr, "ERROR %d (%s) in %s\n", code, errorName, name);
    }
}


extern "C"
int main(int argc, char* argv[]) try
{

    Resources resource;
    resource.Load();

#ifdef GLAD_DEBUG
     glad_set_post_callback(DebugCallback);
#endif

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    StateManager stateManager;

    stateManager.AddState(std::make_unique<MainState>(stateManager, resource));
    stateManager.UpdateStateChange();

    bool running = true;

    while(running)
    {
        SDL_Event ev;

        while(SDL_PollEvent(&ev))
        {
            switch(ev.type)
            {
            case SDL_QUIT:
                running = false;
                break;
            default:
                stateManager.ProcessEvent(ev);
                break;
            }
        }

        stateManager.UpdateStateChange();

        // todo regulate rendering time..

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        stateManager.Render();

        int width;
        int height;

        SDL_GetWindowSize(resource.window, &width, &height);

        Rect2i window{ 0, 0, width, height };

        // resource.font.Draw(window, {  10, 10 }, Font::Align::LeftTop, "White");
        // resource.font.Draw(window, { -10, 10 }, Font::Align::RightTop, "Black");

#if 0
        resource.font.Draw(window, { 0, 0 }, Font::Align::LeftTop, "LT");
        resource.font.Draw(window, { 0, 0 }, Font::Align::Left, "LC");
        resource.font.Draw(window, { 0, 0 }, Font::Align::LeftBot, "LB");

        resource.font.Draw(window, { 0, 0 }, Font::Align::Top, "CT");
        resource.font.Draw(window, { 0, 0 }, Font::Align::Center, "CC");
        resource.font.Draw(window, { 0, 0 }, Font::Align::Bot, "CB");

        resource.font.Draw(window, { 0, 0 }, Font::Align::RightTop, "RT");
        resource.font.Draw(window, { 0, 0 }, Font::Align::Right, "RC");
        resource.font.Draw(window, { 0, 0 }, Font::Align::RightBot, "RB");
#endif

        SDL_GL_SwapWindow(resource.window);

    }

    return 0;
}
catch(const std::exception& ex)
{
    std::cerr << ex.what() << std::endl;

    return 1;
}
