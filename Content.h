#ifndef CONTENT_H
#define CONTENT_H

#include <list>
#include <vector>
#include <string>
#include "defines.h"


extern bool IsQuiet;

class Game
{
  public:
    Game();
    Game(const Game&);
    Game& operator=(const Game&);
    ~Game();

    void setPosition(unsigned int pos);
    unsigned int getPosition() const;

    void setName(const std::string& name);
    const std::string& getName() const;

    void setShortName(const std::string& shortName);
    const std::string& getShortName() const;

    void setSortKey(const std::string& sortKey);
    const std::string& getSortKey() const;

    void addGameType(const std::string& type);
    const std::list<std::string>& getGameTypes() const;

    void setDevice(const std::string& device);
    const std::string& getDevice() const;

    void setNumPlayers(unsigned int numPlayers);
    unsigned int getMaxPlayers() const;
    std::string getMaxPlayersString() const;

    void addGameFamily(const std::string& family);
    const std::list<std::string>& getGameFamilies() const;

    void setCommandLine(const std::string& commandLine);
    const std::string& getCommandLine() const;

    void setPicturePath(const std::string& picturePath);
    const std::string& getPicturePath() const;

    bool operator<(const Game&) const;

    bool matches(const std::string& type = std::string(),
                 const std::string& device = std::string(),
                 unsigned int numPlayers = 1,
                 const std::string& family = std::string()) const;

  private:

    class Private;
    Private* d;

};

class Content
{
  public:
    Content();
    ~Content();

    void startAddingGames();
    void addGame(const Game&);
    void stopAddingGames();

    const std::string& nextGameType();
    const std::string& nextDevice();
    const std::string& nextMultiplayer();
    const std::string& nextGameFamily();

    const std::string& previousGameType();
    const std::string& previousDevice();
    const std::string& previousMultiplayer();
    const std::string& previousGameFamily();

    const std::vector<Game*>& currentSelection() const;

    bool init();
    bool quit();

  private:
        void loadFile();
    class Private;
    Private* d;
};

#endif // CONTENT_H
