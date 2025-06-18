#include "SmartBot.h"
#include "UnoBot.h"
#include <cstdlib>
#include "prod/src/game/uno_game.h"
#include "prod/src/utils/stats.h"
#include "prod/src/utils/logger.h"

void statisticTest();

int main()
{
    //statisticTest();
    // Ваш main должен выглядеть примерно так
    //srand(time(nullptr));
    UnoGame game;
    Logger logger;
    UnoBot player1("Asya");//, player2("Kolya");
    SmartBot player2("Seva");
    game.addPlayer(&player1);
    game.addPlayer(&player2);
    game.addObserver(&logger);
    game.runGame();
    statisticTest();
}

void statisticTest()
{
    UnoGame game;
    UnoBot player1("Asya");
    SmartBot player2("Seva");
    game.addPlayer(&player1);
    game.addPlayer(&player2);
    StatsObserver result = runGames(game, 100);
    StatsObserver::MV winMV = result.getWinsMV();
    std::cout << "Seva won in " << winMV.mean[1] * 100 << "% of games" << std::endl;
    
}
