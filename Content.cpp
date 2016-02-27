#include "Content.h"

#include <iostream>
#include <algorithm>
#include <fstream>
#include "PixBox.h"

using namespace std;

class Game::Private
{
  public:
    Private();
    Private(const Private& other);
    Private& operator=(const Private& other);
    ~Private(){}

    string _name;
    string _shortName;
    string _sortKey;
    list<string> _gameTypes;
    string _device;
    bool _multiplayer;
    list<string> _gameFamily;
    string _commandLine;
    string _picturePath;
    unsigned int _position;
};

Game::Private::Private():
  _name(),
  _shortName(),
  _sortKey(),
  _gameTypes(),
  _device(),
  _multiplayer(false),
  _gameFamily(),
  _commandLine(),
  _picturePath(),
  _position(0)
{}

Game::Private::Private(const Game::Private &other):
  _name(other._name),
  _shortName(other._shortName),
  _sortKey(other._sortKey),
  _gameTypes(other._gameTypes),
  _device(other._device),
  _multiplayer(other._multiplayer),
  _gameFamily(other._gameFamily),
  _commandLine(other._commandLine),
  _picturePath(other._picturePath),
  _position(other._position)
{}

Game::Private& Game::Private::operator =(const Game::Private& other)
{
  if(&other == this)
    return *this;

  _name = other._name;
  _shortName = other._shortName;
  _sortKey = other._sortKey;
  _gameTypes = other._gameTypes;
  _device = other._device;
  _multiplayer = other._multiplayer;
  _gameFamily = other._gameFamily;
  _commandLine = other._commandLine;
  _picturePath = other._picturePath;
  _position = other._position;

  return *this;
}










Game::Game():
  d(new Private)
{}

Game::Game(const Game& other):
  d(new Private(*other.d))
{
}

Game& Game::operator=(const Game& other)
{
  if(&other == this)
    return *this;

  d = other.d;

  return *this;
}

Game::~Game()
{
  delete d;
}

void Game::setPosition(unsigned int pos)
{
  d->_position = pos;
}

unsigned int Game::getPosition() const
{
  return d->_position;
}

void Game::setName(const std::string& name)
{
  d->_name = name;
  if(d->_shortName.empty())
    d->_shortName = name;
  if(d->_sortKey.empty())
    d->_sortKey = name;
}

const std::string& Game::getName() const
{
  return d->_name;
}

void Game::setShortName(const std::string& shortName)
{
  if(shortName.empty() && !d->_shortName.empty())
    return;
  d->_shortName = shortName.substr(0, MAX_SHORTNAME_LENGTH);
}

const std::string& Game::getShortName() const
{
  return d->_shortName;
}

void Game::setSortKey(const std::string& sortKey)
{
  if(sortKey.empty() && !d->_sortKey.empty())
    return;
  d->_sortKey = sortKey;
}

const std::string& Game::getSortKey() const
{
  return d->_sortKey;
}

void Game::addGameType(const std::string& type)
{
  d->_gameTypes.push_back(type);
}

const std::list<std::string>& Game::getGameTypes() const
{
  return d->_gameTypes;
}

void Game::setDevice(const std::string& device)
{
  d->_device = device;
}

const std::string& Game::getDevice() const
{
  return d->_device;
}

void Game::setIsMultiplayer(bool isMultiplayer)
{
  d->_multiplayer = isMultiplayer;
}

bool Game::isMultiplayer() const
{
  return d->_multiplayer;
}

void Game::addGameFamily(const std::string& family)
{
  d->_gameFamily.push_back(family);
}

const std::list<std::string>& Game::getGameFamilies() const
{
  return d->_gameFamily;
}

void Game::setCommandLine(const string& commandLine)
{
  d->_commandLine = commandLine;
}

const string& Game::getCommandLine() const
{
  return d->_commandLine;
}

void Game::setPicturePath(const string& picturePath)
{
  d->_picturePath = picturePath;
}

const string& Game::getPicturePath() const
{
  return d->_picturePath;
}

bool Game::operator <(const Game& other) const
{
  if(d->_sortKey != other.d->_sortKey)
    return (d->_sortKey < other.d->_sortKey);

  if(d->_name != other.d->_name)
    return (d->_name < other.d->_name);

  if(d->_shortName != other.d->_shortName)
    return (d->_shortName < other.d->_shortName);

  if(d->_device != other.d->_device)
    return (d->_device < other.d->_device);

  return true;
}

