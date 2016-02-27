#include "GraphicElements.h"
#include "Content.h"
#include <iostream>
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "PixBox.h"

using namespace std;

class SurfaceRect
{
  public:
    SDL_Surface* surface;
    SDL_Rect rect;
    int* xOffset;
    int* yOffset;

    SurfaceRect():
      surface(NULL)
    {
      clear();
    }

    ~SurfaceRect()
    {
      clear();
    }

    void clear()
    {
      if(surface != NULL)
      {
        SDL_FreeSurface(surface);
        surface = NULL;
      }
      rect.x = rect.y = rect.w = rect.h = 0;
      xOffset = NULL;
      yOffset = NULL;
    }

    void createSurface(int x, int y, int width, int height)
    {
      if(surface != NULL)
      {
        SDL_FreeSurface(surface);
        surface = NULL;
      }

      surface = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 16, 0, 0, 0, 0);
      rect.x = x;
      rect.y = y;
      rect.w = width;
      rect.h = height;
    }

    void setText(const char* text, TTF_Font* font, const SDL_Color& color, bool centered, int x, int y)
    {
      if(surface != NULL)
      {
        SDL_FreeSurface(surface);
        surface = NULL;
      }

      int width(0), height(0);
      TTF_SizeText(font, text, &width, &height);

      surface = TTF_RenderUTF8_Solid(font, text, color);
      if(centered)
      {
        rect.x = x-width/2;
        rect.y = y-height/2;
      }
      else
      {
        rect.x = x;
        rect.y = y;
      }
      rect.w = width;
      rect.h = height;
    }

    void setImage(const char* path, int x, int y, bool centered, SDL_Surface* screen)
    {
      clear();

      //Temporary storage for the image that's loaded
      SDL_Surface* loadedImage = IMG_Load(path);
      if(!loadedImage)
      {
        if(!PixBox::instance()->isQuiet())
          printf("IMG_Load: %s\n", IMG_GetError());
        return;
      }

      //Create an optimized image
      surface = SDL_DisplayFormat( loadedImage );
      //Free the old image
      SDL_FreeSurface( loadedImage );

      SDL_SetColorKey(surface, SDL_SRCCOLORKEY | SDL_RLEACCEL , SDL_MapRGB(screen->format, 255, 0, 255));

      if(centered)
      {
         rect.x = x - surface->w/2;
         rect.y = y - surface->h/2;
      }
      else
      {
         rect.x = x;
         rect.y = y;
      }
      rect.w = surface->w;
      rect.h = surface->h;
    }

    void blit(SDL_Surface* surf)
    {
      if(surf == NULL || surface == NULL)
        return;

      SDL_Rect actualRect(rect);

      if(xOffset != NULL && yOffset != NULL)
      {
        actualRect.x += *xOffset;
        actualRect.y += *yOffset;
      }

      if(rect.w != 0 && rect.h != 0)
        SDL_BlitSurface( surface, NULL, surf, &actualRect );
      else
        SDL_BlitSurface( surface, NULL, surf, NULL );
    }
};

class GraphicElements::Private
{
  public:

    Private();
    ~Private();

    void reset();
    void init();

    void bling();
    void bump();

    static inline unsigned int numGamesDisplayed() {return 20;}

    SDL_Surface* _screen;
    unsigned int _frame;
    struct
    {
        SurfaceRect _background;
        struct
        {
            TTF_Font* _titlesFont;
            TTF_Font* _entriesFont;
            SDL_Color _titleColor;
            SDL_Color _entriesColor;
            SDL_Color _entriesColor2;
        } _fonts;
        struct
        {
          SurfaceRect _title;
          vector<SurfaceRect*> _gameTitles;
//          SurfaceRect _cursor;
        } _gamesElements;
        struct
        {
            SurfaceRect _deviceTitle;
            SurfaceRect _deviceName;
            SurfaceRect _deviceBackground;
            unsigned int _deviceSelectedTargetFrame;
            int _XOffset;
            int _YOffset;
//            SurfaceRect _deviceBlinkBackground;
        } _deviceElements;
        struct
        {
            SurfaceRect _typeTitle;
            SurfaceRect _typeName;
            SurfaceRect _typeBackground;
            unsigned int _typeSelectedTargetFrame;
            int _XOffset;
            int _YOffset;
//            SurfaceRect _typeBlinkBackground;
        } _typeElements;
        struct
        {
            SurfaceRect _multiTitle;
            SurfaceRect _multiName;
            SurfaceRect _multiBackground;
            unsigned int _multiSelectedTargetFrame;
            int _XOffset;
            int _YOffset;
//            SurfaceRect _multiBlinkBackground;
        } _multiElements;
        struct
        {
            SurfaceRect _familyTitle;
            SurfaceRect _familyName;
            SurfaceRect _familyBackground;
            unsigned int _familySelectedTargetFrame;
            int _XOffset;
            int _YOffset;
//            SurfaceRect _familyBlinkBackground;
        } _familyElements;
        struct
        {
          SurfaceRect _gameTitle;
          SurfaceRect _gameName;
          SurfaceRect _deviceTitle;
          SurfaceRect _deviceName;
          SurfaceRect _multiTitle;
          SurfaceRect _multiName;
          SurfaceRect _typeTitle;
          SurfaceRect _typeName;
          SurfaceRect _artwork;
#ifndef BEFORE_MODIF
          bool _isPendingArtwork;
          string _pendingArtworkPath;
          unsigned int _targetFrame;
#endif

        } _mainElements;
        struct
        {
          SurfaceRect _cursor;
          int _XOffset;
          int _YOffset;
          unsigned int _targetFrame;
          int _targetYOffset;
        } _cursorElements;
        struct
        {
            SurfaceRect characterSheet;
            SDL_Rect sourceRect; // cycles
        } _character;
        SurfaceRect _keyLayout;
    } _elements;

