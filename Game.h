#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

void Game::update(){
    for(int i = 0 ; i < fps_multi ; i++){
        if(!remaining) return;
    /*
        if(remaining < 500 && fps_multi < 5 && once[1]){fps_multi = 5; once[1] = 0;}
        if(remaining < 200 && once[2] && fps_multi < 10){ fps_multi = 10; once[2] = 0;}*/
        if(isolateError)
            std::cout << "1\n";

        crnt_highscore = 0;

        if(snake_pointer[shownSnake]->DEAD)
            switchShownSnake();


        if(isolateError)
            std::cout << "2\n";

        for(int i = 0 ; i < population ; i++){
            if(!snake_pointer[i]->DEAD && !snake_pointer[i]->win)
                snake_pointer[i]->update();

            if(snake_pointer[i]->win && !pause_when_won){
                snake_pointer[i]->DEAD = 1;
                decrementRemaining();
                snake_pointer[i]->calculateFitness();
            }

            checkIfHighestScore(i);
            if(!snake_pointer[i]->DEAD)
                checkIfCrntBest(i);
        }

        if(snake_pointer[shownSnake]->score == highScore && once[0]){
            Mix_PlayChannel(-1 , newRecSF , 0 );
            once[0] = 0;
        }

        if(isolateError)
            std::cout << "3\n";


        if(crnt_highscore > 200)
            for(int i = 0 ; i < population ; i++)
                if(!snake_pointer[i]->DEAD) killOffIfBad(i);


        if(isolateError)
            std::cout << "4\n";
    }
}

void Game::render(){
    SDL_SetRenderDrawColor(renderer , background_color.r , background_color.g , background_color.b , 255);
    SDL_RenderClear(renderer);

    renderGrid();
    if(isolateError) std::cout << "5 \n";

    renderFood();
    if(isolateError) std::cout << "6 \n";

    renderSnakes();
    if(isolateError) std::cout << "7 \n";

    snake_pointer[shownSnake]->renderNN();
    if(isolateError) std::cout << "8 \n";

    displayStatistics();
    if(isolateError) std::cout << "9 \n";

    SDL_RenderPresent(renderer);
}

void Game::selectParents(){
    father = mother = 0;
    for(int i = 0 ; i < population ; i++)
        if(snake_pointer[i]->fitness > snake_pointer[father]->fitness)
            father = i;

    snake_pointer[father]->saveNN(father);

    if(snake_pointer[father]->score < (highScore - 100)*0.7 ){
        father = 0;
    }

    if(snake_pointer[father]->fitness > bestFitness) bestFitness = snake_pointer[father]->fitness;

    if(crossoverON){

        if(!father) mother++;
        for(int i = 0 ; i < population ; i++)
            if(snake_pointer[i]->fitness > snake_pointer[mother]->fitness && i != father)
                mother = i;

        clone(father , 0 , population/4 - 1);
        father = 0;

        clone(mother , population/4 , population/2 - 1);
        mother = population/4 + 1;

        child = population - 1;
        crossover(father , mother);
        clone(child , population/2 , 3*population/4 - 1);

        crossover(mother , father);
        clone(child , 3*population/4 , population - 1);
    }

    else clone(father , 0 , population-1);

    generation++;
    //if( (generation == 5 && bestFitness < 1000) || (generation == 10 && bestFitness < 2500) ) restart();
}