bool Game::matches(const string& type,
                   const string& device,
                   bool isMultiplayer,
                   const string& family) const
{
//  cout << "matches " << d->_name.c_str() << endl;
  if(!type.empty())
  {
    bool hasType(false);
    for(list<string>::iterator iter = d->_gameTypes.begin();
        iter != d->_gameTypes.end() && !hasType;
        ++iter)
    {
      hasType = (*iter == type);
    }
    if(!hasType)
      return false;
  }

  if(!device.empty() && d->_device != device)
    return false;

  if(isMultiplayer && !d->_multiplayer)
    return false;

  if(!family.empty())
  {
    bool hasFamily(false);
    for(list<string>::iterator iter = d->_gameFamily.begin();
        iter != d->_gameFamily.end() && !hasFamily;
        ++iter)
    {
      hasFamily = (*iter == family);
    }
    if(!hasFamily)
      return false;
  }
//  cout << "YES" << endl;
  return true;
}



class Content::Private
{
  public:
    Private();
    ~Private();

    void reset();
    void resetSelectedGames();
    void regenerateSelection();

    list<Game*> _allGames;
    vector<Game*> _selectedGames;

    vector<string> _types;
    int _currentType;
    string _currentTypeString;
    vector<string> _devices;
    int _currentDevice;
    string _currentDeviceString;
    bool _currentMultiplayer;
    string _currentMultiplayerString;
    vector<string> _families;
    int _currentFamily;
    string _currentFamilyString;

    static bool comp(Game* first, Game* second);

    static list<string> split(const string&, const char sep);
    static string macroSubstitution(const list< pair<string, string> >& macros, const string& str);
    static string unixify(const string& command);
};

Content::Private::Private()
{
  reset();
}

Content::Private::~Private()
{
  reset();
}

void Content::Private::reset()
{
  for(list<Game*>::iterator iter = _allGames.begin();
      iter != _allGames.end();
      ++iter)
  {
    delete *iter;
  }

    _allGames.clear();
  _selectedGames.clear();

  _types.clear();
  _currentType = -1;
  _currentTypeString.clear();

  _devices.clear();
  _currentDevice = -1;
  _currentDeviceString.clear();

  _currentMultiplayer = false;
  _currentMultiplayerString = "1P/2P";

  _families.clear();
  _currentFamily = -1;
  _currentFamilyString.clear();
}

void Content::Private::regenerateSelection()
{
  _selectedGames.clear();

  for(list<Game*>::iterator iter = _allGames.begin();
      iter != _allGames.end();
      ++iter)
  {
    if((*iter)->matches(_currentTypeString,
                        _currentDeviceString,
                        _currentMultiplayer,
                        _currentFamilyString))
    {
      _selectedGames.push_back(*iter);
    }
  }
}

void Content::Private::resetSelectedGames()
{
  for(vector<Game*>::iterator iter = _selectedGames.begin();
      iter != _selectedGames.end();
      ++iter)
  {
    delete *iter;
  }
}

bool Content::Private::comp(Game* first, Game* second)
{
  return (*first < *second);
}

list<string> Content::Private::split(const string & str, const char sep)
{
  string temp(str);
  list<string> res;

  if(str.empty())
    return res;

  size_t pos = temp.find(sep, 0);
  while(pos != string::npos)
  {
    res.push_back(temp.substr(0, pos));
    temp = temp.substr(pos + 1, temp.length());
    pos = temp.find(sep, 0);
  }
  res.push_back(temp);

  return res;
}

string Content::Private::macroSubstitution(const list< pair<string, string> >& macros, const string& str)
{
  string result(str);
  for(list< pair <string, string> >::const_iterator iter = macros.begin();
      iter != macros.end();
      ++iter)
  {
    string macro = iter->first;
    int macroSize = macro.length();
    string subst = iter->second;
    int position = result.find(macro);
    while(position != string::npos)
    {
      result = result.replace(position, macroSize, subst);
      position = result.find(macro);
    }

  }

  return result;
}

string Content::Private::unixify(const string& command)
{
  string res;
  for(unsigned int ii = 0; ii < command.length(); ++ii)
  {
    switch(command[ii])
    {
      case ' ':
      case '(':
      case ')':
      case '&':
	case '[':
	case ']':
	case '!':
      case '\'':
        res += '\\';
        res += command[ii];
        break;

      case '\r':
        break;

      default:
        res += command[ii];
        break;
    }
  } // end for(unsigned int ii = 0; ii < command.length(); ++ii)

  return res;
}

Content::Content():
  d(new Private)
{

}