    struct
    {
        int _gameLineYPadding;
        int _gameLineXOffset;
        int _gameLineYOffset;

        int _filterYPadding;
        int _filterXOffset;
        int _filterYOffset;
//        int _filterTextXOffset;
    }_parameters;

    Mix_Chunk * _bling;
    Mix_Chunk * _bump;

    string _currentCommand;
    string _currentSystem;

#ifndef BEFORE_MODIF
    void scheduleArtwork(const string& art);
#endif
};

#ifndef BEFORE_MODIF
void GraphicElements::Private::scheduleArtwork(const string &art)
{
  _elements._mainElements._artwork.clear();
  _elements._mainElements._isPendingArtwork = true;
  _elements._mainElements._pendingArtworkPath = art;
  _elements._mainElements._targetFrame = this->_frame + 25;
}
#endif

GraphicElements::Private::Private():
  _screen(NULL),
  _frame(0),
  _bling(NULL),
  _bump(NULL)
{
  _elements._fonts._titlesFont = NULL;
  _elements._fonts._entriesFont = NULL;
  _elements._fonts._titleColor.r = 33;
  _elements._fonts._titleColor.g = 65;
  _elements._fonts._titleColor.b = 198;

  _elements._fonts._entriesColor.r = 239;
  _elements._fonts._entriesColor.g = 177;
  _elements._fonts._entriesColor.b = 45;

  _elements._fonts._entriesColor2.r = 99;
  _elements._fonts._entriesColor2.g = 97;
  _elements._fonts._entriesColor2.b = 231;

  _parameters._gameLineYPadding = 20;
  _parameters._gameLineXOffset = 30;
  _parameters._gameLineYOffset = 140;

  _parameters._filterYPadding = 40;
  _parameters._filterXOffset = 500;
  _parameters._filterYOffset = 560;
}

GraphicElements::Private::~Private()
{
  reset();
}

void GraphicElements::Private::reset()
{
  _frame = 0;
  if(_screen != NULL)
  {
    SDL_FreeSurface(_screen);
    _screen = NULL;
  }

  if(_elements._fonts._titlesFont != NULL)
  {
    TTF_CloseFont(_elements._fonts._titlesFont);
    _elements._fonts._titlesFont = NULL;
  }

  if(_elements._fonts._entriesFont != NULL)
  {
    TTF_CloseFont(_elements._fonts._entriesFont);
    _elements._fonts._entriesFont = NULL;
  }

  _elements._background.clear();

  _elements._gamesElements._title.clear();
  for(vector<SurfaceRect*>::iterator iter = _elements._gamesElements._gameTitles.begin();
      iter != _elements._gamesElements._gameTitles.end();
      ++iter)
  {
    delete (*iter);
  }
  _elements._gamesElements._gameTitles.clear();
//  _elements._gamesElements._cursor.clear();

  _elements._deviceElements._deviceTitle.clear();
  _elements._deviceElements._deviceName.clear();
  _elements._deviceElements._deviceBackground.clear();
//  _elements._deviceElements._deviceBlinkBackground.clear();
  _elements._deviceElements._deviceSelectedTargetFrame = 0;

  _elements._typeElements._typeTitle.clear();
  _elements._typeElements._typeName.clear();
  _elements._typeElements._typeBackground.clear();
//  _elements._typeElements._typeBlinkBackground.clear();
  _elements._typeElements._typeSelectedTargetFrame = 0;

  _elements._multiElements._multiTitle.clear();
  _elements._multiElements._multiName.clear();
  _elements._multiElements._multiBackground.clear();
//  _elements._multiElements._multiBlinkBackground.clear();
  _elements._multiElements._multiSelectedTargetFrame = 0;

  _elements._familyElements._familyTitle.clear();
  _elements._familyElements._familyName.clear();
  _elements._familyElements._familyBackground.clear();
//  _elements._familyElements._familyBlinkBackground.clear();
  _elements._familyElements._familySelectedTargetFrame = 0;

  _elements._mainElements._gameTitle.clear();
  _elements._mainElements._gameName.clear();
  _elements._mainElements._deviceTitle.clear();
  _elements._mainElements._deviceName.clear();
  _elements._mainElements._multiTitle.clear();
  _elements._mainElements._multiName.clear();
  _elements._mainElements._typeTitle.clear();
  _elements._mainElements._typeName.clear();
  _elements._mainElements._artwork.clear();


  _elements._background.clear();
  _elements._cursorElements._cursor.clear();

  _elements._character.characterSheet.clear();
  _elements._character.sourceRect.x = 0;
  _elements._character.sourceRect.y = 0;
  _elements._character.sourceRect.w = 0;
  _elements._character.sourceRect.h = 0;


}

