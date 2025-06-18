#pragma once
//#include <iostream>
#include <string>
#include <vector>
#include "prod/src/game/uno_game.h"
//#include "prod/src/utils/stats.h"
//#include "prod/src/utils/logger.h"
//using namespace std;
using std::string;
using std::vector;
using std::cout;
using std::endl;


class UnoBot : public UnoPlayer
{
    string plname;
    int ctr_h;
    vector<const Card*> Hand;
    vector <unsigned int> clrs;
    const Card* LastCard;
    CardColor selectedColor;

public:

    void f();

    UnoBot(const char* name);

    string name() const override;

    void receiveCards(const vector<const Card*>& cards);/// @brief Игрок получает на руки карты. @param cards массив карт.

    const Card* playCard() override;/// @brief Игрок возвращает карту, которую он сыграет (положит в сброс).@return карта, которую игрок положит в сброс.

    bool isMatchingCard(const Card* card) const;

    void playMatchingCard(const Card* card);

    bool drawAdditionalCard(const Card* additionalCard) override; /// @brief Если у игрока нет подходящих карт, он тянет дополнительную карту и говорит, хочет ли он ее сразу положить или нет.@param additionalCard дополнительная карта. @return true, если игрок сразу же кладет эту карту, иначе false.

    CardColor changeColor() override;/// @brief Если игрок положил "Закажи цвет" или "Возьми четыре", то игра запросит у него новый цвет. @return новый цвет.

    void handleSetStarted(int gameNumber);
};

void UnoBot::f() { std::cout << 5; }