#ifndef CLASSES_H_INCLUDED
#define CLASSES_H_INCLUDED

const bool isolateError = 0;

class Snake;
class Game{
public:
    Game(const int X_BLOCKS , const int Y_BLOCKS , const int Z);
    ~Game();

    void handleEvent();
    void update();
    void render();

    void won(){finished = 1;};
    void upload();
    void linkSnake(Snake *s){snake_pointer[population] = s; population++;};
    void renderText(const char *s , int x , int y , TTF_Font *font , Uint8 r , Uint8 g , Uint8 b);

    void selectParents();
    void crossover(int parent1 , int parent2);
    void clone(int best , int from , int till);
    void mutate();
    void decrementRemaining(){remaining--;};
    void reset();

    void setCrossover(bool on){crossoverON = on;};
    void pauseWhenWon(bool on){pause_when_won = on;};

    void getAverage();
    int getBestGen(){return bestGen;};
    int getFitness(){return bestFitness;};
    int getHighestScore(){return highScore;};
    int getGeneration(){return generation;};
    int getNoOfBlocks(){return noOfBlocks;};
    int getGridW(){return gridWidth;};
    int getGridH(){return gridHeight;};
    SDL_Point getFoodPos(int n){return foodPos[n];};

    bool Quit(){return quit;};
    bool getFin(){return finished;};
    bool allDone();

    SDL_Renderer* getRenderer(){return renderer;};
    SDL_Rect* getFrameRect(){return &frame;};


private:
    Snake *snake_pointer[POPULATION] = { 0 };
    static const int PADDING_UP = 0 , PADDING_LEFT = 450 , PADDING_RIGHT = 0 , PADDING_DOWN = 0;

    SDL_Point *foodPos = NULL;
    int fps_multi = 1 , crnt_highscore = 0 , shownSnake = 0 , bestGen  = 0 , bestFitness = 0 , father = 0 , mother = 0 , child = 0 , highScore = 0;
    int gridWidth , gridHeight , WINDOW_WIDTH , WINDOW_HEIGHT , noOfBlocks , population = 0 , generation = 1 , remaining = 0;
    float mutation_rate = 0 , average = 0 , prevArvg = 0;
    bool pause_when_won = 1 , finished = 0 , crossoverON = 1 , grid_shown = 0 , quit = 0 , once[5] = {1,1,1,1,1};
    bool food_fixed = false;
    std::stringstream textSString;
    std::ifstream weightsFile;

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Surface *textSurf = NULL;
    SDL_Texture *textTexture = NULL;
    TTF_Font *mFont = NULL , *ffont = NULL;
    Mix_Music *music = NULL;
    Mix_Chunk *newRecSF = NULL;

    SDL_Color background_color , food_color , grid_color;
    SDL_Rect frame , textRect;
    SDL_Event e;

    void checkIfCrntBest(int i);
    void checkIfHighestScore(int i);
    void switchShownSnake();
    void killOffIfBad(int i);
    void displayStatistics();
    void renderSnakes();
    void renderFood();
    void restart();
    void showBest();
    void renderGrid();
};

class Snake{
public:
    Snake();
    ~Snake();
    void setup(Game *g , int length_);
    friend class Game;

private:
    Game* game_pointer = NULL;
    enum dir{NONE , UP , RIGHT , DOWN , LEFT}DIRECTION , *previous_move , *latest_move;

    static const int SNAKE_WIDTH = snakeWidth, SPEED = snakeWidth + S_MARGIN;
    int foodindex = -1 , fitness = 0;
    int length , steps = 0 , score = 0 , foodNo = 0 , lifespan = 200 , points = 0 , init_lenght;

    bool win = 0 , DEAD = 0 , mutated = 0;

    float **block_value = NULL;

    std::vector<float> inputLayer;
    float **weights1 = NULL;
    std::vector<float> hiddenLayer1;
    float **weights2 = NULL;
    std::vector<float> hiddenLayer2;
    float **weights3 = NULL;
    std::vector<float> outputLayer;
    std::ofstream weightsOutputFile;

    SDL_Rect neoronRect;
    SDL_Surface *neuronSurf = NULL;
    SDL_Texture *neuronTexture = NULL;
    SDL_Point *part, food_position , new_part;
    SDL_Rect snake_rect , food_rect;
    static SDL_Color snake_color;

    void setNN();
    void resetNN();
    void updateNN();
    void renderNN();
    void saveNN(int snakeINdex);
    void make_food();
    void renderFood(SDL_Renderer* renderer);
    void mutate(float rate);
    void calculateFitness();
    void move_();
    void reset();
    void handleEvent(SDL_Event e);
    void grow();
    void render(SDL_Renderer* renderer);

    int getBlockX(SDL_Point p){return (p.x - game_pointer->getFrameRect()->x - 1 - S_MARGIN )/(SNAKE_WIDTH + S_MARGIN);};
    int getBlockY(SDL_Point p){return (p.y - game_pointer->getFrameRect()->y - 1 - S_MARGIN )/(SNAKE_WIDTH + S_MARGIN);};
    int update();

    bool detectCollision();
    bool food_found();
};

SDL_Color Snake::snake_color;

#endif // CLASSES_H_INCLUDED
