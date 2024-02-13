#include "Input.h"
#include "Core.h"
#include <algorithm>

namespace GameEngine
{
    /*
    * constructor that initializes OPENAL controller and assigns any found
    */
    Input::Input(std::weak_ptr<Core> _core) : 
        m_keys(), m_pressedKeys(), m_releasedKeys(), m_mousePos(glm::vec2(0)),
        m_core(_core), m_lockMouse(true), m_mouseDelta(glm::vec2(0)), m_controller(nullptr)
    {
        //Init controller support
        SDL_Init(SDL_INIT_GAMECONTROLLER);
        // Loop through all joysticks and if one is a controller assign to variable
        for (int i = 0; i < SDL_NumJoysticks(); i++)
        {
            if (SDL_IsGameController(i))
            {
                m_controller = SDL_GameControllerOpen(i);
                break;
            }
        }
    }

    /*
    * cleans up controller pointers
    */
    Input::~Input()
    {
        if (m_controller != NULL)
        {
            SDL_GameControllerClose(m_controller);
        }

        m_controller = nullptr;
    }

    /*
    * returns if a key is currently being held down
    */
    bool Input::isKey(int _key)
    {
        for(int key : m_keys)
        {
            if (_key == key) {
                return true;
            }
        }
        return false;
    }

    /*
    * returns if a key has been pressed this frame
    */
    bool Input::isKeyPressed(int _key)
    {
        for(int key : m_pressedKeys)
        {
            if (_key == key) {
                return true;
            }
        }
        return false;
    }

    /*
   * returns if a key has been released this frame
   */
    bool Input::isKeyReleased(int _key)
    {
        for(int key : m_releasedKeys)
        {
            if (_key == key) {
                return true;
            }
        }
        return false;
    }
   
    /*
   * loops through all SDL poll events to check for mouse, keyboard and controller input
   */
    void Input::tick()
    {
        //reset mouse delta each frame
        m_mouseDelta.x = 0;
        m_mouseDelta.y = 0;

        while (SDL_PollEvent(&m_event) != 0)
        {
             if (m_event.type == SDL_MOUSEMOTION) //check for any mouse movement
             {
                 m_dm = true;
                 if (m_mset > 0) //work out mouse input values
                 {
                     m_mousePos.x = m_event.motion.x; // Set mouse position on screen from SDL
                     m_mousePos.y = m_event.motion.y;

                     m_mouseDelta.x += m_mousePos.x - m_core.lock()->m_nativeWindow->m_windowWidth / 2;
                     m_mouseDelta.y += m_mousePos.y - m_core.lock()->m_nativeWindow->m_windowHeight / 2;
                 }
            }
            else if (m_event.type == SDL_KEYDOWN) //check if any keys have been pressed down
            {
                m_pressedKeys.push_back(m_event.key.keysym.sym); //Add to pressed keys this frame

                //Check if key is already in list
                bool check = true;
                for(int key : m_keys)
                {
                    if (m_event.key.keysym.sym == key) {
                        check = false;
                    }
                }
                if (check)
                {
                    m_keys.push_back(m_event.key.keysym.sym); //Add to held keys list
                }
            }
            else if (m_event.type == SDL_KEYUP || m_event.type == SDL_MOUSEBUTTONUP)
            {
                m_releasedKeys.push_back(m_event.key.keysym.sym); //Add to released keys this frame

                //Remove from held keys list
                std::vector<int>::iterator position = std::find(m_keys.begin(), m_keys.end(), m_event.key.keysym.sym); //find key in list
                if (position != m_keys.end()) // == myVector.end() means the element was not found
                    m_keys.erase(position);
            }
            else if (m_event.type == SDL_MOUSEBUTTONDOWN) // Mouse input
            {
                m_mouseDown = true;
                m_mouseDownClick = true;
            }
            else if (m_event.type == SDL_MOUSEBUTTONUP)
            {
                m_mouseDown = false;
            }
            else if (m_event.type == SDL_CONTROLLERBUTTONDOWN) //Controller input
            {
                if (m_event.cbutton.button == SDL_CONTROLLER_BUTTON_A)
                {
                    std::cout << "BUTTON A PRESSED" << std::endl;
                }
            }
            else if (m_event.type == SDL_JOYAXISMOTION)
            {
                if (m_event.jaxis.axis == 0) // X axis motion
                {
                    if (m_event.jaxis.value < -JOYSTICK_DEAD_ZONE) // Left of dead zone
                    {
                        m_keys.push_back(SDLK_a);
                    }
                    else if (m_event.jaxis.value > JOYSTICK_DEAD_ZONE) // Right of dead zone
                    {
                        m_keys.push_back(SDLK_d);
                    }
                    else
                    {
                        //Remove from held keys list
                        std::vector<int>::iterator position = std::find(m_keys.begin(), m_keys.end(), SDLK_a); //find key in list
                        if (position != m_keys.end()) // == myVector.end() means the element was not found
                            m_keys.erase(position);

                        position = std::find(m_keys.begin(), m_keys.end(), SDLK_d); //find key in list
                        if (position != m_keys.end()) // == myVector.end() means the element was not found
                            m_keys.erase(position);
                    }
                }
                else if (m_event.jaxis.axis == 1)
                {
                    if (m_event.jaxis.value < -JOYSTICK_DEAD_ZONE) // Below of dead zone
                    {
                        m_keys.push_back(SDLK_w);
                    }
                    else if (m_event.jaxis.value > JOYSTICK_DEAD_ZONE) // Above of dead zone
                    {
                        m_keys.push_back(SDLK_s);
                    }
                    else
                    {
                        //Remove from held keys list
                        std::vector<int>::iterator position = std::find(m_keys.begin(), m_keys.end(), SDLK_s); //find key in list
                        if (position != m_keys.end()) // == myVector.end() means the element was not found
                            m_keys.erase(position);

                        position = std::find(m_keys.begin(), m_keys.end(), SDLK_w); //find key in list
                        if (position != m_keys.end()) // == myVector.end() means the element was not found
                            m_keys.erase(position);
                    }
                }
            }
        }

        if (isKeyPressed(SDLK_TAB))
        {   // When tab is pressed lock / unlock mouse
            m_lockMouse = m_lockMouse ? false : true;
        }
        if (m_lockMouse)
        {   // Reset the mouse to the middle of the screen each frame
            SDL_WarpMouseInWindow(NULL, m_core.lock()->m_nativeWindow->m_windowWidth / 2, m_core.lock()->m_nativeWindow->m_windowHeight / 2);
        }

        // When the mouse first gets on the screen
        if (m_dm)
        {
            m_mset = 1;
        }
    }

    void Input::clear()
    {
        //Only valid for one frame
        m_pressedKeys.clear();
        m_releasedKeys.clear();
        m_mouseDownClick = false;
    }
}