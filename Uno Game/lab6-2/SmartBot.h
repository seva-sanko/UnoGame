#pragma once
#include <vector>
#include "prod/src/game/uno_game.h"
using std::string;
using std::vector;

class SmartBot : public UnoPlayer
{
    string plname;
    int ctr_h;
    vector<const Card*> Hand;
    vector <unsigned int> clrs;
    const Card* LastCard;
    CardColor selectedColor;
    vector<const Card*> discardPile; // Карты, уходящие в сброс
    vector<const Card*> cardsInDiscard; // Карты, запомненные в сбросе

public:
    SmartBot(const char* name);

    string name() const override;

    void receiveCards(const vector<const Card*>& cards);

    const Card* playCard() override;

    bool isMatchingCard(const Card* card) const;

    void playMatchingCard(const Card* card);

    bool drawAdditionalCard(const Card* additionalCard) override;

    CardColor changeColor() override;

    void handleSetStarted(int gameNumber);

    void rememberDiscardedCards(const vector<const Card*>& cards);

    const Card* findCardInHandWithSameValue(const Card* card) const;

    const Card* findCardInHandWithSameColor(const Card* card) const;
};