void Game::crossover(int parent1 , int parent2){
    static int total_number_of_nodes = snake_pointer[0]->inputLayer.size()  *snake_pointer[0]->hiddenLayer1.size()
                                     + snake_pointer[0]->hiddenLayer1.size()*snake_pointer[0]->hiddenLayer2.size()
                                     + snake_pointer[0]->hiddenLayer2.size()*snake_pointer[0]->outputLayer.size();

     static int crossover_point = total_number_of_nodes/2;

    for(unsigned int i = 0 ; i < snake_pointer[0]->inputLayer.size() ; i++)
        for(unsigned int j = 0 ; j < snake_pointer[0]->hiddenLayer1.size() ; j++)
            if(i*snake_pointer[0]->hiddenLayer1.size() + j < crossover_point)
                snake_pointer[child]->weights1[i][j] = snake_pointer[parent1]->weights1[i][j];
            else
                snake_pointer[child]->weights1[i][j] = snake_pointer[parent2]->weights1[i][j];

    for(unsigned int i = 0 ; i < snake_pointer[0]->hiddenLayer1.size() ; i++)
        for(unsigned int j = 0 ; j < snake_pointer[0]->hiddenLayer2.size() ; j++)
            if(snake_pointer[0]->inputLayer.size()*snake_pointer[0]->hiddenLayer1.size() + i*snake_pointer[0]->hiddenLayer2.size() + j < crossover_point)
                snake_pointer[child]->weights2[i][j] = snake_pointer[parent1]->weights2[i][j];
            else
                snake_pointer[child]->weights2[i][j] = snake_pointer[parent2]->weights2[i][j];


    for(unsigned int i = 0 ; i < snake_pointer[0]->hiddenLayer2.size() ; i++)
        for(unsigned int j = 0 ; j < snake_pointer[0]->outputLayer.size() ; j++)
            if(snake_pointer[0]->inputLayer.size()  *snake_pointer[0]->hiddenLayer1.size()
             + snake_pointer[0]->hiddenLayer1.size()*snake_pointer[0]->hiddenLayer2.size()
           + i*snake_pointer[0]->outputLayer.size() + j < crossover_point)
                snake_pointer[child]->weights3[i][j] = snake_pointer[parent1]->weights3[i][j];
            else
                snake_pointer[child]->weights3[i][j] = snake_pointer[parent2]->weights3[i][j];
}

void Game::clone(int best , int from , int till){
    for(int index = from ; index <= till ; index++){
        for(unsigned int i = 0 ; i < snake_pointer[index]->inputLayer.size() ; i++)
            for(unsigned int j = 0 ; j < snake_pointer[index]->hiddenLayer1.size() ; j++)
                snake_pointer[index]->weights1[i][j] = snake_pointer[best]->weights1[i][j];

        for(unsigned int i = 0 ; i < snake_pointer[index]->hiddenLayer1.size() ; i++)
            for(unsigned int j = 0 ; j < snake_pointer[index]->hiddenLayer2.size() ; j++)
                snake_pointer[index]->weights2[i][j] = snake_pointer[best]->weights2[i][j];

        for(unsigned int i = 0 ; i < snake_pointer[index]->hiddenLayer2.size() ; i++)
            for(unsigned int j = 0 ; j < snake_pointer[index]->outputLayer.size() ; j++)
                snake_pointer[index]->weights3[i][j] = snake_pointer[best]->weights3[i][j];
    }
}

void Game::mutate(){
    mutation_rate = 0.105 - 0.1*(float)highScore/(float)noOfBlocks;
    if(!crossoverON)
        for(int i = 10 ; i < population ; i++)
            snake_pointer[i]->mutate( mutation_rate );

    else{
        for(int i = 10 ; i < population/4 ; i++)
            snake_pointer[i]->mutate( mutation_rate );

        for(int i = population/4 + 10 ; i < population/2 ; i++)
            snake_pointer[i]->mutate( mutation_rate );

        for(int i = population/2 + 10 ; i < population*3/4 ; i++)
            snake_pointer[i]->mutate( mutation_rate );

        for(int i = population*3/4 + 10 ; i < population ; i++)
            snake_pointer[i]->mutate( mutation_rate );
    }
}

void Game::getAverage(){
    int a = 0;
    for(int i = 0 ; i < 20 ; i++)
        a += snake_pointer[i]->score;

    if(average > prevArvg) prevArvg = average;
    average = a/20;
}

