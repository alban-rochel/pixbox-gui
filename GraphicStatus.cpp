#include "GraphicStatus.h"

#define NUM_GAMES 20

#include "Content.h"
#include <vector>
#include <iostream>

using namespace std;

class GraphicStatus::Private
{
  public:
    Private();
    ~Private();

    vector<Game*> _currentGames;
    vector<Game*> _gamesInCurrentPage;
    unsigned int _currentGameIndex;

    void offsetCurrentGameIndex(int offset);
    void recomputeCurrentPage();
};

GraphicStatus::Private::Private():
  _currentGames(),
  _gamesInCurrentPage(),
  _currentGameIndex(0)
{

}

GraphicStatus::Private::~Private()
{

}

void GraphicStatus::Private::offsetCurrentGameIndex(int offset)
{
  if(((int) _currentGameIndex) + offset <= 0)
   _currentGameIndex = 0;
  else
  {
    _currentGameIndex += offset;
    if(_currentGameIndex >= _currentGames.size())
      _currentGameIndex = _currentGames.size() - 1;
  }

  recomputeCurrentPage();
}

void GraphicStatus::Private::recomputeCurrentPage()
{
  _gamesInCurrentPage.clear();

  unsigned int pageIndex = _currentGameIndex / NUM_GAMES;

  for(unsigned int ii = pageIndex * NUM_GAMES;
      ii < (pageIndex+1) * NUM_GAMES && ii < _currentGames.size();
      ++ii)
  {
    _gamesInCurrentPage.push_back(_currentGames[ii]);
  }
}

GraphicStatus::GraphicStatus():
  d(new Private)
{
}

GraphicStatus::~GraphicStatus()
{
  delete d;
}

bool GraphicStatus::init()
{
  return true;
}

bool GraphicStatus::quit()
{
  return true;
}

void GraphicStatus::setCurrentGames(const vector<Game*>& currentSelection)
{
  unsigned int currentPosition(0);
  if(!d->_currentGames.empty() && d->_currentGameIndex < d->_currentGames.size())
  {
    currentPosition = d->_currentGames[d->_currentGameIndex]->getPosition();
  }

  d->_currentGames.clear();
  d->_gamesInCurrentPage.clear();

  d->_currentGames = currentSelection;
  bool found(false);
  for(unsigned int ii = 0; !found && ii < d->_currentGames.size(); ++ii)
  {
    if(d->_currentGames[ii]->getPosition() >= currentPosition)
    {
      found = true;
      d->_currentGameIndex = ii;
    }
  }
  if(!found)
  {
    if(d->_currentGames.empty())
      d->_currentGameIndex=0;
    else
      d->_currentGameIndex = d->_currentGames.size() - 1;
  }
  d->recomputeCurrentPage();
}

void GraphicStatus::nextPage()
{
  d->offsetCurrentGameIndex(NUM_GAMES);
}

void GraphicStatus::previousPage()
{
  d->offsetCurrentGameIndex(- NUM_GAMES);
}

void GraphicStatus::nextGame()
{
  d->offsetCurrentGameIndex(1);
}

void GraphicStatus::previousGame()
{
  d->offsetCurrentGameIndex(-1);
}

const vector<Game*>& GraphicStatus::getDisplayedGames() const
{
  return d->_gamesInCurrentPage;
}

unsigned int GraphicStatus::getGameIndexInPage() const
{
  if(d->_currentGameIndex == 0)
    return 0;
  return d->_currentGameIndex % NUM_GAMES;
}

Game* GraphicStatus::getCurrentGame() const
{
  return d->_currentGames[d->_currentGameIndex];
}
