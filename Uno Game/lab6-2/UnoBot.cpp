#include "UnoBot.h"

UnoBot::UnoBot(const char* name) {
	plname = name;
	ctr_h = 0; // Инициализация счетчика
	clrs.resize(6, 0); // Инициализация размера и заполнение нулями
	//Функция resize вектора используется для изменения размера вектора. Она принимает два аргумента: новый 
	//размер вектора и значение, которым заполняются новые элементы, если новый размер больше текущего.
}

string UnoBot::name() const {
	return plname;
}

void UnoBot::handleSetStarted(int gameNumber) {
	clrs.clear();
	for (int i = 0; i < 6; i++) {
		clrs.push_back(0);
	}
	Hand.clear();
	ctr_h = 0;
	selectedColor = changeColor();
}

void UnoBot::receiveCards(const vector<const Card*>& cards) {
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

const Card* UnoBot::playCard() {
	CardColor currentColor = game()->currentColor();
	const Card* topCard = game()->topCard();

	if (topCard == nullptr) {
		LastCard = nullptr; // Устанавливаем LastCard в nullptr, если верхней карты нет
		return nullptr; // Если верхней карты нет, возвращаем nullptr
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


	LastCard = nullptr; // Устанавливаем LastCard в nullptr, если не найдено подходящей карты
	return nullptr; // Если не найдено подходящей карты, возвращаем nullptr
}

bool UnoBot::isMatchingCard(const Card* card) const {//функция проверяет подходит ли набранная карта
	if (card->color == game()->currentColor() ||
		card->value == Wild ||
		card->value == WildDraw4 ||
		card->value == game()->topCard()->value) {
		return true;
	}
	return false;
}

void UnoBot::playMatchingCard(const Card* card) {//если да мы играем этой картой сразу
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

bool UnoBot::drawAdditionalCard(const Card* additionalCard) {
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

CardColor UnoBot::changeColor() {
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

	vector<CardColor> leastCommonColors; // Список цветов с наименьшим количеством карт
	int leastCount = INT_MAX; // Начальное значение - максимальное целое число

	for (int i = Red; i <= Yellow; i++) {
		if (colorCount[i] < leastCount && colorCount[i] > 0) { // Исключаем цвета с нулевым количеством карт
			leastCount = colorCount[i];
			leastCommonColors.clear();
			leastCommonColors.push_back(static_cast<CardColor>(i));
		}
		else if (colorCount[i] == leastCount && colorCount[i] > 0) { // Добавляем цвета с равным наименьшим количеством карт
			leastCommonColors.push_back(static_cast<CardColor>(i));
		}
	}

	if (leastCommonColors.empty()) {
		// Если нет цветов с ненулевым количеством карт, вернуть случайный цвет
		return static_cast<CardColor>(rand() % 4);
	}
	else {
		// Из списка наименьших цветов выбрать случайный
		int randomIndex = rand() % leastCommonColors.size();
		return leastCommonColors[randomIndex];
	}
}