void GraphicElements::Private::init()
{
  _elements._background.setImage(RESOURCE_PATH("pixbox-interface3.png").c_str(), 0, 0, false, _screen);//.createSurface(0, 0, 1280, 720);

  _elements._fonts._titlesFont = TTF_OpenFont(RESOURCE_PATH("PressStart2P.ttf").c_str(), 16);
  _elements._fonts._entriesFont = TTF_OpenFont(RESOURCE_PATH("PressStart2P.ttf").c_str(), 16);

//  _elements._gamesElements._title.surface = TTF_RenderUTF8_Solid(_elements._fonts._titlesFont, "JEUX", _elements._fonts._titleColor);
//  _elements._gamesElements._title.setRect(0, 0, 200, 50);
//  _elements._gamesElements._title.setText("JEUX", _elements._fonts._titlesFont, _elements._fonts._titleColor, true, 50, 30);
  _elements._gamesElements._gameTitles.clear();
  for(unsigned int ii = 0; ii < numGamesDisplayed(); ++ii)
  {
    SurfaceRect* surf = new SurfaceRect;
    surf->setText("", _elements._fonts._entriesFont, _elements._fonts._entriesColor, false, _parameters._gameLineXOffset, _parameters._gameLineYOffset + ii*_parameters._gameLineYPadding);
    _elements._gamesElements._gameTitles.push_back(surf);
  }

//  _elements._deviceElements._deviceTitle.surface = TTF_RenderUTF8_Solid(_elements._fonts._titlesFont, "Machine", _elements._fonts._titleColor);
//  _elements._deviceElements._deviceTitle.setRect(200, 0, 200, 50);
  _elements._deviceElements._deviceTitle.setText("Machine", _elements._fonts._titlesFont, _elements._fonts._titleColor, false, _parameters._filterXOffset, _parameters._filterYOffset);
  _elements._deviceElements._deviceTitle.xOffset = &_elements._deviceElements._XOffset;
  _elements._deviceElements._deviceTitle.yOffset = &_elements._deviceElements._YOffset;
//  _elements._deviceElements._deviceName.surface = TTF_RenderUTF8_Solid(_elements._fonts._entriesFont, "Tout", _elements._fonts._entriesColor);
//  _elements._deviceElements._deviceName.setRect(200, 50, 200, 50);
  _elements._deviceElements._deviceName.setText("Tout", _elements._fonts._entriesFont, _elements._fonts._entriesColor2, false, _parameters._filterXOffset + 150, _parameters._filterYOffset);
  _elements._deviceElements._deviceName.xOffset = &_elements._deviceElements._XOffset;
  _elements._deviceElements._deviceName.yOffset = &_elements._deviceElements._YOffset;
//  _elements._deviceElements._deviceBackground.createSurface(200, 0, 200, 50);
//  _elements._deviceElements._deviceBackground.setImage("C:/temp/plop.png", 500, 640, true, _screen);//createSurface(200, 0, 200, 50);
  _elements._deviceElements._deviceBackground.xOffset = &_elements._deviceElements._XOffset;
  _elements._deviceElements._deviceBackground.yOffset = &_elements._deviceElements._YOffset;
//  _elements._deviceElements._deviceBlinkBackground.createSurface(200, 0, 200, 50);

//  _elements._typeElements._typeTitle.surface = TTF_RenderUTF8_Solid(_elements._fonts._titlesFont, "Type", _elements._fonts._titleColor);
//  _elements._typeElements._typeTitle.setRect(300, 0, 200, 50);
  _elements._typeElements._typeTitle.setText("Type", _elements._fonts._titlesFont, _elements._fonts._titleColor, false, _parameters._filterXOffset, _parameters._filterYOffset + _parameters._filterYPadding);
  _elements._typeElements._typeTitle.xOffset = &_elements._typeElements._XOffset;
  _elements._typeElements._typeTitle.yOffset = &_elements._typeElements._YOffset;
//  _elements._typeElements._typeName.surface = TTF_RenderUTF8_Solid(_elements._fonts._entriesFont, "Tout", _elements._fonts._entriesColor);
//  _elements._typeElements._typeName.setRect(300, 50, 200, 50);
  _elements._typeElements._typeName.setText("Tout", _elements._fonts._entriesFont, _elements._fonts._entriesColor2, false, _parameters._filterXOffset + 150, _parameters._filterYOffset + _parameters._filterYPadding);
  _elements._typeElements._typeName.xOffset = &_elements._typeElements._XOffset;
  _elements._typeElements._typeName.yOffset = &_elements._typeElements._YOffset;
//  _elements._typeElements._typeBackground.setImage("C:/temp/plop.png", 700, 640, true, _screen);//createSurface(200, 0, 200, 50);
  _elements._typeElements._typeBackground.xOffset = &_elements._typeElements._XOffset;
  _elements._typeElements._typeBackground.yOffset = &_elements._typeElements._YOffset;
//  _elements._typeElements._typeBlinkBackground.createSurface(200, 0, 200, 50);

//  _elements._multiElements._multiTitle.surface = TTF_RenderUTF8_Solid(_elements._fonts._titlesFont, "Multi ?", _elements._fonts._titleColor);
//  _elements._multiElements._multiTitle.setRect(400, 0, 200, 50);
  _elements._multiElements._multiTitle.setText("Multi ?", _elements._fonts._titlesFont, _elements._fonts._titleColor, false, _parameters._filterXOffset, _parameters._filterYOffset + 2 * _parameters._filterYPadding);
  _elements._multiElements._multiTitle.xOffset = &_elements._multiElements._XOffset;
  _elements._multiElements._multiTitle.yOffset = &_elements._multiElements._YOffset;
//  _elements._multiElements._multiName.surface = TTF_RenderUTF8_Solid(_elements._fonts._entriesFont, "Tout", _elements._fonts._entriesColor);
//  _elements._multiElements._multiName.setRect(400, 50, 200, 50);
  _elements._multiElements._multiName.setText("1P/2P", _elements._fonts._entriesFont, _elements._fonts._entriesColor2, false, _parameters._filterXOffset + 150, _parameters._filterYOffset + 2 * _parameters._filterYPadding);
  _elements._multiElements._multiName.xOffset = &_elements._multiElements._XOffset;
  _elements._multiElements._multiName.yOffset = &_elements._multiElements._YOffset;
//  _elements._multiElements._multiBackground.setImage("C:/temp/plop.png", 900, 640, true, _screen);//createSurface(200, 0, 200, 50);
  _elements._multiElements._multiBackground.xOffset = &_elements._multiElements._XOffset;
  _elements._multiElements._multiBackground.yOffset = &_elements._multiElements._YOffset;
//  _elements._multiElements._multiBlinkBackground.createSurface(200, 0, 200, 50);

//  _elements._familyElements._familyTitle.surface = TTF_RenderUTF8_Solid(_elements._fonts._titlesFont, "Famille", _elements._fonts._titleColor);
//  _elements._familyElements._familyTitle.setRect(500, 0, 200, 50);
  _elements._familyElements._familyTitle.setText("Famille", _elements._fonts._titlesFont, _elements._fonts._titleColor, false, _parameters._filterXOffset, _parameters._filterYOffset + 3 * _parameters._filterYPadding);
  _elements._familyElements._familyTitle.xOffset = &_elements._familyElements._XOffset;
  _elements._familyElements._familyTitle.yOffset = &_elements._familyElements._YOffset;
//  _elements._familyElements._familyName.surface = TTF_RenderUTF8_Solid(_elements._fonts._entriesFont, "Tout", _elements._fonts._entriesColor);
//  _elements._familyElements._familyName.setRect(500, 50, 200, 50);
  _elements._familyElements._familyName.setText("Tout", _elements._fonts._entriesFont, _elements._fonts._entriesColor2, false, _parameters._filterXOffset + 150, _parameters._filterYOffset + 3 * _parameters._filterYPadding);
  _elements._familyElements._familyName.xOffset = &_elements._familyElements._XOffset;
  _elements._familyElements._familyName.yOffset = &_elements._familyElements._YOffset;
//  _elements._familyElements._familyBackground.setImage("C:/temp/plop.png", 1100, 640, true, _screen);//createSurface(200, 0, 200, 50);
  _elements._familyElements._familyBackground.xOffset = &_elements._familyElements._XOffset;
  _elements._familyElements._familyBackground.yOffset = &_elements._familyElements._YOffset;
//  _elements._familyElements._familyBlinkBackground.createSurface(200, 0, 200, 50);

  _elements._mainElements._gameTitle.setText("Jeu", _elements._fonts._titlesFont, _elements._fonts._titleColor, true, 800, 50);
  _elements._mainElements._deviceTitle.setText("Machine", _elements._fonts._titlesFont, _elements._fonts._titleColor, false, 400, 150);
  _elements._mainElements._typeTitle.setText("Type", _elements._fonts._titlesFont, _elements._fonts._titleColor, false, 400, 200);
  _elements._mainElements._multiTitle.setText("Multijoueur ?", _elements._fonts._titlesFont, _elements._fonts._titleColor, false, 400, 250);

  _elements._character.characterSheet.surface;
//  optimisedLoadPng("C:/temp/items-pixbox.png", &_elements._character.characterSheet.surface);
//  SDL_SetColorKey(_elements._character.characterSheet.surface, SDL_SRCCOLORKEY | SDL_RLEACCEL , SDL_MapRGB(_screen->format, 255, 0, 255));
  _elements._character.characterSheet.setImage(RESOURCE_PATH("items-pixbox.png").c_str(), 0, 500, false, _screen);
//  _elements._character.characterSheet.rect.x = 600;
//  _elements._character.characterSheet.rect.y = 0;
//  _elements._character.characterSheet.rect.w = 96;
//  _elements._character.characterSheet.rect.h = 48;
  _elements._character.sourceRect.x = 0;
  _elements._character.sourceRect.y = (rand() % 10) * 48 + 1;
  _elements._character.sourceRect.w = 96;
  _elements._character.sourceRect.h = 48;

  _elements._cursorElements._cursor.setImage(RESOURCE_PATH("pixbox-selection.png").c_str(), _parameters._gameLineXOffset-10, _parameters._gameLineYOffset, false, _screen);
  _elements._cursorElements._cursor.xOffset = &_elements._cursorElements._XOffset;
  _elements._cursorElements._cursor.yOffset = &_elements._cursorElements._YOffset;
  _elements._cursorElements._targetFrame = 0;
  _elements._cursorElements._targetYOffset = _parameters._gameLineYOffset;
  _elements._cursorElements._XOffset = 0;
  _elements._cursorElements._YOffset = 0;

  _bling = Mix_LoadWAV( RESOURCE_PATH("smb_coin.wav").c_str());
  _bump = Mix_LoadWAV( RESOURCE_PATH("smb_bump.wav").c_str());

}

