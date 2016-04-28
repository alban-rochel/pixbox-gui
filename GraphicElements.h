#ifndef GRAPHICELEMENTS_H
#define GRAPHICELEMENTS_H

#include "defines.h"
#include "SDL.h"   /* All SDL App's need this */
#include "SDL_ttf.h"
#include <list>
#include <vector>
#include <string>

class Game;

class GraphicElements
{
  public:
    GraphicElements();
    ~GraphicElements();

    bool init();

    bool quit();

    void setCurrentFrame(unsigned int frame);
    void setVisibleGames(const std::vector<Game*>& games, unsigned int cursorPosition);
    void startCurrentGame();
    void showKeyLayout();
    void hideKeyLayout();

    void setDevice(const std::string& device);
    void setType(const std::string& type);
    void setMulti(const std::string& multi);
    void setFamily(const std::string& family);

//    void blinkDevice();
//    void blinkType();
//    void blinkMulti();
//    void blinkFamily();

    void flip();

    bool static optimisedLoadPng(const char *path, SDL_Surface **target);

  protected:
    int screenWidth() const {return PIXBOX_WIDTH;}
    int screenHeight() const {return PIXBOX_HEIGHT;}
    int screenBpp() const {return  16;}

  private:
    class Private;
    Private* d;

};

#endif // GRAPHICELEMENTS_H
