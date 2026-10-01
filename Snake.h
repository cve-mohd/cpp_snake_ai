#ifndef SNAKE_H_INCLUDED
#define SNAKE_H_INCLUDED


void Snake::setNN(){
    neoronRect = {25 , 0 , 25 , 25};

    for(int i = 0 ; i < 16 ; i++)
        inputLayer.push_back(0);

    for(int i = 0 ; i < 10 ; i++)
        hiddenLayer1.push_back(0);

    for(int i = 0 ; i < 10 ; i++)
        hiddenLayer2.push_back(0);

    for(int i = 0 ; i < 4 ; i++)
        outputLayer.push_back(0);

    // weights
    weights1 = new float*[ inputLayer.size() ];
    for(unsigned int i = 0 ; i < inputLayer.size() ; i++)
        weights1[i] = new float[ hiddenLayer1.size() ];

    weights2 = new float*[ hiddenLayer1.size() ];
    for(unsigned int i = 0 ; i < hiddenLayer1.size() ; i++)
        weights2[i] = new float[ hiddenLayer2.size() ];

    weights3 = new float*[ hiddenLayer2.size() ];
    for(unsigned int i = 0 ; i < hiddenLayer2.size() ; i++)
        weights3[i] = new float[ outputLayer.size() ];

    //////////////////////////////////////////////////////////////////

    for(unsigned int i = 0 ; i < inputLayer.size() ; i++)
        for(unsigned int j = 0 ; j < hiddenLayer1.size() ; j++)
            weights1[i][j] = (float)(rand()%1001 - 500)/(float)100;

    for(unsigned int i = 0 ; i < hiddenLayer1.size() ; i++)
        for(unsigned int j = 0 ; j < hiddenLayer2.size() ; j++)
            weights2[i][j] = (float)(rand()%1001 - 500)/(float)100;

    for(unsigned int i = 0 ; i < hiddenLayer2.size() ; i++)
        for(unsigned int j = 0 ; j < outputLayer.size() ; j++)
            weights3[i][j] = (float)(rand()%1001 - 500)/(float)100;
}

float sigmoid(float x){
    return x/( 1 + abs(x) );
}