void GraphicElements::Private::bling()
{
  if(_bling != NULL)
    Mix_PlayChannel( -1, _bling, 0 );
}

void GraphicElements::Private::bump()
{
  if(_bump != NULL)
    Mix_PlayChannel( -1, _bump, 0 );
}

GraphicElements::GraphicElements():
  d(new Private)/*,
  _screen(NULL),
  _background(NULL),
  _gameNames(),
  _deviceBlinkFrame(0),
  _typeBlinkFrame(0),
  _multiBlinkFrame(0),
  _familyBlinkFrame(0)*/
{
//  _fonts.gameIndexFont = NULL;
//  _gameColor.r = 255;
//  _gameColor.g = 255;
//  _gameColor.b = 0;

//  for(unsigned int ii = 0; ii < 12; ++ii)
//  {
//    SDL_Rect* rect = new SDL_Rect;
//    rect->x = 30;
//    rect->y = ii * 20 + 10;
//    rect->h = 20;
//    rect->w = 200;
//    _gameNamesRect.push_back(rect);
//  }

//  _prevCursorY = 10;
}

GraphicElements::~GraphicElements()
{
  delete d;
//  delete d;
//  reset();

//  for(vector<SDL_Rect*>::iterator iter = _gameNamesRect.begin();
//      iter != _gameNamesRect.end();
//      ++iter)
//  {
//    if(*iter != NULL)
//      delete *iter;
//  }
//  _gameNamesRect.clear();

}