Game::Game(const int X_BLOCKS , const int Y_BLOCKS , const int Z){
    fps_multi = Z;

    background_color = {230,230,230};
    remaining = POPULATION;
    food_color = {204 , 48 , 0};
    grid_color = {205 , 205 , 205};

    gridWidth = X_BLOCKS;
    gridHeight = Y_BLOCKS;
    noOfBlocks = gridWidth*gridHeight;

    foodPos = new SDL_Point[noOfBlocks];
    for(int i = 0 ; i < noOfBlocks ; i++)
        foodPos[i] = {i/X_BLOCKS , i%X_BLOCKS};

    for(int i = 0 ; i < noOfBlocks ; i++){
        SDL_Point temp = {0,0};
        int n = rand()%noOfBlocks;

        temp = foodPos[i];
        foodPos[i] = foodPos[n];
        foodPos[n] = temp;
    }
      //  swipePoints(foodPos[i] , foodPos[ rand()%noOfBlocks ]);

    textSurf = NULL;
    textTexture = NULL;
    textSString.str("");

    frame = {PADDING_LEFT-1 , PADDING_UP-1 , X_BLOCKS*(snakeWidth + S_MARGIN) + 2 + S_MARGIN , Y_BLOCKS*(snakeWidth + S_MARGIN) + 2 + S_MARGIN};
    WINDOW_WIDTH = frame.w - 2 + PADDING_LEFT + PADDING_RIGHT;
    WINDOW_HEIGHT = ( Y_BLOCKS>=40?frame.h-2:40*(snakeWidth + S_MARGIN) ) + PADDING_UP + PADDING_DOWN;

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();
    Mix_Init(MIX_INIT_MP3);
    Mix_OpenAudio(44100 , MIX_DEFAULT_FORMAT , 1 , 2048);
	music = Mix_LoadMUS("music.mp3");
	newRecSF = Mix_LoadWAV("sf.wav");
	Mix_PlayMusic(music , -1);

    window = SDL_CreateWindow("Snake Neural Network" , SDL_WINDOWPOS_UNDEFINED , SDL_WINDOWPOS_UNDEFINED , WINDOW_WIDTH , WINDOW_HEIGHT , SDL_WINDOW_SHOWN);
    SDL_SetWindowResizable(window , SDL_TRUE);
    renderer = SDL_CreateRenderer(window , -1 , SDL_RENDERER_ACCELERATED);

    SDL_SetRenderDrawColor(renderer , 255 , 255 , 255 , 255);
    SDL_RenderClear(renderer);
    mFont = TTF_OpenFont("Courier Prime Code.ttf" , 25);
    ffont = TTF_OpenFont("Courier Prime Code.ttf" , 18);
}

Game::~Game(){
    Mix_FreeChunk(newRecSF);
    Mix_FreeMusic(music);

    TTF_CloseFont(mFont);
    TTF_CloseFont(ffont);

    SDL_FreeSurface(textSurf);
    SDL_DestroyTexture(textTexture);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);



    textSurf = NULL;
    textTexture = NULL;
    renderer = NULL;
    window = NULL;
    music = NULL;
    newRecSF = NULL;
    ffont = NULL;
    mFont = NULL;

    Mix_Quit();
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();

    delete[] foodPos;
}

void Game::reset(){
    for(int i = 0 ; i < population ; i++)
        snake_pointer[i]->reset();

    shownSnake = 0;
    for(int i = 0 ; i < 5 ; i++)
        once[i] = 1;
    crnt_highscore = 0;
    remaining = population;
    //fps_multi = 2;
}

void Game::restart(){
    generation = 1;
    bestGen = 1;
    remaining = population;
    bestFitness = 0;
    for(int i = 0 ; i < population ; i++){
        snake_pointer[i]->reset();
        snake_pointer[i]->resetNN();
    }
}

