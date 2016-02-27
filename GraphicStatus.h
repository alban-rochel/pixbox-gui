#ifndef GRAPHICSTATUS_H
#define GRAPHICSTATUS_H

#include <vector>

class Game;

class GraphicStatus
{
  public:
    GraphicStatus();
    ~GraphicStatus();

    bool init();

    bool quit();

    void setCurrentGames(const std::vector<Game*>& currentSelection);

    void nextPage();
    void previousPage();
    void nextGame();
    void previousGame();

    const std::vector<Game*>& getDisplayedGames() const;
    unsigned int getGameIndexInPage() const;
    Game* getCurrentGame() const;

  private:
    class Private;
    Private* d;
};

#endif // GRAPHICSTATUS_H