bool GraphicElements::optimisedLoadPng(const char *path, SDL_Surface **target)
{
  if(*target!=NULL)
  {
    SDL_FreeSurface(*target);
    *target = NULL;
  }

  //Temporary storage for the image that's loaded
  SDL_Surface* loadedImage = IMG_Load(path);
  if(!loadedImage)
  {
    printf("IMG_Load: %s\n", IMG_GetError());
    return false;
  }

  //Create an optimized image
  *target = SDL_DisplayFormat( loadedImage );
  //Free the old image
  SDL_FreeSurface( loadedImage );

  return false;
}


bool GraphicElements::init()
{
  d->reset();

  unsigned int subsystems = SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO;

  if (SDL_Init(subsystems) != 0)
  {
    if(!PixBox::instance()->isQuiet())
      fprintf(stderr,
              "\nUnable to initialize SDL:  %s\n",
              SDL_GetError()
              );
    return false;
  }

  // init screen
  /*
  SDL_SWSURFACE	Create the video surface in system memory
  SDL_HWSURFACE	Create the video surface in video memory
  SDL_ASYNCBLIT	Enables the use of asynchronous updates of the display surface. This will usually slow down blitting on single CPU machines, but may provide a speed increase on SMP systems.
  SDL_ANYFORMAT	Normally, if a video surface of the requested bits-per-pixel (bpp) is not available, SDL will emulate one with a shadow surface. Passing SDL_ANYFORMAT prevents this and causes SDL to use the video surface, regardless of its pixel depth.
  SDL_HWPALETTE	Give SDL exclusive palette access. Without this flag you may not always get the the colors you request with SDL_SetColors or SDL_SetPalette.
  SDL_DOUBLEBUF	Enable hardware double buffering; only valid with SDL_HWSURFACE. Calling SDL_Flip will flip the buffers and update the screen. All drawing will take place on the surface that is not displayed at the moment. If double buffering could not be enabled then SDL_Flip will just perform a SDL_UpdateRect on the entire screen.
  SDL_FULLSCREEN	SDL will attempt to use a fullscreen mode. If a hardware resolution change is not possible (for whatever reason), the next higher resolution will be used and the display window centered on a black background.
  SDL_OPENGL	Create an OpenGL rendering context. You should have previously set OpenGL video attributes with SDL_GL_SetAttribute.
  SDL_OPENGLBLIT	Create an OpenGL rendering context, like above, but allow normal blitting operations. The screen (2D) surface may have an alpha channel, and SDL_UpdateRects must be used for updating changes to the screen surface. NOTE: This option is kept for compatibility only, and is not recommended for new code.
  SDL_RESIZABLE	Create a resizable window. When the window is resized by the user a SDL_VIDEORESIZE event is generated and SDL_SetVideoMode can be called again with the new size.
  SDL_NOFRAME	If possible, SDL_NOFRAME causes SDL to create a window with no title bar or frame decoration. Fullscreen modes automatically have this flag set.
  */
  d->_screen = SDL_SetVideoMode(1280, 720, 16, SDL_SWSURFACE /*SDL_HWSURFACE | SDL_DOUBLEBUF*/);
  if ( d->_screen == NULL )
  {
    if(!PixBox::instance()->isQuiet())
      fprintf(stderr, "Couldn't set video mode: %s\n",
              SDL_GetError());
    return false;
  }

  // Init font system

  if(TTF_Init()==-1)
  {
    if(!PixBox::instance()->isQuiet())
      printf("TTF_Init: %s\n", TTF_GetError());
    return false;
  }

  // Init audio

  if( Mix_OpenAudio( MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 1, 2048 ) < 0 )
  {
    if(!PixBox::instance()->isQuiet())
      printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
    return false;
  }

  d->init();

//  _spriteSheet = NULL;
//  optimisedLoadPng("C:/temp/items-pixbox.png", &_spriteSheet);
//  SDL_SetColorKey(_spriteSheet, SDL_SRCCOLORKEY | SDL_RLEACCEL , SDL_MapRGB(_screen->format, 255, 0, 255));

//  _background= SDL_CreateRGBSurface
//        (SDL_SWSURFACE, screenWidth(), screenHeight(), screenBpp(),
//        0, 0, 0, 0);

//  _deviceRect= SDL_CreateRGBSurface
//        (SDL_SWSURFACE, 50, 50, screenBpp(),
//        200, 200, 200, 0);
//  _typeRect= SDL_CreateRGBSurface
//        (SDL_SWSURFACE, 50, 50, screenBpp(),
//        200, 200, 200, 0);
//  _multiRect= SDL_CreateRGBSurface
//        (SDL_SWSURFACE, 50, 50, screenBpp(),
//        200, 200, 200, 0);
//  _familyRect= SDL_CreateRGBSurface
//        (SDL_SWSURFACE, 50, 50, screenBpp(),
//        200, 200, 200, 0);

//  _fonts.gameIndexFont=TTF_OpenFont("C:\\temp\\pixbox\\PressStart2P.ttf", 16);
//  if(!_fonts.gameIndexFont)
//  {
//    printf("TTF_OpenFont: %s\n", TTF_GetError());
//    // handle error
//    return false;
//  }

//  _cursor = TTF_RenderUTF8_Solid(_fonts.gameIndexFont, "*", _gameColor);

  return true;
}