Content::~Content()
{
  delete d;
}

void Content::startAddingGames()
{
  d->reset();
}

void Content::addGame(const Game& game)
{
  d->_allGames.push_back(new Game(game));

  const std::list<string>& types = game.getGameTypes();

  // Add type
  for(list<string>::const_iterator gameTypeIter = types.begin();
      gameTypeIter != types.end();
      ++gameTypeIter)
  {
    bool found(false);

    for(vector<string>::const_iterator currentTypeIter = d->_types.begin();
        currentTypeIter != d->_types.end() && !found;
        ++currentTypeIter)
    {
      if(*currentTypeIter == *gameTypeIter)
      {
        found = true;
      }
    }
    if(!found)
    {
      d->_types.push_back(*gameTypeIter);
    }
  } // end for(std::list<std::string>::const_iterator* gameTypeIter = types.begin();

  // Add device
  {
    bool found(false);

    for(vector<string>::const_iterator currentDeviceIter = d->_devices.begin();
        currentDeviceIter != d->_devices.end() && !found;
        ++currentDeviceIter)
    {
      if(*currentDeviceIter == game.getDevice())
      {
        found = true;
      }
    }
    if(!found)
    {
      d->_devices.push_back(game.getDevice());
    }
  } // end for(vector<string>::const_iterator currentDeviceIter = d->_devices.begin();

  const std::list<string>& families = game.getGameFamilies();

  // Add family
  for(list<string>::const_iterator gameFamilyIter = families.begin();
      gameFamilyIter != families.end();
      ++gameFamilyIter)
  {
    if(!gameFamilyIter->empty())
{
          bool found(false);
    for(vector<string>::const_iterator currentFamilyIter = d->_families.begin();
        currentFamilyIter != d->_families.end() && !found;
        ++currentFamilyIter)
    {
      if(*gameFamilyIter == *currentFamilyIter)
      {
        found = true;
      }
    }
    if(!found)
    {
      d->_families.push_back(*gameFamilyIter);
    }
    }
  } // end for(list<string>::const_iterator gameFamilyIter = families.begin();
}

void Content::stopAddingGames()
{
  d->_allGames.sort(Content::Private::comp);

  unsigned int counter(0);
  for(list<Game*>::iterator iter = d->_allGames.begin();
      iter != d->_allGames.end();
      ++iter)
  {
    (*iter)->setPosition(counter++);
  }

  std::sort(d->_devices.begin(), d->_devices.end());
  std::sort(d->_types.begin(), d->_types.end());
  std::sort(d->_families.begin(), d->_families.end());
  d->regenerateSelection();
}

const string& Content::nextGameType()
{
  ++d->_currentType;

  if(d->_currentType >= d->_types.size())
  {
    d->_currentType = -1;
    d->_currentTypeString.clear();
  }
  else
  {
    d->_currentTypeString = d->_types[d->_currentType];
  }

  d->regenerateSelection();

  return d->_currentTypeString;
}

const string& Content::nextDevice()
{
  ++d->_currentDevice;

  if(d->_currentDevice >= d->_devices.size())
  {
    d->_currentDevice = -1;
    d->_currentDeviceString.clear();
  }
  else
  {
    d->_currentDeviceString = d->_devices[d->_currentDevice];
  }

  d->regenerateSelection();

  return d->_currentDeviceString;
}

const string& Content::nextMultiplayer()
{
  d->_currentMultiplayer = !d->_currentMultiplayer;

  if(d->_currentMultiplayer)
  {
    d->_currentMultiplayerString = "2P";
  }
  else
  {
    d->_currentMultiplayerString = "1P/2P";
  }

  d->regenerateSelection();

  return d->_currentMultiplayerString;
}

const string& Content::nextGameFamily()
{
  ++d->_currentFamily;

  if(d->_currentFamily >= d->_families.size())
  {
    d->_currentFamily = -1;
    d->_currentFamilyString.clear();
  }
  else
  {
    d->_currentFamilyString = d->_families[d->_currentFamily];
  }

  d->regenerateSelection();

  return d->_currentFamilyString;
}




const string& Content::previousGameType()
{
  if(d->_currentType < 0)
  {
    d->_currentType = d->_types.size() - 1;
  }
  else
  {
    --d->_currentType;
  }

  if(d->_currentType < 0)
  {
    d->_currentType = -1;
    d->_currentTypeString.clear();
  }
  else
  {
    d->_currentTypeString = d->_types[d->_currentType];
  }

  d->regenerateSelection();

  return d->_currentTypeString;
}

