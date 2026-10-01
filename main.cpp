#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<SDL_mixer.h>
#include<Timer.h>
#include<sstream>
#include<cstdlib>
#include<cmath>
#include<ctime>
#include<iostream>
#include<vector>
#include<fstream>
#define POPULATION 1000
#define snakeWidth 18
#define S_MARGIN 2

#include<Classes.h>
#include<Game.h>
#include<Snake.h>

int main(int argc , char* argv[]){
    srand( time(0) );
    Timer FPS(60);

    int gridW = 0 , gridH = 0;

    std::cout << "Enter grid width: ";
    std::cin >> gridW;

    std::cout << "Enter grid height: ";
    std::cin >> gridH;

    int yes[2] = {0};

    std::cout << "Crossover? ( 1.On 2.Off )\n";
    std::cin >> yes[0];

    std::cout << "Pause when a snake wins? ( 1.Yes 2.No )\n";
    std::cin >> yes[1];

    system("cls");

    Game game(gridW , gridH , 1);

    game.setCrossover( yes[0]==1?1:0 );
    game.pauseWhenWon( yes[1]==1?1:0 );

    Snake snake[POPULATION];
    for(int i = 0 ; i < POPULATION ; i++){
        snake[i].setup(&game , 2);
        game.linkSnake(&snake[i]);
    }

    //game.upload();

    while(!game.Quit()){
        FPS.start();

        game.handleEvent();
        game.update();
        game.render();

        if(game.allDone()){
            if(isolateError)
                std::cout << " 10 \n";
            game.selectParents();
            game.reset();
            game.mutate();
        }

        FPS.stop();
    }

    return 0;
}
