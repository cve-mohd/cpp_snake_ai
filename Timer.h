#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED

class Timer{
    Uint32 start_ticks;
    bool isStarted;
    Uint32 FRAMES_RATE;

public:
    getFR(){return FRAMES_RATE;};
    Timer(Uint32 x);
    void start();
    void stop();
    bool is_started(){return isStarted;};
    Uint32 get_current_ticks(){return SDL_GetTicks() - start_ticks;};
    friend class Game;
};

Timer::Timer(Uint32 x){
    FRAMES_RATE = x;
    start_ticks = 0;
    isStarted = false;
}

void Timer::start(){
    isStarted = true;
    start_ticks = SDL_GetTicks();
}

void Timer::stop(){
    if(get_current_ticks() < 1000/FRAMES_RATE)
        SDL_Delay(1000/FRAMES_RATE - get_current_ticks());
    isStarted = false;
    start_ticks = 0;
}


#endif // TIMER_H_INCLUDED