bool GraphicElements::quit()
{
  TTF_Quit();
  SDL_Quit();
  return true;
}

void GraphicElements::setCurrentFrame(unsigned int frame)
{
  // Move whatever there is to move

  if(d->_elements._deviceElements._deviceSelectedTargetFrame > frame)
  {
    int delta = frame - d->_elements._deviceElements._deviceSelectedTargetFrame;
    d->_elements._deviceElements._XOffset = 0;
    d->_elements._deviceElements._YOffset = delta * 2;
  }
  else
  {
    d->_elements._deviceElements._XOffset = 0;
    d->_elements._deviceElements._YOffset = 0;
  }

  if(d->_elements._typeElements._typeSelectedTargetFrame > frame)
  {
    int delta = frame - d->_elements._typeElements._typeSelectedTargetFrame;
    d->_elements._typeElements._XOffset = 0;
    d->_elements._typeElements._YOffset = delta * 2;
  }
  else
  {
    d->_elements._typeElements._XOffset = 0;
    d->_elements._typeElements._YOffset = 0;
  }

  if(d->_elements._multiElements._multiSelectedTargetFrame > frame)
  {
    int delta = frame - d->_elements._multiElements._multiSelectedTargetFrame;
    d->_elements._multiElements._XOffset = 0;
    d->_elements._multiElements._YOffset = delta * 2;
  }
  else
  {
    d->_elements._multiElements._XOffset = 0;
    d->_elements._multiElements._YOffset = 0;
  }

  if(d->_elements._familyElements._familySelectedTargetFrame > frame)
  {
    int delta = frame - d->_elements._familyElements._familySelectedTargetFrame;
    d->_elements._familyElements._XOffset = 0;
    d->_elements._familyElements._YOffset = delta * 2;
  }
  else
  {
    d->_elements._familyElements._XOffset = 0;
    d->_elements._familyElements._YOffset = 0;
  }

#ifndef BEFORE_MODIF
  if(d->_elements._mainElements._isPendingArtwork && d->_elements._mainElements._targetFrame <= frame)
  {
    d->_elements._mainElements._isPendingArtwork = false;
    d->_elements._mainElements._artwork.setImage(d->_elements._mainElements._pendingArtworkPath.c_str(), 1050, 250, true, d->_screen);
  }
#endif

  // moving character
  {
    int characterStep = (frame / 3) % 4;
    d->_elements._character.sourceRect.x = characterStep * 96;

    d->_elements._character.characterSheet.rect.x += 4;

    if(d->_elements._character.characterSheet.rect.x > 1280)
    {
      // new character moving by
      d->_elements._character.characterSheet.rect.x = -96;
      d->_elements._character.sourceRect.y = (rand() % 10) * 48 + 1;
    }
  }

  // Move cursor
  if(frame < d->_elements._cursorElements._targetFrame)
  {
    d->_elements._cursorElements._YOffset += (d->_elements._cursorElements._targetYOffset - (double) d->_elements._cursorElements._YOffset)/2.;
  }
  else if(d->_elements._cursorElements._YOffset != d->_elements._cursorElements._targetYOffset)
  {
    d->_elements._cursorElements._YOffset = d->_elements._cursorElements._targetYOffset;
  }


  d->_frame = frame;
}

