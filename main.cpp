#include "PixBox.h"
#include <ctime>
#undef main

void printPix()
{
printf("____                 ____                     \\n"
       "/\\  _`\\   __         /\\  _`\\                   \\n"
       "\\ \\ \\L\\ \\/\\_\\   __  _\\ \\ \\L\\ \\    ___   __  _  \\n"
       " \\ \\ ,__/\\/\\ \\ /\\ \\/'\\\\ \\  _ <'  / __`\\/\\ \\/'\\ \\n"
       "  \\ \\ \\/  \\ \\ \\\\/>  </ \\ \\ \\L\\ \\/\\ \\L\\ \\/>  </ \\n"
       "   \\ \\_\\   \\ \\_\\/\\_/\\_\\ \\ \\____/\\ \\____//\\_/\\_\\\\n"
       "    \\/_/    \\/_/\\//\\/_/  \\/___/  \\/___/ \\//\\/_/\\n");
}

int main(int argc, char **)
{
  srand(time(NULL));

  printPix();

  PixBox::instance()->init(argc != 1);

  if(!PixBox::instance()->isQuiet())
    printf("SDL initialized.\\n");

  PixBox::instance()->mainLoop();

  if(!PixBox::instance()->isQuiet())
    printf("Quiting SDL.\\n");

  PixBox::instance()->quit();

  if(!PixBox::instance()->isQuiet())
  printf("Quiting....\\n");

  return 0;
}