void Snake::updateNN(){
    static int x , y;
    x = getBlockX(part[0]);
    y = getBlockY(part[0]);

    for(unsigned int i = 0 ; i < inputLayer.size() ; i++)
        inputLayer[i] = 0;

    for(unsigned int j = 0 ; j < hiddenLayer1.size() ; j++)
        hiddenLayer1[j] = 0;

    for(unsigned int j = 0 ; j < hiddenLayer2.size() ; j++)
        hiddenLayer2[j] = 0;

    outputLayer[0] = outputLayer[1] = outputLayer[2] = outputLayer[3] = 0;


    //distance to walls

    inputLayer[0] = 1 - sigmoid(y); //up
    inputLayer[1] = 1 - sigmoid(game_pointer->getGridW() - 1 - x); //right
    inputLayer[2] = 1 - sigmoid(game_pointer->getGridH() - 1 - y); // down
    inputLayer[3] = 1 - sigmoid(x); //left

    //tail
    for(int i = x + 1 ; i < game_pointer->getGridW() ; i++)
        if(block_value[i][y] != -2){
            inputLayer[4] = 1 - sigmoid(i - 1 - x);
            break;
        }

    for(int i = x - 1 ; i >= 0 ; i--)
        if(block_value[i][y] != -2){
            inputLayer[5] = 1 - sigmoid(x - 1 - i);
            break;
        }

    for(int i = y + 1 ; i < game_pointer->getGridH() ; i++)
        if(block_value[x][i] != -2){
            inputLayer[6] = 1 - sigmoid(i - 1 - y);
            break;
        }

    for(int i = y - 1 ; i >= 0 ; i++)
        if(block_value[x][i] != -2){
            inputLayer[7] = 1 - sigmoid(y - 1 - i);
            break;
        }



    //food
    if(y == food_position.y){
        if(x < food_position.x) inputLayer[9] = 1;
        else inputLayer[11] = 1;
    }
    else if(x == food_position.x){
        if(y < food_position.y) inputLayer[10] = 1;
        else inputLayer[8] = 1;
    }
    else if( abs(food_position.x - x) == abs(food_position.y - y) ){

        if(food_position.x > x){
            if(food_position.y > y) inputLayer[12] = 1;
            else inputLayer[13] = 1;
        }
        else if(food_position.x < x){
            if(food_position.y > y) inputLayer[14] = 1;
            else inputLayer[15] = 1;
        }

    }


    /*
    inputLayer[16] = (DIRECTION == UP?1:0);
    inputLayer[17] = (DIRECTION == RIGHT?1:0);
    inputLayer[18] = (DIRECTION == DOWN?1:0);
    inputLayer[19] = (DIRECTION == LEFT?1:0);

    inputLayer[20] = (latest_move[length-1] == UP?1:0);
    inputLayer[21] = (latest_move[length-1] == RIGHT?1:0);
    inputLayer[22] = (latest_move[length-1] == DOWN?1:0);
    inputLayer[23] = (latest_move[length-1] == LEFT?1:0);
    */


    //  //////////////////////////////////////
    for(unsigned int i = 0 ; i < inputLayer.size() ; i++)
        for(unsigned int j = 0 ; j < hiddenLayer1.size() ; j++)
            hiddenLayer1[j] += inputLayer[i]*weights1[i][j];

    for(unsigned int i = 0 ; i < hiddenLayer1.size() ; i++)
        for(unsigned int j = 0 ; j < hiddenLayer2.size() ; j++)
            hiddenLayer2[j] += hiddenLayer1[i]*weights2[i][j];

    for(unsigned int i = 0 ; i < hiddenLayer2.size() ; i++)
        for(unsigned int j = 0 ; j < outputLayer.size() ; j++)
            outputLayer[j] += hiddenLayer2[i]*weights3[i][j];


    ///////////////////////////////////////////////////////////////////////


    static dir most_activated;
    most_activated = NONE;
    for(int i = 0 ; i < 4 ; i++)
        if(outputLayer[i] > outputLayer[most_activated]) most_activated = static_cast<dir>(i);

    most_activated = static_cast<dir>(most_activated+1);

    previous_move[0] = DIRECTION;

    if(abs(most_activated - DIRECTION) != 2)
        DIRECTION = most_activated;

    else{
        if(DIRECTION == LEFT) DIRECTION = UP;
        else DIRECTION = static_cast<dir>(DIRECTION+1);
    }
}

