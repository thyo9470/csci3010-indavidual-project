#include<iostream>

#include<ctime>
#include<stdlib.h>

#include <QApplication>

#include "headers/Window.h"
#include "headers/Game.h"

using namespace std;

int main(int argc, char *argv[])
{
    srand(time(nullptr));

    QApplication a(argc, argv);
    Game* game = new Game();

    game->GameLoop();

    return a.exec();
}
