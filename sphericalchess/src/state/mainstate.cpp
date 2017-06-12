
#include "mainstate.hpp"

#include "../game/resources.hpp"

namespace
{
    struct CollideResult
    {
        enum struct Type
        {
            None,
            Board,
            Piece,
            Cap,
        };

        Type type;
        Vec2i position;
    };

    CollideResult CollideLineWorld(const Resources& resource, const Board& board, const Vec3& start, const Vec3& end)
    {
        CollideResult result{ CollideResult::Type::None };

        float percent = 1.0f;

        if(Math::CalculateIntersection(Math::Sphere{ Vec3(0,0,0), 1.0f }, start, end, percent))
        {
            Vec3 point = start + (end - start) * percent;

            float phi = std::atan2(point.y, point.x);
            float rho = std::acos(point.z);

            if(phi > 0.0f) phi -= Math::TwoPi<float>();

            int x = int(phi / -45.0_deg);
            int y = int(rho / 18.0_deg);


            if(y >= 1 && y <= Board::kDimension)
            {
                result = { CollideResult::Type::Board, Vec2i(x, y - 1) };
            }
            else
            {
                result = { CollideResult::Type::Cap };
            }
        }

        for(int i = 0; i < Board::kDimension; ++i)
        {
            for(int j = 0; j < Board::kDimension; ++j)
            {
                if(const Piece& piece = board.PieceAt(i, j))
                {
                    auto& loc = resource.locations[i][j].inverse;

                    Vec4 s = loc * Vec4(start, 1.0f);
                    Vec4 e = loc * Vec4(end, 1.0f);

                    const Resources::Piece* pieceResource;

                    switch(piece.GetType()) // todo make a function in Resource to get this instead ?
                    {
                    default:                  pieceResource = nullptr;          break;
                    case Piece::Type::Pawn:   pieceResource = &resource.pawn;   break;
                    case Piece::Type::Bishop: pieceResource = &resource.bishop; break;
                    case Piece::Type::Rook:   pieceResource = &resource.rook;   break;
                    case Piece::Type::Knight: pieceResource = &resource.knight; break;
                    case Piece::Type::Queen:  pieceResource = &resource.queen;  break;
                    case Piece::Type::King:   pieceResource = &resource.king;   break;
                    }

                    if(pieceResource == nullptr)
                    {
                        continue;
                    }

                    float output;

                    if(pieceResource->collision.Collide(Vec3(s) / s.w, Vec3(e) / e.w, percent, output))
                    {
                        percent = output;
                        result = { CollideResult::Type::Piece, Vec2i(i, j) };
                    }
                }
            }
        }

        return result;
    }
}