void GraphicElements::setVisibleGames(const std::vector<Game*> &games, unsigned int cursorPosition)
{
  hideKeyLayout();

  for(int ii = 0; ii < d->_elements._gamesElements._gameTitles.size(); ++ii)
  {
    if(ii < games.size())
    {
      d->_elements._gamesElements._gameTitles[ii]->setText(games[ii]->getShortName().c_str(), d->_elements._fonts._entriesFont, d->_elements._fonts._entriesColor, false, d->_parameters._gameLineXOffset, d->_parameters._gameLineYOffset + ii*d->_parameters._gameLineYPadding);
    }
    else
    {
      d->_elements._gamesElements._gameTitles[ii]->setText("", d->_elements._fonts._entriesFont, d->_elements._fonts._entriesColor, false, d->_parameters._gameLineXOffset, d->_parameters._gameLineYOffset + ii*d->_parameters._gameLineYPadding);
    }
  }
//  for(list<SDL_Surface*>::iterator iter = _gameNames.begin();

  d->_elements._cursorElements._targetFrame = d->_frame + 3;
  d->_elements._cursorElements._targetYOffset = cursorPosition * d->_parameters._gameLineYPadding-3;

  d->_elements._mainElements._artwork.clear();
  if(cursorPosition < games.size())
  {
    Game* game = games[cursorPosition];
    d->_elements._mainElements._gameName.setText(game->getName().c_str(), d->_elements._fonts._entriesFont, d->_elements._fonts._entriesColor2, true, 800, 80);
    d->_elements._mainElements._deviceName.setText(game->getDevice().c_str(), d->_elements._fonts._entriesFont, d->_elements._fonts._entriesColor2, false, 550, 150);
    string type;
    const list<string>& types = game->getGameTypes();
    for(list<string>::const_iterator iter = types.begin();
        iter != types.end();
        ++iter)
    {
      if(iter != types.begin())
        type.append(", ");
      type.append(*iter);
    }
    if(type.empty())
      type = "-";
    d->_elements._mainElements._typeName.setText(type.c_str(), d->_elements._fonts._entriesFont, d->_elements._fonts._entriesColor2, false, 500, 200);
    d->_elements._mainElements._multiName.setText(game->isMultiplayer() ? "1P/2P" : "1P", d->_elements._fonts._entriesFont, d->_elements._fonts._entriesColor2, false, 650, 250);
    if(!game->getPicturePath().empty())
    {
#ifdef BEFORE_MODIF
      d->_elements._mainElements._artwork.setImage(game->getPicturePath().c_str(), 1050, 250, true, d->_screen);
#else
      d->scheduleArtwork(game->getPicturePath());
#endif
    }
    d->_currentCommand = game->getCommandLine();
    d->_currentSystem = game->getDevice();
  }
  else
  {
    d->_elements._mainElements._gameName.setText("-", d->_elements._fonts._entriesFont, d->_elements._fonts._entriesColor2, true, 500, 120);
    d->_elements._mainElements._deviceName.setText("-", d->_elements._fonts._entriesFont, d->_elements._fonts._entriesColor2, true, 500, 220);
    d->_elements._mainElements._typeName.setText("-", d->_elements._fonts._entriesFont, d->_elements._fonts._entriesColor2, true, 500, 320);
    d->_elements._mainElements._multiName.setText("-", d->_elements._fonts._entriesFont, d->_elements._fonts._entriesColor2, true, 500, 420);
    d->_currentCommand.clear();
    d->_currentSystem.clear();
  }
}

void GraphicElements::startCurrentGame()
{
  if(d->_currentCommand.empty())
    return;

  hideKeyLayout();

  SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);
  d->_screen = NULL;
  Mix_FreeChunk(d->_bling);
  Mix_FreeChunk(d->_bump);

//  printf("NO_MENU=1 /media/BBB/sources/picodrive/picodrive/PicoDrive /media/BBB/old/roms/gen_usa/Aladdin\\ \\(USA\\).md\n");
  if(!PixBox::instance()->isQuiet())
  {
    printf(d->_currentCommand.c_str());
    system(d->_currentCommand.c_str());
  }
  else
  {
    system((d->_currentCommand + " 2>&1 > /dev/null").c_str());
  }
  SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);
  d->_screen = SDL_SetVideoMode(1280, 720, 16, SDL_SWSURFACE /*SDL_HWSURFACE | SDL_DOUBLEBUF*/);
  Mix_OpenAudio( MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 1, 2048 );
  d->_bling = Mix_LoadWAV( RESOURCE_PATH("smb_coin.wav").c_str());
  d->_bump = Mix_LoadWAV( RESOURCE_PATH("smb_bump.wav").c_str());
}

