# UnoGame

C++ OOP lab — a fully engineered Uno card game engine with a pluggable bot interface, Observer-based event system, and a `SmartBot` AI. The goal of the project is to implement a "smart bot" that competes against other bots at the table.

## Architecture

```
Observer (17 events)
  └── UnoPlayer (abstract)
        ├── UnoBot       — baseline bot
        └── SmartBot     — intelligent bot with discard memory

Broadcaster<iterator> (template fan-out)
  └── UnoGame::EventBroadcaster
        └── UnoGame
```

**Design patterns used:**
- **Observer** — all game events fan out to registered observers via `Broadcaster<iterator>`
- **Strategy** — `UnoPlayer` defines the player interface; any subclass can implement its own tactics
- **Message Queue** — players send chat messages via `MessageQueue`; queue is flushed after each event

---

## Core Classes

### `Card`

```cpp
enum CardColor  { Red=0, Green=1, Blue=2, Yellow=3 };
enum CardValue  { Draw2=10, Reverse=11, Skip=12, Wild=13, WildDraw4=14 };
```

Scoring: number cards = face value, action cards = 20, wild cards = 50.

### `UnoPlayer` (abstract base)

| Method | Description |
|--------|-------------|
| `name()` | Player's display name |
| `receiveCards(cards)` | Called when cards are dealt |
| `playCard()` | Return the card to place on the discard pile |
| `drawAdditionalCard(card)` | Decide to play or keep the drawn card |
| `changeColor()` | Choose new color after Wild/Wild+4 |

Inherits `Observer` — override any of the 17 event handlers to react to game state.

Protected interface for sending messages (`say()`) and reading game state (`game()`).

### `UnoGame`

| Constant | Value |
|----------|-------|
| `WINNING_SCORE` | 500 |
| `DECK_SIZE` | 108 |
| `INITIAL_CARDS_NUMBER` | 7 (dealt per player at set start) |
| `MIN/MAX_NUMBER_OF_PLAYERS` | 2–10 |
| `DEFAULT_TURNS_LIMIT` | 100 000 turns/set |
| `DEFAULT_SETS_LIMIT` | 1 000 sets/game |
| `DEFAULT_MESSAGE_QUEUE_LIMIT` | 50 messages |

Key methods:
- `runSet()` → `tuple<winner_index, score>` — plays one hand; returns `(-1, 0)` if turn limit reached
- `runGame()` → `tuple<winner_index, total_score>` — plays until someone reaches 500 points
- `addPlayer(player)`, `addObserver(observer)`, `shufflePlayers()`
- `setTurnsLimit()`, `setSetsLimit()`, `setMessageQueueSizeLimit()`, `setRandomGeneratorSeed()`

Overridable for testing:
- `chooseCards(player, n)` — control which cards are dealt
- `chooseFirstCard()` — control the first discard card

RNG: `std::minstd_rand` (seeded, reproducible).

### 17 Game Events (`Observer` interface)

| # | Event | Trigger |
|---|-------|---------|
| 1 | `handlePlayerEntered` | Player added to game |
| 2 | `handleSetStarted` | New hand begins |
| 3 | `handleDeckShuffled` | Deck reshuffled from discard |
| 4 | `handleFirstCardPlaced` | Opening card placed |
| 5 | `handlePlayerDealt` | Cards dealt to a player |
| 6 | `handleCardPlayed` | Player placed a card |
| 7 | `handlePlayerDrewAnotherCard` | Player drew extra card |
| 8 | `handlePlayerDrewAndSkip` | Player drew N cards and was skipped |
| 9 | `handlePlayerChangedColor` | Color changed after Wild |
| 10 | `handlePlayerSaid` | Player sent a chat message |
| 11 | `handlePlayerDisqualified` | Player played an illegal card |
| 12 | `handlePlayerWonSet` | Player won the hand |
| 13 | `handlePlayerWonGame` | Player reached 500 points |
| 14 | `handleDirectionChanged` | Reverse card played |
| 15 | `handleMessageOverflow` | Message queue overflowed |
| 16 | `handleTurnsLimitReached` | Turn limit hit — draw |
| 17 | `handleSetsLimitReached` | Set limit hit |

After every event the `EventBroadcaster` flushes the `MessageQueue`, calling `handlePlayerSaid` for each queued message.

### `MessageQueue`

FIFO queue of `(playerIndex, message)` tuples. Configurable capacity (default 50); overflow flag triggers `handleMessageOverflow`. Used so bots can "speak" without reentrancy issues.

---

## Bots

### `UnoBot` — baseline bot

Simple greedy player: plays the first matching card found by color or value; if no match, draws. Chooses color by hand frequency (`clrs` histogram).

### `SmartBot` — smart bot

Extends `UnoBot` with:
- **Discard memory** — tracks all cards that have gone to the discard pile via `rememberDiscardedCards()` (observer hook on `handleSetStarted`)
- **Same-value search** — `findCardInHandWithSameValue(card)` — plays a matching value card of a different color to avoid getting stuck
- **Same-color search** — `findCardInHandWithSameColor(card)`
- **Color selection** — `changeColor()` picks the color most represented in hand from `clrs`
- **Draw policy** — `drawAdditionalCard()` plays the drawn card immediately if it matches

---

## Utilities

| Module | Description |
|--------|-------------|
| `logger.h/.cpp` | Console/file game log (observer) |
| `stats.h/.cpp` | Per-game statistics collector (observer) |

---

## Documentation (HTML)

Pre-generated docs in `prod/docs/`:

| File | Content |
|------|---------|
| `rules.html` | Official Uno rules as implemented |
| `algorithm.html` | `runSet()` / `runGame()` pseudocode |
| `model.html` | Class diagram and relationships |
| `changelog.html` | Version history |
| `faq.html` | Common questions |

---

## Files

| Path | Description |
|------|-------------|
| `prod/src/game/card.h` | `Card`, `CardColor`, `CardValue` |
| `prod/src/game/events.h` | `Observer`, `Broadcaster<T>`, `GameDirection`, 17 events |
| `prod/src/game/uno_game.h/.cpp` | `UnoGame`, `UnoPlayer`, `EventBroadcaster` |
| `prod/src/game/game_components.h/.cpp` | `MessageQueue` |
| `UnoBot.h/.cpp` | Baseline bot |
| `SmartBot.h/.cpp` | Smart bot with discard memory |
| `prod/src/utils/logger.h/.cpp` | Logger observer |
| `prod/src/utils/stats.h/.cpp` | Stats observer |
| `Uno.pdf` | Lab assignment |
| `UnoAnother.pdf` | Extended report |

## Build

```
Visual Studio — open lab6-2.sln
Requires: C++17
```