void Snake::renderNN(){
    static const int Y_CORDINATES = 290 , X_CORDINATES = 50 , margin = 5;
    for(unsigned int i = 0 ; i < inputLayer.size() ; i++){
        neoronRect.y = Y_CORDINATES + i*(neoronRect.h+margin);
        neoronRect.x = X_CORDINATES;
        for(unsigned int j = 0 ; j < hiddenLayer1.size() ; j++){
            if(weights1[i][j] > 0) SDL_SetRenderDrawColor(game_pointer->getRenderer() , 0 , 0 , 255 , 255);
            else SDL_SetRenderDrawColor(game_pointer->getRenderer() , 255 , 0 , 0 , 255);
            SDL_RenderDrawLine(game_pointer->getRenderer() , neoronRect.x + neoronRect.w/2 , neoronRect.y + neoronRect.h/2 , X_CORDINATES*3 + neoronRect.w/2 , Y_CORDINATES
                                + (neoronRect.h+margin)*(inputLayer.size() - hiddenLayer1.size())/2 + j*(neoronRect.h+margin) + neoronRect.w/2);
        }

        SDL_SetTextureColorMod(neuronTexture , 255 , 255 , 255);
        SDL_SetTextureColorMod(neuronTexture , 255*(1 - inputLayer[i]) , 255 , 255*(1 - inputLayer[i]) );
        SDL_RenderCopy(game_pointer->getRenderer() , neuronTexture , NULL , &neoronRect);
    }

    for(unsigned int i = 0 ; i < hiddenLayer1.size() ; i++){
        neoronRect.y = Y_CORDINATES + i*(neoronRect.h+margin) + (neoronRect.h+margin)*(inputLayer.size() - hiddenLayer1.size())/2;
        neoronRect.x = X_CORDINATES*3;
        for(unsigned int j = 0 ; j < hiddenLayer2.size() ; j++){
            if(weights2[i][j] > 0) SDL_SetRenderDrawColor(game_pointer->getRenderer() , 0 , 0 , 255 , 255);
            else SDL_SetRenderDrawColor(game_pointer->getRenderer() , 255 , 0 , 0 , 255);
            SDL_RenderDrawLine(game_pointer->getRenderer() , neoronRect.x + neoronRect.w/2 , neoronRect.y + neoronRect.h/2 , X_CORDINATES*5 + neoronRect.w/2 , Y_CORDINATES
                                + (neoronRect.h+margin)*(inputLayer.size() - hiddenLayer2.size())/2 + j*(neoronRect.h+margin) + neoronRect.w/2);
        }

        SDL_SetTextureColorMod(neuronTexture , 255 , 255 , 255);
        SDL_SetTextureColorMod(neuronTexture , 255/*(1 - hiddenLayer1[i])*/ , 255 , 255*(1 - hiddenLayer1[i]) );
        SDL_RenderCopy(game_pointer->getRenderer() , neuronTexture , NULL , &neoronRect);
    }

    for(unsigned int i = 0 ; i < hiddenLayer2.size() ; i++){
        neoronRect.y = Y_CORDINATES + i*(neoronRect.h+margin) + (neoronRect.h+margin)*(inputLayer.size() - hiddenLayer2.size())/2;
        neoronRect.x = X_CORDINATES*5;
        for(unsigned int j = 0 ; j < outputLayer.size() ; j++){
            if(weights3[i][j] > 0) SDL_SetRenderDrawColor(game_pointer->getRenderer() , 0 , 0 , 255 , 255);
            else SDL_SetRenderDrawColor(game_pointer->getRenderer() , 255 , 0 , 0 , 255);
            SDL_RenderDrawLine(game_pointer->getRenderer() , neoronRect.x + neoronRect.w/2 , neoronRect.y + neoronRect.h/2 , X_CORDINATES*7 + neoronRect.w/2 , Y_CORDINATES
                                + (neoronRect.h+margin)*(inputLayer.size() - outputLayer.size())/2 + j*(neoronRect.h+margin) + neoronRect.w/2);
        }

        SDL_SetTextureColorMod(neuronTexture , 255 , 255 , 255);
        SDL_SetTextureColorMod(neuronTexture , 255/*(1 - hiddenLayer2[i])*/ , 255 , 255*(1 - hiddenLayer2[i]) );
        SDL_RenderCopy(game_pointer->getRenderer() , neuronTexture , NULL , &neoronRect);
    }

    for(unsigned int i = 0 ; i < outputLayer.size() ; i++){
        neoronRect.y = Y_CORDINATES + (neoronRect.h+margin)*(inputLayer.size() - outputLayer.size())/2 + i*(neoronRect.h+margin);
        neoronRect.x = X_CORDINATES*7;

        if(DIRECTION != i + 1) SDL_SetTextureColorMod(neuronTexture , 255 , 255 , 255);
        else SDL_SetTextureColorMod(neuronTexture , 0 , 191 , 255);
        SDL_RenderCopy(game_pointer->getRenderer() , neuronTexture , NULL , &neoronRect);
    }
}

void Snake::calculateFitness(){
    fitness = steps + score*score + 0.007*points*score;
}

void Snake::mutate(float rate){
    int rate_1 = 1/rate;
  //  std::cout << rate_1 << "\n";
    do{
        for(unsigned int i = 0 ; i < inputLayer.size() ; i++)
            for(unsigned int j = 0 ; j < hiddenLayer1.size() ; j++)
                if( rand()%rate_1 == 1 ){mutated = true; weights1[i][j] = (float)(rand()%1001 - 500)/(float)100;}

        for(unsigned int i = 0 ; i < hiddenLayer1.size() ; i++)
            for(unsigned int j = 0 ; j < hiddenLayer2.size() ; j++)
                if( rand()%rate_1 == 1 ){mutated = true; weights2[i][j] = (float)(rand()%1001 - 500)/(float)100;}

        for(unsigned int i = 0 ; i < hiddenLayer2.size() ; i++)
            for(unsigned int j = 0 ; j < outputLayer.size() ; j++)
                if( rand()%rate_1 == 1 ){mutated = true; weights3[i][j] = (float)(rand()%1001 - 500)/(float)100;}
    }while(!mutated);
}

