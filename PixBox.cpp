#include "PixBox.h"

#include <iostream>

using namespace std;

PixBox* PixBox::_instance = NULL;

PixBox::PixBox():
  _graphics(new GraphicElements),
  _status(new GraphicStatus),
  _content(new Content),
  _isQuiet(true)
{
}

PixBox::~PixBox()
{
  delete _graphics;
  delete _status;
  delete _content;
}

PixBox* PixBox::instance()
{
  if(_instance == NULL)
    _instance = new PixBox;

  return _instance;
}

bool PixBox::init(bool isQuiet)
{
  _isQuiet = isQuiet;
  bool res = _graphics->init();
  res = _status->init() && res;
  res = _content->init() && res;
  SDL_EnableKeyRepeat(100,SDL_DEFAULT_REPEAT_INTERVAL);
  return res;
}

bool PixBox::quit()
{
  bool res = _graphics->quit();
  res = _status->quit() && res;
  res = _content->quit() && res;
  return res;
}

void PixBox::mainLoop()
{
  //Event handler
  SDL_Event e;

  static unsigned int lastProcessedEventFrame = 0;

#define EVENT_THRESHOLD_IN_FRAMES 2

  _status->setCurrentGames(_content->currentSelection());
  _graphics->setVisibleGames(_status->getDisplayedGames(), _status->getGameIndexInPage());
  _graphics->flip();

  bool quitRequested(false);
  while(!quitRequested)
  {
    unsigned int frame = frameNumber();
    _graphics->setCurrentFrame(frame);
    bool update = true;
    //Handle events on queue
    bool escapeDown(false), coinDown(false);
    while( SDL_PollEvent( &e ) != 0 )
    {
      if( e.type == SDL_QUIT )
      {
        quitRequested = true;
      }
      else if( e.type == SDL_KEYDOWN )
      {
        switch( e.key.keysym.sym )
        {
          case SDLK_0:
            {
              quitRequested = true;
            }
            break;

          case SDLK_a: // Player 1
          case SDLK_f: // Player 2
          case SDLK_F4:  // Player 3
            {
              string device = this->_content->nextDevice();
              update = true;
              _status->setCurrentGames(_content->currentSelection());
              _graphics->setVisibleGames(_status->getDisplayedGames(), _status->getGameIndexInPage());
              _graphics->setDevice(device);
            }
            break;

          case SDLK_w: // Player 1
          case SDLK_t: // Player 2
          case SDLK_F8:  // Player 3
            {
              string device = this->_content->previousDevice();
              update = true;
              _status->setCurrentGames(_content->currentSelection());
              _graphics->setVisibleGames(_status->getDisplayedGames(), _status->getGameIndexInPage());
              _graphics->setDevice(device);
            }
            break;

          case SDLK_s: // Player 1
          case SDLK_g: // Player 2
            case SDLK_F5:  // Player 3
            {
              string type = this->_content->nextGameType();
              update = true;
              _status->setCurrentGames(_content->currentSelection());
              _graphics->setVisibleGames(_status->getDisplayedGames(), _status->getGameIndexInPage());
              _graphics->setType(type);
            }
            break;

          case SDLK_e: // Player 1
          case SDLK_y: // Player 2
            case SDLK_F9:  // Player 3
            {
              string type = this->_content->previousGameType();
              update = true;
              _status->setCurrentGames(_content->currentSelection());
              _graphics->setVisibleGames(_status->getDisplayedGames(), _status->getGameIndexInPage());
              _graphics->setType(type);
            }

          case SDLK_d: // Player 1
          case SDLK_h: // Player 2
            case SDLK_F6:  // Player 3
            {
              string multiplayer = this->_content->nextMultiplayer();
              update = true;
              _status->setCurrentGames(_content->currentSelection());
              _graphics->setVisibleGames(_status->getDisplayedGames(), _status->getGameIndexInPage());
              _graphics->setMulti(multiplayer);
            }
            break;

          case SDLK_z: // Player 1
          case SDLK_v: // Player 2
            case SDLK_F10:  // Player 3
            {
              string multiplayer = this->_content->previousMultiplayer();
              update = true;
              _status->setCurrentGames(_content->currentSelection());
              _graphics->setVisibleGames(_status->getDisplayedGames(), _status->getGameIndexInPage());
              _graphics->setMulti(multiplayer);
            }
            break;

          case SDLK_q: // Player 1
          case SDLK_b: // Player 2
            case SDLK_F7:  // Player 3
            {
              string family = this->_content->nextGameFamily();
              update = true;
              _status->setCurrentGames(_content->currentSelection());
              _graphics->setVisibleGames(_status->getDisplayedGames(), _status->getGameIndexInPage());
              _graphics->setFamily(family);
            }
            break;

          case SDLK_x: // Player 1
          case SDLK_r: // Player 2
            case SDLK_F11:  // Player 3
            {
              string family = this->_content->previousGameFamily();
              update = true;
              _status->setCurrentGames(_content->currentSelection());
              _graphics->setVisibleGames(_status->getDisplayedGames(), _status->getGameIndexInPage());
              _graphics->setFamily(family);
            }
            break;

          case SDLK_UP: // Player 1
          case SDLK_i: // Player 2
            case SDLK_u:  // Player 3
            {
              if(frameNumber() - lastProcessedEventFrame > EVENT_THRESHOLD_IN_FRAMES)
              {
                update=true;
                _status->previousGame();
                _graphics->setVisibleGames(_status->getDisplayedGames(), _status->getGameIndexInPage());
                lastProcessedEventFrame = frameNumber();
              }
            }
            break;

          case SDLK_DOWN: // Player 1
          case SDLK_k: // Player 2
            case SDLK_o:  // Player 3
            {
              if(frameNumber() - lastProcessedEventFrame > EVENT_THRESHOLD_IN_FRAMES)
              {
                update=true;
                _status->nextGame();
                _graphics->setVisibleGames(_status->getDisplayedGames(), _status->getGameIndexInPage());
                lastProcessedEventFrame = frameNumber();
              }
            }
            break;

          case SDLK_LEFT: // Player 1
          case SDLK_j: // Player 2
            case SDLK_c:  // Player 3
            {
              if(frameNumber() - lastProcessedEventFrame > EVENT_THRESHOLD_IN_FRAMES)
              {
                update=true;
                _status->previousPage();
                _graphics->setVisibleGames(_status->getDisplayedGames(), _status->getGameIndexInPage());
                lastProcessedEventFrame = frameNumber();
              }
            }
            break;

          case SDLK_RIGHT: // Player 1
          case SDLK_l: // Player 2
            case SDLK_n:  // Player 3
            {
              if(frameNumber() - lastProcessedEventFrame > EVENT_THRESHOLD_IN_FRAMES)
              {
                update=true;
                _status->nextPage();
                _graphics->setVisibleGames(_status->getDisplayedGames(), _status->getGameIndexInPage());
                lastProcessedEventFrame = frameNumber();
              }
            }
            break;

          case SDLK_1: // Player 1
          case SDLK_2: // Player 2
            case SDLK_3: // Player 3
            {
              update=true;
              _graphics->startCurrentGame();
            }
            break;

          case SDLK_ESCAPE:
#ifndef WIN32
          system("poweroff");
#endif
            break;

          case SDLK_5:
            {
              update=true;
//              _status->nextPage();
//              _graphics->setVisibleGames(_status->getDisplayedGames(), _status->getGameIndexInPage());
              _graphics->showKeyLayout();
            }
            break;

          default:
            break;
        }

//        const vector<Game*>& selection = _content->currentSelection();
//        for(vector<Game*>::const_iterator iter = selection.begin();
//            iter != selection.end();
//            ++iter)
//        {
//          cout << (*iter)->getName().c_str() << endl;
//        }
      } // end else if( e.type == SDL_KEYDOWN )
      else if( e.type == SDL_KEYUP )
      {
        switch( e.key.keysym.sym )
        {
          case SDLK_5:
            {
              update=true;
              //              _status->nextPage();
              //              _graphics->setVisibleGames(_status->getDisplayedGames(), _status->getGameIndexInPage());
              _graphics->hideKeyLayout();
            }
            break;

          default:
            break;
        }
      }


    } //  end while( SDL_PollEvent( &e ) != 0 )

#undef EVENT_THRESHOLD_IN_FRAMES

    if(update)
    {
      _graphics->flip();
    } // end if(update)

    SDL_Delay( 30 );
  } // end while(!quitRequested)
}

unsigned int PixBox::frameNumber() const
{
  return SDL_GetTicks()/40;
}