bool MainState::ProcessEvent(SDL_Event& ev) 
{
    int width;
    int height;

    SDL_GetWindowSize(resource.window, &width, &height);

    switch(ev.type)
    {
    case SDL_MOUSEBUTTONDOWN:
    {
        if(ev.button.button == SDL_BUTTON_LEFT)
        {
            // todo check collision, 

            Vec3 start;
            Vec3 end;

            std::tie(start, end) = CalculateMouseWorldLine(Vec2i(ev.motion.x, ev.motion.y));

            auto result = CollideLineWorld(resource, board, start, end);

            switch(result.type)
            {
            default:
            {
                mouse.state = Mouse::State::Moving;
                mouse.delta = Vec2i(0);
                break;
            }
            case CollideResult::Type::Board:
            {
                mouse.state = Mouse::State::Moving;
                mouse.delta = Vec2i(0);

                if(selectedPiece.selected)
                {
                    auto it = std::find_if(selectedPiece.actions.begin(), selectedPiece.actions.end(), [pos = result.position](Piece::Action& a)
                    {
                        return a.destination == pos;
                    });

                    if(it != selectedPiece.actions.end())
                    {
                        mouse.state = Mouse::State::Selection;
                        mouse.selection.state = Mouse::Selection::State::Board;
                        mouse.selection.position = result.position;
                    }
                    else
                    {
                        mouse.state = Mouse::State::Deselection;
                    }
                }

                break;
            }
            case CollideResult::Type::Piece:
            {
                mouse.state = Mouse::State::Moving;
                mouse.delta = Vec2i(0);

                if(board.GetCurrentTeamTurn() == board.PieceAt(result.position).GetTeam())
                {
                    mouse.state = Mouse::State::Selection;
                    mouse.selection.state = Mouse::Selection::State::Piece;
                    mouse.selection.position = result.position;
                }
                else if(selectedPiece.selected)
                {
                    if(board.GetCurrentTeamTurn() != board.PieceAt(selectedPiece.position).GetTeam())
                    {
                        selectedPiece.ClearSelected();
                    }
                    else
                    {
                        for(auto& action : selectedPiece.actions)
                        {
                            if(action.destination == result.position)
                            {
                                mouse.state = Mouse::State::Selection;
                                mouse.selection.state = Mouse::Selection::State::Board;
                                mouse.selection.position = result.position;

                                break;
                            }
                        }
                    }
                }

                break;
            }
            }

        }
        break;
    }
    case SDL_MOUSEBUTTONUP:
    {
        if(ev.button.button == SDL_BUTTON_LEFT)
        {
            switch(mouse.state)
            {
            case Mouse::State::Selection:
            {
                switch(mouse.selection.state)
                {
                case Mouse::Selection::State::Piece:
                {
                    Vec2i pos = mouse.selection.position;

                    selectedPiece.selected = true;
                    selectedPiece.position = pos;
                    selectedPiece.actions  = board.PieceAt(pos).CalculatePossibleActions(board, pos);
                    break;
                }
                case Mouse::Selection::State::Board:
                {
                    if(selectedPiece.selected)
                    {
                        auto it = std::find_if(selectedPiece.actions.begin(), selectedPiece.actions.end(), [pos = mouse.selection.position](auto& a)
                        {
                            return a.destination == pos;
                        });

                        if(it != selectedPiece.actions.end())
                        {
                            switch(board.ApplyActionIfValid(*it))
                            {
                            case Board::State::Playing:
                            case Board::State::Stalemate:
                            {
                                selectedPiece.ClearSelected();
                                

                                switch(board.GetCurrentTeamState())
                                {
                                case Board::State::Check:
                                {
                                    AddMessage("You are in check!", 100);

                                    break;
                                }
                                case Board::State::Checkmate:
                                {
                                    // todo game over...
                                    AddMessage("You win, congratulations!", 100);
                                    break;
                                }
                                }

                                break;
                            }
                            case Board::State::Check:
                            case Board::State::Checkmate:
                            {
                                
                                AddMessage("Invalid move? check.", 100); // todo account for move into check or in check..
                                break;
                            }
                            }

                        }

                        
                    }
                    break;
                }
                }
                break;
            }
            case Mouse::State::Deselection:
            {
                selectedPiece.ClearSelected();
                break;
            }
            }

            mouse.state = Mouse::State::None;
            mouse.selection.state = Mouse::Selection::State::None;
        }

        break;
    }
    case SDL_MOUSEMOTION:
    {
        switch(mouse.state)
        {
        default:
        {
            break;
        }
        case Mouse::State::Selection:
        case Mouse::State::Deselection:
        {
            mouse.delta += Vec2i(ev.motion.xrel, ev.motion.yrel);

            if(Math::LengthSqr(mouse.delta) >= 100)
            {
                mouse.state = Mouse::State::Moving;
            }
            break;
        }
        case Mouse::State::Moving:
        {
            Vec3 velocity(float(ev.motion.yrel), float(ev.motion.xrel), 0);

            float length = Math::Normalize(velocity);

            rotation = Math::AxisAngleMatrix(velocity, length / 140.0f) * rotation;
        
            break;
        }
        }
        break;
    }
    case SDL_MOUSEWHEEL:
    {
        cameraZoom = Math::Clamp(cameraZoom + ev.wheel.y / 10.0f, -25.0f, -1.25f);
        break;
    }
    }

    return false;
}