void Game::handleEvent(){
    while(SDL_PollEvent(&e))
        if(e.type == SDL_QUIT || e.key.keysym.sym == SDLK_ESCAPE) quit = true;
            else if(e.type == SDL_KEYDOWN){
                if(e.key.keysym.sym == SDLK_g){
                    if(grid_shown) grid_shown = 0;
                    else grid_shown = 1;
                }

                else if(e.key.keysym.sym == SDLK_SPACE){
                    if(Mix_PausedMusic()) Mix_ResumeMusic();
                    else Mix_PauseMusic();
                }

                else if(e.key.keysym.sym == SDLK_u) upload();
                else if(e.key.keysym.sym == SDLK_s) snake_pointer[shownSnake]->saveNN(shownSnake);
                else if(e.key.keysym.sym == SDLK_m) mutate();

               /* else if(e.key.keysym.sym == SDLK_s){
                    if(showbestonly) showbestonly = 0;
                    else showbestonly = 1;
                }*/
                else if(e.key.keysym.sym == SDLK_RIGHT){
                    shownSnake++;
                    if(shownSnake == population) shownSnake -= population;
                    while(snake_pointer[shownSnake]->DEAD){
                        shownSnake++;
                        if(shownSnake == population) shownSnake -= population;
                    }
                    if(snake_pointer[shownSnake]->snake_color.r == 57) snake_pointer[shownSnake]->snake_color = {30 , 80 , 200};
                    else snake_pointer[shownSnake]->snake_color = {57 , 115 , 115};
                }
                else if(e.key.keysym.sym == SDLK_LEFT){
                    shownSnake--;
                    if(shownSnake < 0) shownSnake += population;
                    while(snake_pointer[shownSnake]->DEAD){
                        shownSnake--;
                        if(shownSnake < 0) shownSnake += population;
                    }
                    if(snake_pointer[shownSnake]->snake_color.r == 57) snake_pointer[shownSnake]->snake_color = {30 , 80 , 200};
                    else snake_pointer[shownSnake]->snake_color = {57 , 115 , 115};
                }

                else if(e.key.keysym.sym == SDLK_UP && fps_multi < 10) fps_multi++;

                else if(e.key.keysym.sym == SDLK_DOWN && fps_multi > 1) fps_multi--;

                //else if(e.key.keysym.sym == SDLK_r) snake_pointer->reset();
            }
}

void Game::checkIfCrntBest(int i){
    if(snake_pointer[i]->score > crnt_highscore)
        crnt_highscore = snake_pointer[i]->score;
}

void Game::checkIfHighestScore(int i){
    if(snake_pointer[i]->score > highScore){
        highScore = snake_pointer[i]->score;
        bestGen = generation;
    }
}

void Game::killOffIfBad(int i){
    if(snake_pointer[i]->score < crnt_highscore - 60 && snake_pointer[i]->points < 2000){
        snake_pointer[i]->DEAD = 1;
        decrementRemaining();
        snake_pointer[i]->score = 0;
    }
}

bool Game::allDone(){
    return remaining == 0;
}

void Game::switchShownSnake(){
    while(snake_pointer[shownSnake]->DEAD){
        if(!remaining) break;
        shownSnake++;
        if(shownSnake == population) shownSnake = 0;
    }

    if(snake_pointer[shownSnake]->snake_color.r == 57) snake_pointer[shownSnake]->snake_color = {30 , 80 , 200};
    else snake_pointer[shownSnake]->snake_color = {57 , 115 , 115};
}

void Game::renderFood(){
    SDL_SetRenderDrawColor(renderer , food_color.r , food_color.g , food_color.b , 255);
    snake_pointer[shownSnake]->renderFood(renderer);
}

void Game::renderSnakes(){
    snake_pointer[shownSnake]->render(renderer);
}

void Game::displayStatistics(){
    textSString.str("");
    textSString << "GEN: " << generation;
    renderText(textSString.str().c_str() , 7 , 10 , mFont , 40 , 40 , 40);

    textSString.str("");
    textSString << "Best Fitness: " << bestFitness;
    renderText(textSString.str().c_str() , 7 , 40 , mFont , 40 , 40 , 40);

    textSString.str("");
    textSString << "Population: " << population << " (" << shownSnake;
    if(snake_pointer[shownSnake]->mutated) textSString << "*)";
    else textSString << ")";

    renderText(textSString.str().c_str() , 7 , 70 , mFont , 40 , 40 , 40);

    textSString.str("");
    textSString << "Remaining: " << remaining;
    renderText(textSString.str().c_str() , 7 , 100 , mFont , 40 , 40 , 40);

    textSString.str("");
    textSString << "Current Score: ";
    renderText(textSString.str().c_str() , 7 , 130 , mFont , 40 , 40 , 40);

    textSString.str("");
    textSString << snake_pointer[shownSnake]->score;
    renderText(textSString.str().c_str() , 230 , 130 , mFont , (bestGen == generation?0:40) , (bestGen == generation?0:40) , (bestGen == generation?255:40) );

    textSString.str("");
    textSString << "Highest Score: ";
    renderText(textSString.str().c_str() , 7 , 160 , mFont , 40 , 40 , 40);

    textSString.str("");
    textSString << highScore;
    renderText(textSString.str().c_str() , 230 , 160 , mFont , (bestGen == generation?0:40) , (bestGen == generation?0:40) , (bestGen == generation?255:40) );

    textSString.str("");
    textSString << "Best Generaion: " << bestGen;
    renderText(textSString.str().c_str() , 7 , 190 , mFont , 40 , 40 , 40);

    textSString.str("");
    textSString << "Mutation Rate: %" << 100*mutation_rate;
    renderText(textSString.str().c_str() , 7 , 220 , mFont , 40 , 40 , 40);

    textSString.str("");
    textSString << "Crossover: ";
    if(crossoverON) textSString << "On";
    else textSString << "Off";
    renderText(textSString.str().c_str() , 7 , 250 , mFont , 40 , 40 , 40);

    textSString.str("");
    textSString << "Speed X" << fps_multi;
    renderText(textSString.str().c_str() , frame.x - 140 , WINDOW_HEIGHT - 25 , mFont , 40 , 40 , 40);

    /*renderText("Points: " , 7 , 220 , mFont);
    renderNumber(snake_pointer[shownSnake]->points , 110 , 220);*/
}