Snake::~Snake(){
    for(int i = 0 ; i < game_pointer->getGridW() ; i++)
        delete[] block_value[i];

    for(unsigned int i = 0 ; i < inputLayer.size() ; i++)
        delete[] weights1[i];

    for(unsigned int i = 0 ; i < hiddenLayer1.size() ; i++)
        delete[] weights2[i];

    for(unsigned int i = 0 ; i < hiddenLayer2.size() ; i++)
        delete[] weights3[i];

    delete[] weights1;
    delete[] weights2;
    delete[] weights3;
    delete[] block_value;
    delete[] part;
    delete[] previous_move;
    delete[] latest_move;
    SDL_DestroyTexture(neuronTexture);
    neuronTexture = 0;
}

void Snake::saveNN(int snakeINdex){
    static std::stringstream filename_;
    filename_.str("");
    filename_ << "generations\\G" << game_pointer->getGeneration() << "S" << snakeINdex << ".txt";
    weightsOutputFile.open(filename_.str().c_str() , std::ios::out | std::ios::trunc);

    weightsOutputFile << game_pointer->getGeneration() << ' ' << game_pointer->getHighestScore() << ' ';
    weightsOutputFile << game_pointer->getFitness() << ' ' << game_pointer->getBestGen() << ' ';
    for(unsigned int i = 0 ; i < inputLayer.size() ; i++)
        for(unsigned int j = 0 ; j < hiddenLayer1.size() ; j++)
            weightsOutputFile << weights1[i][j] << ' ';

    for(unsigned int i = 0 ; i < hiddenLayer1.size() ; i++)
        for(unsigned int j = 0 ; j < hiddenLayer2.size() ; j++)
            weightsOutputFile << weights2[i][j] << ' ';

    for(unsigned int i = 0 ; i < hiddenLayer2.size() ; i++)
        for(unsigned int j = 0 ; j < outputLayer.size() ; j++)
            weightsOutputFile << weights3[i][j] << ' ';

    weightsOutputFile.close();

    weightsOutputFile.open("SaveFile.txt" , std::ios::out | std::ios::trunc);

    weightsOutputFile << game_pointer->getGeneration() << ' ' << game_pointer->getHighestScore() << ' ';
    weightsOutputFile << game_pointer->getFitness() << ' ' << game_pointer->getBestGen() << ' ';
    for(unsigned int i = 0 ; i < inputLayer.size() ; i++)
        for(unsigned int j = 0 ; j < hiddenLayer1.size() ; j++)
            weightsOutputFile << weights1[i][j] << ' ';

    for(unsigned int i = 0 ; i < hiddenLayer1.size() ; i++)
        for(unsigned int j = 0 ; j < hiddenLayer2.size() ; j++)
            weightsOutputFile << weights2[i][j] << ' ';

    for(unsigned int i = 0 ; i < hiddenLayer2.size() ; i++)
        for(unsigned int j = 0 ; j < outputLayer.size() ; j++)
            weightsOutputFile << weights3[i][j] << ' ';

    weightsOutputFile.close();
}

void Snake::reset(){
    snake_color = {30,80,200};
    lifespan = 200 + 2*game_pointer->getHighestScore();
    points = 0;
    fitness = 0;
    steps = 0;
    score = 0;
    foodindex = -1;
    for(int i=0 ; i < length ; i++){
        part[i] = {0,0};
        previous_move[i] = NONE;
        latest_move[i] = NONE;
    }

    for(int i = 0 ; i < game_pointer->getGridW() ; i++)
        for(int j = 0 ; j < game_pointer->getGridH() ; j++)
            block_value[i][j] = -2;

    DIRECTION = RIGHT;

    length = init_lenght;
    for(int i=0 ; i < length ; i++){
        part[i] = {game_pointer->getFrameRect()->x + 1 + S_MARGIN + (game_pointer->getGridH()/2-i)*(SNAKE_WIDTH + S_MARGIN) , game_pointer->getFrameRect()->y + 1 + S_MARGIN + (game_pointer->getGridH()/2)*(SNAKE_WIDTH + S_MARGIN)};
        if(part[i].x >= game_pointer->getFrameRect()->x + 1)
            block_value[getBlockX(part[i])][getBlockY(part[i])] = i;
        previous_move[i] = DIRECTION;
        latest_move[i] = DIRECTION;

    }

    DEAD = false;
    mutated = false;
    make_food();
}