void MainState::Render()
{
    using namespace std::string_literals;

    resource.vao.Bind();

    int width;
    int height;

    SDL_GetWindowSize(resource.window, &width, &height);

    glViewport(0, 0, width, height);

    std::tie(perspectiveMatrix, perspectiveInverseMatrix) = Math::PerspectiveMatrixAndInverse(25.0_deg, float(width) / height, 0.01f, 128.0f);

    Vec3 lightDir = Vec3(0,1,0);

    Math::Normalize(lightDir);

    Vec3 eye = Math::Transpose(rotation) * Vec3(0,0,-cameraZoom);
    Mat4 world(1.0f); 
    Mat4 camera = Mat4(rotation);

    Mat4 cameraMatrix = Mat4(1.0f);
    Mat4 inverseCameraMatrix = Mat4(1.0f);

    cameraMatrix[3][2] = cameraZoom;
    inverseCameraMatrix[3][2] = -cameraZoom;

    projview        = perspectiveMatrix * cameraMatrix;
    inverseProjView = inverseCameraMatrix * perspectiveInverseMatrix;

    camera[3] = Vec4(0,0, cameraZoom, 1.0f);

    Vec3 whiteAlbedoColor(0.955f, 0.638f, 0.538f);
    Vec3 blackAlbedoColor(0.2f, 0.2f, 0.2f);
    float roughness = 0.3f;

    auto& texShader = resource.texShader;
    auto& basicShader = resource.basicShader;

    basicShader.UseProgram();

    Mat4 m = projview * Mat4(rotation);

    basicShader.SetUniform("color"s, Vec3(1,1,1));
    basicShader.SetUniform("viewproj"s, m);

    glDrawArrays(GL_TRIANGLES, resource.boardTop.first, resource.boardTop.count);

    basicShader.SetUniform("color"s, Vec3(0,0,0));

    glDrawArrays(GL_TRIANGLES, resource.boardBot.first, resource.boardBot.count);

    for(int i = 0; i < Board::kDimension; ++i)
    {
        for(int j = 0; j < Board::kDimension; ++j)
        {
            Vec2i position = Vec2i(i, j);

            Vec3 color = ((i % 2) ^ (j % 2)) ? Vec3(1,1,1) : Vec3(0,0,0);

            Mat4 m = projview * Mat4(rotation * Math::EulerAngleZ(-45.0_deg * i));

            if(selectedPiece.selected)
            {
                for(auto& action : selectedPiece.actions)
                {
                    if(action.destination == position)
                    {
                        color = Vec3(0, 1, 0);
                        break;
                    }
                }
            }

            if(hoverPosition == position)
            {
                color = Vec3(0,0,1);
            }

            basicShader.SetUniform("color"s, color);
            basicShader.SetUniform("viewproj"s, m);

            glDrawArrays(GL_TRIANGLES, resource.board[j].first, resource.board[j].count);
        }
    }

#if 0
    {
        texShader.UseProgram();

        Mat4 m = projview * Mat4(rotation);

        texShader.uniforms.diffuseTexture.Set(0, 1);
        texShader.uniforms.viewproj.Set(m, 1);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, boardTexture);


        glDrawArrays(GL_TRIANGLES, resource.board.first, resource.board.count);

    }