void Game::renderGrid(){
    SDL_SetRenderDrawColor(renderer , grid_color.r - 40 , grid_color.g - 40 , grid_color.b - 40 , 255);
    SDL_RenderDrawRect(renderer , &frame);

    if(grid_shown){
        for(int i = 1 ; i < gridWidth ; i++)
            SDL_RenderDrawLine(renderer , frame.x + 1 + (snake_pointer[0]->SNAKE_WIDTH + S_MARGIN)*i - 1 , frame.y + 1 , frame.x + 1 + (snake_pointer[0]->SNAKE_WIDTH + S_MARGIN)*i - 1 , frame.y + frame.h);

        for(int i = 1 ; i < gridHeight ; i++)
            SDL_RenderDrawLine(renderer , frame.x + 1 , frame.y + 1 + (snake_pointer[0]->SNAKE_WIDTH + S_MARGIN)*i - 1 , frame.x + frame.w , frame.y + 1 + (snake_pointer[0]->SNAKE_WIDTH + S_MARGIN)*i - 1);
    }
}

void Game::renderText(const char *s , int x , int y , TTF_Font *font , Uint8 r , Uint8 g , Uint8 b){
    static SDL_Color font_color;
    font_color = {r , g , b};
    textSurf = TTF_RenderText_Solid(font , s , font_color);
    textRect = {x , y , textSurf->w , textSurf->h};
    textTexture = SDL_CreateTextureFromSurface(renderer , textSurf);
    SDL_RenderCopy(renderer , textTexture , NULL , &textRect);

    SDL_FreeSurface(textSurf);
    SDL_DestroyTexture(textTexture);

    textSurf = NULL;
    textTexture = NULL;
}

void Game::upload(){
    weightsFile.open("SaveFile.txt" , std::ios::in);

    weightsFile >> generation;
    if(generation <= 1) return;
    weightsFile >> highScore;
    weightsFile >> bestFitness;
    weightsFile >> bestGen;

    for(unsigned int i = 0 ; i < snake_pointer[0]->inputLayer.size() ; i++)
        for(unsigned int j = 0 ; j < snake_pointer[0]->hiddenLayer1.size() ; j++)
            weightsFile >> snake_pointer[0]->weights1[i][j];

    for(unsigned int i = 0 ; i < snake_pointer[0]->hiddenLayer1.size() ; i++)
        for(unsigned int j = 0 ; j < snake_pointer[0]->hiddenLayer2.size() ; j++)
            weightsFile >> snake_pointer[0]->weights2[i][j];

    for(unsigned int i = 0 ; i < snake_pointer[0]->hiddenLayer2.size() ; i++)
        for(unsigned int j = 0 ; j < snake_pointer[0]->outputLayer.size() ; j++)
            weightsFile >> snake_pointer[0]->weights3[i][j];

    weightsFile.close();
    clone(0 , 0 , population-1);
}

#endif // GAME_H_INCLUDED