void Snake::resetNN(){
    for(unsigned int i = 0 ; i < inputLayer.size() ; i++)
        for(unsigned int j = 0 ; j < hiddenLayer1.size() ; j++)
            weights1[i][j] = (float)(rand()%1001 - 500)/(float)100;

    for(unsigned int i = 0 ; i < hiddenLayer1.size() ; i++)
        for(unsigned int j = 0 ; j < hiddenLayer2.size() ; j++)
            weights2[i][j] = (float)(rand()%1001 - 500)/(float)100;

    for(unsigned int i = 0 ; i < hiddenLayer2.size() ; i++)
        for(unsigned int j = 0 ; j < outputLayer.size() ; j++)
            weights3[i][j] = (float)(rand()%1001 - 500)/(float)100;
}

Snake::Snake(){
    snake_color = {30,80,200};
    DIRECTION = RIGHT;
    DEAD = false;

    snake_rect = food_rect = {0,0,SNAKE_WIDTH,SNAKE_WIDTH};
    setNN();
}

void Snake::setup(Game* g , int length_){
    init_lenght = length_; length = length_; game_pointer = g;

    block_value = new float*[ game_pointer->getGridW() ];
    for(int i = 0 ; i < game_pointer->getGridW() ; i++){
        block_value[i] = new float[ game_pointer->getGridH() ];
    }

    for(int i = 0 ; i < game_pointer->getGridW() ; i++)
        for(int j = 0 ; j < game_pointer->getGridH() ; j++){
            block_value[i][j] = -2;
        }

    part = new SDL_Point[game_pointer->getNoOfBlocks()];
    previous_move = new dir[game_pointer->getNoOfBlocks()];
    latest_move = new dir[game_pointer->getNoOfBlocks()];
    for(int i=0 ; i < game_pointer->getNoOfBlocks() ; i++){
        part[i] = {0,0};
        previous_move[i] = NONE;
        latest_move[i] = NONE;
    }

    for(int i=0 ; i < length ; i++){
        part[i] = {game_pointer->getFrameRect()->x + 1 + S_MARGIN + (game_pointer->getGridW()/2-i)*(SNAKE_WIDTH + S_MARGIN) , game_pointer->getFrameRect()->y + 1 + S_MARGIN + (game_pointer->getGridH()/2)*(SNAKE_WIDTH + S_MARGIN)};

        if(part[i].x >= game_pointer->getFrameRect()->x + 1)
            block_value[getBlockX(part[i])][getBlockY(part[i])] = i;

        previous_move[i] = DIRECTION;
        latest_move[i] = DIRECTION;
    }
    neuronSurf = IMG_Load("neuron.png");
    SDL_SetColorKey(neuronSurf , SDL_TRUE , SDL_MapRGB(neuronSurf->format , 0 , 0 , 0));
    neuronTexture = SDL_CreateTextureFromSurface(game_pointer->getRenderer() , neuronSurf);
    SDL_FreeSurface(neuronSurf);
    neuronSurf = 0;

    make_food();
}

void Snake::make_food(){
    do{
        food_position.x = rand()%game_pointer->getGridW();
        food_position.y = rand()%game_pointer->getGridH();

        /*foodindex++;
        if( foodindex == game_pointer->getNoOfBlocks() ) foodindex = 0;
        food_position = game_pointer->getFoodPos( foodindex );*/

    }while(block_value[food_position.x][food_position.y] != -2);

    food_rect.x = game_pointer->getFrameRect()->x + 1 + S_MARGIN + food_position.x*(SNAKE_WIDTH + S_MARGIN);
    food_rect.y = game_pointer->getFrameRect()->y + 1 + S_MARGIN + food_position.y*(SNAKE_WIDTH + S_MARGIN);
}

int Snake::update(){
    if(DEAD || win) return 0;
    static SDL_Point tail;
    steps++;
    updateNN();

    tail.x = getBlockX(part[length-1]);
    tail.y = getBlockY(part[length-1]);

    move_();

    if(detectCollision()){
        calculateFitness();
        DEAD = true;
        game_pointer->decrementRemaining();
        return 0;
    }

    if(points) points--;
    if(lifespan) lifespan--;

    if(!lifespan){
        DEAD = 1;
        game_pointer->decrementRemaining();
        calculateFitness();
        return 0;
    }

    block_value[tail.x][tail.y] = -2;
    if( block_value[getBlockX(part[0])][getBlockY(part[0])] >= 1 ){
        calculateFitness();
        DEAD = 1;
        game_pointer->decrementRemaining();
        return 0;
    }

    else
        for(int i = 0 ; i < length ; i++)
            block_value[ getBlockX(part[i])][getBlockY(part[i])] = i;


    if(food_found()) grow();

    return 1;
}