void GraphicElements::showKeyLayout()
{
  d->_elements._keyLayout.setImage(RESOURCE_PATH(d->_currentSystem+".png").c_str(), 640, 360, true, d->_screen);
}

void GraphicElements::hideKeyLayout()
{
  d->_elements._keyLayout.clear();
}

void GraphicElements::flip()
{
  if(d->_screen == NULL)
    return;

  d->_elements._background.blit(d->_screen);
  d->_elements._deviceElements._deviceBackground.blit(d->_screen);
  d->_elements._typeElements._typeBackground.blit(d->_screen);
  d->_elements._multiElements._multiBackground.blit(d->_screen);
  d->_elements._familyElements._familyBackground.blit(d->_screen);

  d->_elements._cursorElements._cursor.blit(d->_screen);

  SDL_BlitSurface( d->_elements._character.characterSheet.surface, &d->_elements._character.sourceRect, d->_screen, &d->_elements._character.characterSheet.rect );

  d->_elements._gamesElements._title.blit(d->_screen);
  for(unsigned int ii = 0; ii < d->_elements._gamesElements._gameTitles.size(); ++ii)
  {
    d->_elements._gamesElements._gameTitles[ii]->blit(d->_screen);
  }

  d->_elements._deviceElements._deviceTitle.blit(d->_screen);
  d->_elements._typeElements._typeTitle.blit(d->_screen);
  d->_elements._multiElements._multiTitle.blit(d->_screen);
  d->_elements._familyElements._familyTitle.blit(d->_screen);

  d->_elements._deviceElements._deviceName.blit(d->_screen);
  d->_elements._typeElements._typeName.blit(d->_screen);
  d->_elements._multiElements._multiName.blit(d->_screen);
  d->_elements._familyElements._familyName.blit(d->_screen);

  d->_elements._mainElements._gameTitle.blit(d->_screen);
  d->_elements._mainElements._gameName.blit(d->_screen);
  d->_elements._mainElements._deviceTitle.blit(d->_screen);
  d->_elements._mainElements._deviceName.blit(d->_screen);
  d->_elements._mainElements._typeTitle.blit(d->_screen);
  d->_elements._mainElements._typeName.blit(d->_screen);
  d->_elements._mainElements._multiTitle.blit(d->_screen);
  d->_elements._mainElements._multiName.blit(d->_screen);

  d->_elements._mainElements._artwork.blit(d->_screen);

  d->_elements._keyLayout.blit(d->_screen);


  SDL_Flip( d->_screen );
}

void GraphicElements::setDevice(const string& device)
{
  hideKeyLayout();
  string actualDevice = device.empty() ? "Tout" : device;
  d->_elements._deviceElements._deviceName.setText(actualDevice.c_str(), d->_elements._fonts._entriesFont,
                                                   d->_elements._fonts._entriesColor2,
                                                   false,
                                                   d->_parameters._filterXOffset + 150, d->_parameters._filterYOffset);

  d->_elements._deviceElements._deviceSelectedTargetFrame = d->_frame + 5;
  d->bump();
  if(device.empty())
    d->bling();
}

void GraphicElements::setType(const string& type)
{
  hideKeyLayout();
  string actualType = type.empty() ? "Tout" : type;
  d->_elements._typeElements._typeName.setText(actualType.c_str(), d->_elements._fonts._entriesFont,
                                                   d->_elements._fonts._entriesColor2,
                                                   false,
                                                   d->_parameters._filterXOffset + 150, d->_parameters._filterYOffset + d->_parameters._filterYPadding);
  d->_elements._typeElements._typeSelectedTargetFrame = d->_frame + 5;
  d->bump();
  if(type.empty())
    d->bling();
}

void GraphicElements::setMulti(const string& multi)
{
  hideKeyLayout();
  string actualMulti = multi.empty() ? "1P/2P" : multi;
  d->_elements._multiElements._multiName.setText(actualMulti.c_str(), d->_elements._fonts._entriesFont,
                                                   d->_elements._fonts._entriesColor2,
                                                   false,
                                                   d->_parameters._filterXOffset + 150, d->_parameters._filterYOffset + 2 * d->_parameters._filterYPadding);
  d->_elements._multiElements._multiSelectedTargetFrame = d->_frame + 5;
  d->bump();
  if(actualMulti == "1P/2P" )
    d->bling();
}

void GraphicElements::setFamily(const string& family)
{
  hideKeyLayout();
  string actualFamily = family.empty() ? "Tout" : family;
  d->_elements._familyElements._familyName.setText(actualFamily.c_str(), d->_elements._fonts._entriesFont,
                                                   d->_elements._fonts._entriesColor2,
                                                   false,
                                                   d->_parameters._filterXOffset + 150, d->_parameters._filterYOffset + 3 * d->_parameters._filterYPadding);
  d->_elements._familyElements._familySelectedTargetFrame = d->_frame + 5;
  d->bump();
  if(family.empty())
    d->bling();
}