const string& Content::previousDevice()
{
  if(d->_currentDevice < 0)
  {
    d->_currentDevice = d->_devices.size() - 1;
  }
  else
  {
    --d->_currentDevice;
  }

  if(d->_currentDevice < 0)
  {
    d->_currentDevice = -1;
    d->_currentDeviceString.clear();
  }
  else
  {
    d->_currentDeviceString = d->_devices[d->_currentDevice];
  }

  d->regenerateSelection();

  return d->_currentDeviceString;
}

const string& Content::previousMultiplayer()
{
  return nextMultiplayer();
}

const string& Content::previousGameFamily()
{
  if(d->_currentFamily < 0)
  {
    d->_currentFamily = d->_families.size() - 1;
  }
  else
  {
    --d->_currentFamily;
  }

  if(d->_currentFamily < 0)
  {
    d->_currentFamily = -1;
    d->_currentFamilyString.clear();
  }
  else
  {
    d->_currentFamilyString = d->_families[d->_currentFamily];
  }

  d->regenerateSelection();

  return d->_currentFamilyString;
}



const vector<Game*>& Content::currentSelection() const
{
  return d->_selectedGames;
}

bool Content::init()
{
#define ADDGAME(title, shortName, sortKey, type, device, isMultiplayer, family, artwork, command)\
{\
  Game game;\
  game.setName(title);\
  game.setShortName(shortName);\
  game.setSortKey(sortKey);\
  game.addGameType(type);\
  game.setDevice(device);\
  game.setIsMultiplayer(isMultiplayer);\
  game.addGameFamily(family);\
  game.setPicturePath(artwork);\
  game.setCommandLine(command);\
  addGame(game);\
}

#define ADDGAME2(title, shortName, sortKey, types, device, isMultiplayer, family, artwork, command)\
{\
  Game game;\
  game.setName(title);\
  game.setShortName(shortName);\
  game.setSortKey(sortKey);\
  for(list<string>::const_iterator iter = types.begin(); iter != types.end(); ++iter) game.addGameType(*iter);\
  game.setDevice(device);\
  game.setIsMultiplayer(isMultiplayer);\
  game.addGameFamily(family);\
  game.setPicturePath(artwork);\
  game.setCommandLine(command);\
  addGame(game);\
}

  startAddingGames();

#define FROM_FILE 1
#ifdef FROM_FILE

  loadFile();

#else
  ADDGAME("Sonic the Hedgehog", "Sonic", "sonic1", "Platform", "Megadrive", false, "Sonic", "", "");
  ADDGAME("Sonic the Hedgehog", "Sonic", "sonic1", "Platform", "Game Gear", false, "Sonic", "", "");
  ADDGAME("Sonic the Hedgehog 2", "Sonic 2", "sonic2", "Platform", "Megadrive", true, "Sonic", "", "");
  ADDGAME("Sonic the Hedgehog 3", "Sonic 3", "sonic3", "Platform", "Megadrive", true, "Sonic", "", "");
  ADDGAME("Sonic & Knuckles", "Sonic & Knuckles", "sonic4", "Platform", "Megadrive", false, "Sonic", "", "");
  ADDGAME("Super Mario Bros", "Super Mario Bros", "super-mario1", "Platform", "NES", true, "Mario", "", "");
  ADDGAME("Super Mario Bros 2", "Super Mario Bros 2", "super-mario2", "Platform", "NES", true, "Mario", "", "");
  ADDGAME("Super Mario Bros 3", "Super Mario Bros 3", "super-mario3", "Platform", "NES", true, "Mario", "", "");
  ADDGAME("Super Mario World", "Super Mario World", "super-mario-world1", "Platform", "SNES", true, "Mario", "", "");
  ADDGAME("Super Mario Kart", "Super Mario Kart", "super-mario-kart", "Racing", "SNES", true, "Mario", "", "");
  ADDGAME("Castle of Illusion Starring Mickey Mouse", "Castle of Illusion", "castle-of-illusion", "Platform", "Megadrive", false, "Disney", "", "");
  ADDGAME("01234567890123456789012345679", "01234567890123456789012345679", "01234567890123456789012345679", "Platform", "Megadrive", false, "Disney", "", "");
  ADDGAME("Aladdin", "Aladdin", "aladdin", "Platform", "Megadrive", false, "Disney", "", "");
  ADDGAME("Aladdin", "Aladdin", "aladdin", "Platform", "SNES", false, "Disney", "", "");
  ADDGAME("Aladdin", "Aladdin", "aladdin", "Platform", "Game Gear", false, "Disney", "", "");
  ADDGAME("Quackshot", "Quackshot", "quackshot", "Platform", "Megadrive", false, "Disney", "", "");
  ADDGAME("Duke Nukem 3D", "Duke Nukem 3D", "duke-nukem-3d", "FPS", "PC", false, "", "", "");
  ADDGAME("Wipeout", "Wipeout", "wipeout", "Racing", "Playstation", true, "", "", "");
  ADDGAME("Wipeout XL", "Wipeout XL", "wipeout-xl", "Racing", "Playstation", true, "", "", "");
  ADDGAME("Metal Slug", "Metal Slug", "metal-slug", "Platform", "Neo Geo", true, "", "", "");
  ADDGAME("Metal Slug 2", "Metal Slug 2", "metal-slug-2", "Platform", "Neo Geo", true, "", "", "");
  ADDGAME("Metal Slug 3", "Metal Slug 3", "metal-slug-3", "Platform", "Neo Geo", true, "", "", "");
  ADDGAME("Metal Slug X", "Metal Slug X", "metal-slug-4", "Platform", "Neo Geo", true, "", "", "");
  ADDGAME("Virtua Racing", "Virtua Racing", "virtua-racing", "Racing", "Megadrive", true, "", "", "");
  ADDGAME("Micro Machines 96", "Micro Machines 96", "micro-machines-96", "Racing", "Megadrive", true, "", "", "");
  ADDGAME("Road Rash", "Road Rash", "road-rash", "Racing", "Megadrive", true, "", "", "");
  ADDGAME("Road Rash 2", "Road Rash", "road-rash-2", "Racing", "Megadrive", true, "", "", "");
  ADDGAME("Road Rash 3 - Tour de force", "Road Rash", "road-rash-3", "Racing", "Megadrive", true, "", "", "");
#endif



  stopAddingGames();

  if(!PixBox::instance()->isQuiet())
  {
    cout << "ALL GAMES" << endl;
    for(list<Game*>::const_iterator iter = d->_allGames.begin();
        iter != d->_allGames.end();
        ++iter)
    {
      cout << (*iter)->getName().c_str() << endl;
    }
    cout << "DONE ALL GAMES" << endl;
  }


  return true;
}