void Snake::render(SDL_Renderer* renderer){
    for(int i = 0 ; i < length ; i++){

    if(i <= 1){
        if(score != game_pointer->getHighestScore())
            SDL_SetRenderDrawColor(renderer , snake_color.r - (i?0:30) , snake_color.g - (i?0:30) , snake_color.b - (i?0:30) , 255);
        else
            SDL_SetRenderDrawColor(renderer , (i?255:200) , (i?165:110) , 0 , 255);
    }

            if( getBlockY(part[i]) + 1 < game_pointer->getGridH() && !DEAD ){
                if( block_value[ getBlockX(part[i]) ][ getBlockY(part[i]) + 1 ] == i+1
                   || block_value[ getBlockX(part[i]) ][ getBlockY(part[i]) + 1 ] == i-1 )
                   snake_rect.h = SNAKE_WIDTH + S_MARGIN;
                else snake_rect.h = SNAKE_WIDTH;
            }
            else snake_rect.h = SNAKE_WIDTH;

            if( getBlockX(part[i]) + 1 < game_pointer->getGridW() && !DEAD ){
                if( block_value[ getBlockX(part[i]) + 1 ][ getBlockY(part[i]) ] == i+1
                   || block_value[ getBlockX(part[i]) + 1 ][ getBlockY(part[i]) ] == i-1 )
                   snake_rect.w = SNAKE_WIDTH + S_MARGIN;
                else snake_rect.w = SNAKE_WIDTH;
            }
            else snake_rect.w = SNAKE_WIDTH;


        snake_rect.x = part[i].x;
        snake_rect.y = part[i].y;
        SDL_RenderFillRect(renderer , &snake_rect);
    }
}

void Snake::renderFood(SDL_Renderer* renderer){
    SDL_RenderFillRect(renderer , &food_rect);
}

bool Snake::detectCollision(){
    if(part[0].x + SNAKE_WIDTH > game_pointer->getFrameRect()->x + game_pointer->getFrameRect()->w) return 1;
    if(part[0].x < game_pointer->getFrameRect()->x) return 1;
    if(part[0].y < game_pointer->getFrameRect()->y) return 1;
    if(part[0].y + SNAKE_WIDTH > game_pointer->getFrameRect()->y + game_pointer->getFrameRect()->h) return 1;

    return DEAD;
}

bool Snake::food_found(){
    return part[0].x == food_rect.x && part[0].y == food_rect.y;
}

void Snake::grow(){
    static int x,y;
    if(latest_move[length-1 /*tail*/] == RIGHT || latest_move[length-1] == LEFT){
        x = part[length-1].x + ( latest_move[length-1] == RIGHT?-(SNAKE_WIDTH + S_MARGIN):(SNAKE_WIDTH + S_MARGIN) );
        y = part[length-1].y;
    }
    else{
        y = part[length-1].y + (latest_move[length-1] == DOWN?-(SNAKE_WIDTH + S_MARGIN):(SNAKE_WIDTH + S_MARGIN));
        x = part[length-1].x;
    }

    part[length] = {x,y};
    ++length;
    score++;

    lifespan = 200 + 2*game_pointer->getHighestScore();
    points += 100 + score*1.2;
    if(points > 50*score) points = score*50;

    if(length < game_pointer->getNoOfBlocks()) make_food();
    else{
        win = 1;
        game_pointer->won();
    }
}

void Snake::move_(){
    (DIRECTION==RIGHT || DIRECTION==LEFT?part[0].x:part[0].y) += (DIRECTION==RIGHT || DIRECTION==DOWN?SPEED:-SPEED);
    for(int i=1 ; i < length ; i++){
        ( (previous_move[i-1]==RIGHT || previous_move[i-1]==LEFT)?part[i].x:part[i].y )
        += ( (previous_move[i-1]==RIGHT || previous_move[i-1]==DOWN)?SPEED:-SPEED );
            previous_move[i] = latest_move[i];
            latest_move[i] = previous_move[i-1];
    }
}

#endif // SNAKE_H_INCLUDED
