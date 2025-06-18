#include "SmartBot.h"

SmartBot::SmartBot(const char* name) : plname(name), ctr_h(0), LastCard(nullptr) {}

string SmartBot::name() const {
    return plname;
}

void SmartBot::receiveCards(const vector<const Card*>& cards) {
    const Card* topCard = game()->topCard();

    for (int i = 0; i < cards.size(); i++) {
        Hand.push_back(cards[i]);
        ctr_h++;

        switch (cards[i]->color) {
        case Red:
            clrs[0]++;
            break;
        case Green:
            clrs[1]++;
            break;
        case Blue:
            clrs[2]++;
            break;
        case Yellow:
            clrs[3]++;
            break;
        }

        switch (cards[i]->value) {
        case Wild:
            if (topCard != nullptr) {
                clrs[topCard->color]--;  // Замена цвета последней сыгранной карты
            }
            clrs[4]++;  // Увеличение количества карт Wild
            break;
        case WildDraw4:
            if (topCard != nullptr) {
                clrs[topCard->color]--;  // Замена цвета последней сыгранной карты
            }
            clrs[5]++;  // Увеличение количества карт WildDraw4
            break;
        }
    }

    if (topCard == nullptr) {
        selectedColor = changeColor();  // Установка желаемого цвета, если topCard равен nullptr
    }
}

const Card* SmartBot::playCard() {
    CardColor currentColor = game()->currentColor();
    const Card* topCard = game()->topCard();

    if (topCard == nullptr) {
        LastCard = nullptr; // Устанавливаем LastCard в nullptr, если верхней карты нет
        return nullptr; // Если верхней карты нет, возвращаем nullptr
    }

    if (cardsInDiscard.size() >= 2)
    {
        for (const Card* discardCard : cardsInDiscard)
        {
            const Card* matchingCard = findCardInHandWithSameValue(discardCard);
            if (matchingCard != nullptr)
            {
                Hand.erase(std::find(Hand.begin(), Hand.end(), matchingCard));
                ctr_h--;
                LastCard = matchingCard;
                return matchingCard;
            }
        }

        for (const Card* discardCard : cardsInDiscard)
        {
            const Card* matchingCard = findCardInHandWithSameColor(discardCard);
            if (matchingCard != nullptr)
            {
                Hand.erase(std::find(Hand.begin(), Hand.end(), matchingCard));
                ctr_h--;
                LastCard = matchingCard;
                return matchingCard;
            }
        }
    }

    for (int i = 0; i < ctr_h; i++) {
        const Card* card = Hand[i];

        if (isMatchingCard(card)) {
            if (card->value == 14 && clrs[currentColor] != 0) {
                continue;
            }
            
            else {
                Hand.erase(Hand.begin() + i);
                ctr_h--;

                switch (card->color) {
                case Red:
                    clrs[0]--;
                    break;
                case Green:
                    clrs[1]--;
                    break;
                case Blue:
                    clrs[2]--;
                    break;
                case Yellow:
                    clrs[3]--;
                    break;
                }

                switch (card->value) {
                case 13:
                    clrs[4]--;
                    clrs[game()->currentColor()]++;
                    break;
                case 14:
                    clrs[5]--;
                    clrs[game()->currentColor()]++;
                    break;
                }

                LastCard = card;
                return card;
            }
        }
    }
    // Если не найдено подходящей карты, взять карту из колоды
    
    if (drawAdditionalCard(topCard) == true)
    {
        LastCard = nullptr;
        return nullptr;
    }

    LastCard = nullptr; // Устанавливаем LastCard в nullptr, если не найдено подходящей карты
    return nullptr; // Если не найдено подходящей карты, возвращаем nullptr // Если нет подходящих карт, вернуть nullptr
}

bool SmartBot::isMatchingCard(const Card* card) const {
    if (card->color == game()->currentColor() ||
        card->value == Wild ||
        card->value == WildDraw4 ||
        card->value == game()->topCard()->value) {
        return true;
    }
    // Проверяем, совпадает ли цвет или ранг карты с последней сброшенной картой
    return false;
}

void SmartBot::playMatchingCard(const Card* card) {
    for (int i = 0; i < ctr_h; i++) {
		if (Hand[i] == card) {
			switch (card->color) {
			case Red:
				clrs[0]--;
				break;
			case Green:
				clrs[1]--;
				break;
			case Blue:
				clrs[2]--;
				break;
			case Yellow:
				clrs[3]--;
				break;
			}

			switch (card->value) {
			case Wild:
				clrs[4]--;
				clrs[game()->currentColor()]++;
				break;
			case WildDraw4:
				clrs[5]--;
				clrs[game()->currentColor()]++;
				break;
			}

			Hand.erase(Hand.begin() + i);
			ctr_h--;
			LastCard = card;
			break;
		}
	}
}

bool SmartBot::drawAdditionalCard(const Card* additionalCard) {
    if (isMatchingCard(additionalCard)) {
        // Если полученная карта подходит, сразу же сыгрываем ее
        playMatchingCard(additionalCard);
        return true;
    }
    else {
        // Если полученная карта не подходит, добавляем ее в руку
        Hand.push_back(additionalCard);
        ctr_h++;

        return false;
    }
}

CardColor SmartBot::changeColor() {
    vector<int> colorCount(4, 0); // Индексы 0-3 соответствуют Red, Green, Blue, Yellow

    for (const auto& card : Hand) {
        if (card->color == Red)
            colorCount[Red]++;
        else if (card->color == Green)
            colorCount[Green]++;
        else if (card->color == Blue)
            colorCount[Blue]++;
        else if (card->color == Yellow)
            colorCount[Yellow]++;
    }

    CardColor leastCommonColor; // Начальное значение - отсутствие цвета
    int leastCount = INT_MIN; // Начальное значение - максимальное целое число

    for (int i = Red; i <= Yellow; i++) {
        if (colorCount[i] > leastCount) {
            leastCount = colorCount[i];
            leastCommonColor = static_cast<CardColor>(i);
        }
    }

    return leastCommonColor;
}

void SmartBot::handleSetStarted(int gameNumber) {
    clrs.clear();
    for (int i = 0; i < 6; i++) {
        clrs.push_back(0);
    }
    Hand.clear();
    ctr_h = 0;
    selectedColor = changeColor();
}



void SmartBot::rememberDiscardedCards(const vector<const Card*>& cards)
{
    // Запомните карты, отправляемые в сброс
    for (const Card* card : cards)
    {
        cardsInDiscard.push_back(card);
    }
}

const Card* SmartBot::findCardInHandWithSameValue(const Card* card) const
{
    // Находим карту с тем же значением в руке
    for (const Card* handCard : Hand)
    {
        if (handCard->value == card->value)
        {
            return handCard;
        }
    }
    return nullptr;
}

const Card* SmartBot::findCardInHandWithSameColor(const Card* card) const
{
    // Находим карту с тем же цветом в руке
    for (const Card* handCard : Hand)
    {
        if (handCard->color == card->color)
        {
            return handCard;
        }
    }
    return nullptr;
}