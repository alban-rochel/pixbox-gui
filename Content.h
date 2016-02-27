#ifndef CONTENT_H
#define CONTENT_H

#include <list>
#include <vector>
#include <string>

#define MAX_SHORTNAME_LENGTH 20
#ifdef WIN32
#define RESOURCE_PATH(resource) (string("C:\\temp\\pixbox-resources\\")+resource)
#else
#define RESOURCE_PATH(resource) (string("/media/BBB/pixbox/resources/")+resource)
#endif

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

    void setIsMultiplayer(bool isMultiplayer=false);
    bool isMultiplayer() const;

    void addGameFamily(const std::string& family);
    const std::list<std::string>& getGameFamilies() const;

    void setCommandLine(const std::string& commandLine);
    const std::string& getCommandLine() const;

    void setPicturePath(const std::string& picturePath);
    const std::string& getPicturePath() const;

    bool operator<(const Game&) const;

    bool matches(const std::string& type = std::string(),
                 const std::string& device = std::string(),
                 bool isMultiplayer = false,
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
