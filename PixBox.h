#ifndef PIXBOX_H
#define PIXBOX_H

#include "Content.h"
#include "GraphicElements.h"
#include "GraphicStatus.h"

class PixBox
{
  private:
    PixBox();
    static PixBox* _instance;

    GraphicElements* _graphics;
    GraphicStatus* _status;
    Content* _content;

  public:
    static PixBox* instance();
    ~PixBox();

    bool init(bool isQuiet);

    bool quit();

    void mainLoop();

    bool isQuiet() const
    {
      return _isQuiet;
    }

  private:
    unsigned int frameNumber() const;
    bool _isQuiet;
};

#endif // PIXBOX_H
