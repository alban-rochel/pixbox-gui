#pragma once

// Resolution
#define PIXBOX_WIDTH 1280
#define PIXBOX_HEIGHT 720

// Number of games per screen
#define NUM_GAMES 20

// Resources: "database", graphics, sounds
#define MAX_SHORTNAME_LENGTH 20
#ifdef WIN32
#define RESOURCE_PATH(resource) (string("C:\\temp\\pixbox-resources\\")+resource)
#else
#define RESOURCE_PATH(resource) (string("/media/BBB/pixbox/resources/")+resource)
#endif
#define DATA_FILE "pixbox.csv"
#define BACKGROUND_IMAGE "pixbox-interface3.png"
#define SOUND_ONE "smb_coin.wav"
#define SOUND_TWO "smb_bump.wav"

// Text, to translate to your own language
#define TEXT_MACHINE "Machine"
#define TEXT_TYPE "Type"
#define TEXT_PLAYERS "Joueurs"
#define TEXT_GAME_FAMILY "Famille"
#define TEXT_GAME "Jeu"
#define TEXT_ALL "Tout"