#endif
    auto& pbrShader = resource.pbrShader;

    pbrShader.UseProgram();


    pbrShader.SetUniform("viewproj"s, perspectiveMatrix);
    pbrShader.SetUniform("eyePosition"s, eye);
    pbrShader.SetUniform("ulightDirection"s, lightDir);
    pbrShader.SetUniform("cubemap"s, 0);
    pbrShader.SetUniform("irradiance"s, 1);
    pbrShader.SetUniform("splitsum"s, 2);
    pbrShader.SetUniform("camera"s, camera);
    pbrShader.SetUniform("eyeNormal"s, Math::Transpose(rotation) * Vec3(0, 0, 1));

    pbrShader.SetUniform("world"s, Mat4(1.0f));

    glActiveTexture(GL_TEXTURE0);
    resource.envmap.Bind();

    glActiveTexture(GL_TEXTURE1);
    resource.irradiance.Bind();

    glActiveTexture(GL_TEXTURE2);
    resource.splitsum.Bind();

    pbrShader.SetUniform("uroughnessValue"s, 0.1f);


    // glDrawArrays(GL_TRIANGLES, resource.board.first, resource.board.count);

    Vec3 color = Vec3(0,0,0);

    pbrShader.SetUniform("uroughnessValue"s, roughness);


    for(int i = 0; i < Board::kDimension; ++i)
    {
        for(int j = 0; j < Board::kDimension; ++j)
        {
            if(Piece& piece = board.PieceAt(i, j))
            {
                color = Vec3(1,0,0);
                
                if(piece.GetTeam() == Piece::Team::White)
                {
                    pbrShader.SetUniform("albedo"s, whiteAlbedoColor);
                }
                else
                {
                    pbrShader.SetUniform("albedo"s, blackAlbedoColor);
                }

                Resources::Piece* pieceResource = nullptr;

                switch(piece.GetType())
                {
                default: continue;
                case Piece::Type::Pawn:   pieceResource = &resource.pawn;   break;
                case Piece::Type::Bishop: pieceResource = &resource.bishop; break;
                case Piece::Type::Rook:   pieceResource = &resource.rook;   break;
                case Piece::Type::Knight: pieceResource = &resource.knight; break;
                case Piece::Type::Queen:  pieceResource = &resource.queen;  break;
                case Piece::Type::King:   pieceResource = &resource.king;   break;
                }

                
                Mat4 mm = resource.locations[i][j].transform * Mat4(Mat3(1.6f));


                pbrShader.SetUniform("world"s, mm);

                
                glDrawArrays(GL_TRIANGLES, pieceResource->vertexDesc.first, pieceResource->vertexDesc.count);

            }

        }
    }


#if 0
    basicShader.UseProgram();

    for(auto& action : selectedActions)
    {
                
        Vec3 color = Vec3(0,1,1);

        Mat4 m = projview * Mat4(rotation * Math::EulerAngleZ(-45.0_deg * action.destination.x));

        basicShader.uniforms.viewproj.Set(m, 1);
        basicShader.uniforms.color.Set(color, 1);

        auto& indicator = resource.indicators[action.destination.y];

        glDrawArrays(GL_TRIANGLES, indicator.first, indicator.count);

        
    }
#endif

    Rect2i window{ 0, 0, width, height };


    if(board.GetCurrentTeamTurn() == Piece::Team::White)
    {
        resource.font.Draw(window, { 10, 10 }, Font::Align::LeftTop, "White's Turn");
    }
    else
    {
        resource.font.Draw(window, { -10, 10 }, Font::Align::RightTop, "Black's Turn");
    }

    

    glBindVertexArray(0);
}

std::pair<Vec3, Vec3> MainState::CalculateMouseWorldLine(const Vec2i& screenPosition) const
{
    int width;
    int height;
    
    SDL_GetWindowSize(resource.window, &width, &height);

    float x = (screenPosition.x / (width * 0.5f)) - 1.0f;
    float y = (screenPosition.y / (height * 0.5f)) - 1.0f;

    y = -y;

    Mat4 inverse = Mat4(Math::Transpose(rotation)) * inverseProjView;
                   
    Vec4 start = inverse * Vec4(x, y, 0.0f, 1.0f);
    Vec4 end   = inverse * Vec4(x, y, 1.0f, 1.0f);

    return std::make_pair(Vec3(start) / start.w, Vec3(end) / end.w);
}

void MainState::AddMessage(const std::string& message, int time)
{

}