bool Content::quit()
{
  return true;
}


void Content::loadFile()
{
  ifstream file;
  file.open(RESOURCE_PATH("pixbox.csv").c_str());

  char line[1000];
  string lineStr;

  list< pair<string,string> > macros;

  while(file.good())
  {
    file.getline(line, 1000);
    lineStr = line;

    list<string> split = Private::split(lineStr, ';');
    if(!split.empty())
    {
      string first = split.front();
      if(first == "MACRO")
      {
        split.pop_front();
        string macroName, macroValue;
        if(!split.empty())
        {
          macroName = split.front();
          split.pop_front();
        }
        if(!split.empty())
        {
          macroValue = split.front();
          macroValue = Private::macroSubstitution(macros, macroValue);
          macros.push_back(pair<string, string>(macroName, macroValue));
        }
      }
      else if(first == "PREPARE")
      {
        split.pop_front();
        system(split.front().c_str());
      }
      else if(!first.empty() && first[0] == '#')
      {
        // Ignore
      }
      else
      {
        string title,
            shortTitle,
            sortKey,
            device,
            family,
            multiplayer,
            artwork,
            command;
        list<string> types;
        if(!split.empty()) {title = split.front(); split.pop_front();}
        if(!split.empty()) {shortTitle = split.front(); split.pop_front();}
        if(!split.empty()) {sortKey = split.front(); split.pop_front();}
        if(!split.empty()) {string typesStr = split.front(); types = d->split(typesStr, '%'); split.pop_front();}
        if(!split.empty()) {device = split.front(); split.pop_front();}
        if(!split.empty()) {multiplayer = split.front(); split.pop_front();}
        if(!split.empty()) {family = split.front(); split.pop_front();}
        if(!split.empty()) {artwork = split.front(); artwork = Private::macroSubstitution(macros, artwork); split.pop_front();}
        if(!split.empty()) {command = split.front(); command=Private::unixify(command); command = Private::macroSubstitution(macros, command);}
        ADDGAME2(title, shortTitle, sortKey, types, device, multiplayer != "1", family, artwork, command);
      }
    } // end if(!split.empty())
  }

  file.close();
}


