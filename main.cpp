#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <vector>
#include <random>
#include <ctime>

using namespace std;

int drawCard();
int calculateHandValue(const vector<int>& hand);
void handlePlayerChoice(bool hit, vector<int>& playerHand, vector<int>& dealerHand, sf::Text& resultText, sf::Text& pointsText, int& points, int bet, sf::Text& playerHandText, sf::Text& dealerHandText, bool& playerTurn, bool& gameEnded, bool& dealerCardRevealed);
bool isValidBet(int bet, int points);

int main()
{
    sf::RenderWindow welcomeWindow(sf::VideoMode(800, 600), "Welcome Window");
    sf::RenderWindow window;

    // Load a font
    sf::Font font;
    if (!font.loadFromFile("font.ttf")) {
        std::cerr << "Error loading font\n";
        return 1;
    }

    // Create text objects for the welcome window
    sf::Text welcomeText("Enter Chip Count: ", font, 30);
    welcomeText.setPosition(250, 200);
    welcomeText.setFillColor(sf::Color::Black);

    sf::Text chipCountText("", font, 30);
    chipCountText.setPosition(350, 250);
    chipCountText.setFillColor(sf::Color::Black);

    // Create text objects for the game window
    sf::Text instructionText("Hit or Stay:", font, 30);
    instructionText.setPosition(50, 50);
    instructionText.setFillColor(sf::Color::Black);

    sf::Text resultText("", font, 30);
    resultText.setPosition(50, 150);
    resultText.setFillColor(sf::Color::Red);

    sf::Text playerHandText("Player Hand: ", font, 30);
    playerHandText.setPosition(50, 250);
    playerHandText.setFillColor(sf::Color::Black);

    sf::Text dealerHandText("Dealer Hand: ", font, 30);
    dealerHandText.setPosition(50, 350);
    dealerHandText.setFillColor(sf::Color::Black);

    sf::Text pointsText("", font, 30);
    pointsText.setPosition(50, 500);
    pointsText.setFillColor(sf::Color::Black);

    sf::Text betText("Enter Bet: ", font, 30);
    betText.setPosition(50, 550);
    betText.setFillColor(sf::Color::Black);

    sf::Text currentBetText("", font, 30);
    currentBetText.setPosition(250, 550);
    currentBetText.setFillColor(sf::Color::Black);

    // Create buttons for Hit and Stay
    sf::RectangleShape hitButton(sf::Vector2f(150, 50));
    hitButton.setFillColor(sf::Color::Green);
    hitButton.setPosition(50, 100);

    sf::Text hitButtonText("Hit", font, 24);
    hitButtonText.setFillColor(sf::Color::White);
    hitButtonText.setPosition(100, 110);

    sf::RectangleShape stayButton(sf::Vector2f(150, 50));
    stayButton.setFillColor(sf::Color::Blue);
    stayButton.setPosition(300, 100);

    sf::Text stayButtonText("Stay", font, 24);
    stayButtonText.setFillColor(sf::Color::White);
    stayButtonText.setPosition(350, 110);

    // Create "Cash Out" button
    sf::RectangleShape cashOutButton(sf::Vector2f(150, 50));
    cashOutButton.setFillColor(sf::Color::Red);
    cashOutButton.setPosition(400, 500);

    sf::Text cashOutText("Cash Out", font, 24);
    cashOutText.setFillColor(sf::Color::White);
    cashOutText.setPosition(420, 510);

    // Create "Deal Again" button
    sf::RectangleShape dealAgainButton(sf::Vector2f(180, 50));
    dealAgainButton.setFillColor(sf::Color::Cyan);
    dealAgainButton.setPosition(600, 500);

    sf::Text dealAgainText("Deal Again", font, 24);
    dealAgainText.setFillColor(sf::Color::White);
    dealAgainText.setPosition(620, 510);

    // Random number generator for drawing cards
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Game variables
    int points = 0;
    bool gameEnded = false;
    bool playerTurn = true;
    bool dealerCardRevealed = false;
    bool betPlaced = false;
    int bet = 0;

    vector<int> playerHand;
    vector<int> dealerHand;

    // Cash Out Window variables
    sf::RenderWindow cashOutWindow;
    bool cashOutWindowOpen = false;

    sf::Text cashOutWindowText;
    sf::RectangleShape playAgainButton(sf::Vector2f(200, 100));
    sf::Text playAgainText;

    auto setupCashOutWindow = [&]() {
        cashOutWindow.create(sf::VideoMode(400, 300), "Cash Out");
        cashOutWindowText.setString("Cash Out: " + to_string(points));
        cashOutWindowText.setFont(font);
        cashOutWindowText.setFillColor(sf::Color::Black);
        cashOutWindowText.setPosition(100, 50);

        playAgainButton.setFillColor(sf::Color::Blue);
        playAgainButton.setPosition(100, 150);

        playAgainText.setFont(font);
        playAgainText.setString("Play Again");
        playAgainText.setFillColor(sf::Color::White);
        playAgainText.setPosition(120, 170);
    };

    // Function to reset the game state
    auto resetGame = [&]() {
        playerHand.clear();
        dealerHand.clear();
        playerHand.push_back(drawCard());
        playerHand.push_back(drawCard());
        dealerHand.push_back(drawCard());
        dealerHand.push_back(drawCard());
        playerTurn = true;
        gameEnded = false;
        dealerCardRevealed = false;
        betPlaced = false;
        bet = 0;
        currentBetText.setString("");
        resultText.setString("");
        playerHandText.setString("Player Hand: " + to_string(playerHand[0]) + " " + to_string(playerHand[1]));
        dealerHandText.setString("Dealer Hand: " + to_string(dealerHand[0]) + " ?");
    };

    // Welcome window loop
    while (welcomeWindow.isOpen()) {
        sf::Event event;
        while (welcomeWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                welcomeWindow.close();

            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode >= '0' && event.text.unicode <= '9') {
                    chipCountText.setString(chipCountText.getString() + static_cast<char>(event.text.unicode));
                } else if (event.text.unicode == '\b' && !chipCountText.getString().isEmpty()) {
                    std::string currentText = chipCountText.getString();
                    currentText.pop_back();
                    chipCountText.setString(currentText);
                } else if (event.text.unicode == '\r' && !chipCountText.getString().isEmpty()) {
                    std::string currentText = chipCountText.getString();
                    points = stoi(currentText);
                    pointsText.setString("Points: " + to_string(points));
                    welcomeWindow.close();
                    window.create(sf::VideoMode(800,600), "Blackjack Game");
                }
            }
        }

        welcomeWindow.clear(sf::Color::White);
        welcomeWindow.draw(welcomeText);
        welcomeWindow.draw(chipCountText);
        welcomeWindow.display();
    }

    // Deal initial cards after welcome window closes
    resetGame();

    // Main game loop
    while (window.isOpen()) {
        if (cashOutWindowOpen) {
            sf::Event cashOutEvent;
            while (cashOutWindow.pollEvent(cashOutEvent)) {
                if (cashOutEvent.type == sf::Event::Closed)
                    cashOutWindow.close();

                if (cashOutEvent.type == sf::Event::MouseButtonPressed && cashOutEvent.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f mousePos(cashOutEvent.mouseButton.x, cashOutEvent.mouseButton.y);
                    if (playAgainButton.getGlobalBounds().contains(mousePos)) {
                        cashOutWindow.close();
                        cashOutWindowOpen = false;
                        resetGame();
                    }
                }
            }

            cashOutWindow.clear(sf::Color::White);
            cashOutWindow.draw(cashOutWindowText);
            cashOutWindow.draw(playAgainButton);
            cashOutWindow.draw(playAgainText);
            cashOutWindow.display();

        } else {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();

                if (event.type == sf::Event::TextEntered && !betPlaced && !gameEnded) {
                    if (event.text.unicode >= '0' && event.text.unicode <= '9') {
                        currentBetText.setString(currentBetText.getString() + static_cast<char>(event.text.unicode));
                    } else if (event.text.unicode == '\b' && !currentBetText.getString().isEmpty()) {
                        std::string currentText = currentBetText.getString();
                        currentText.pop_back();
                        currentBetText.setString(currentText);
                    } else if (event.text.unicode == '\r' || event.text.unicode == '\n') {
                        string betString = currentBetText.getString();
                        bet = std::stoi(betString);
                        if (isValidBet(bet, points)) {
                            betPlaced = true;
                            points -= bet; // Deduct bet amount from points
                            pointsText.setString("Points: " + to_string(points));
                        } else {
                            currentBetText.setString("Invalid Bet");
                        }
                    }
                }

                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                    if (cashOutButton.getGlobalBounds().contains(mousePos)) {
                        // Handle cash out
                        gameEnded = true;
                        cashOutWindowOpen = true;
                        setupCashOutWindow();
                    } else if (hitButton.getGlobalBounds().contains(mousePos) && !gameEnded && playerTurn && betPlaced) {
                        // Player chooses to hit
                        handlePlayerChoice(true, playerHand, dealerHand, resultText, pointsText, points, bet, playerHandText, dealerHandText, playerTurn, gameEnded, dealerCardRevealed);
                    } else if (stayButton.getGlobalBounds().contains(mousePos) && !gameEnded && playerTurn && betPlaced) {
                        // Player chooses to stay
                        handlePlayerChoice(false, playerHand, dealerHand, resultText, pointsText, points, bet, playerHandText, dealerHandText, playerTurn, gameEnded, dealerCardRevealed);
                    } else if (dealAgainButton.getGlobalBounds().contains(mousePos) && gameEnded) {
                        // Reset the game and deal again
                        resetGame();
                    }
                }
            }

            // Clear the window
            window.clear(sf::Color::White);

            // Draw the game interface
            window.draw(instructionText);
            window.draw(resultText);
            window.draw(playerHandText);
            window.draw(dealerHandText);
            window.draw(pointsText);
            window.draw(hitButton);
            window.draw(hitButtonText);
            window.draw(stayButton);
            window.draw(stayButtonText);
            window.draw(cashOutButton);
            window.draw(cashOutText);
            window.draw(betText);
            window.draw(currentBetText);
            if (gameEnded) {
                window.draw(dealAgainButton);
                window.draw(dealAgainText);
            }

            // Display the content
            window.display();
        }
    }

    return 0;
}

int drawCard()
{
    return std::rand() % 11 + 1; // Random number between 1 and 11
}

int calculateHandValue(const vector<int>& hand)
{
    int value = 0;
    int aces = 0;
    for (int card : hand) {
        if (card == 1) {
            aces++;
            value += 11;
        } else {
            value += card;
        }
    }
    while (value > 21 && aces > 0) {
        value -= 10;
        aces--;
    }
    return value;
}

void handlePlayerChoice(bool hit, vector<int>& playerHand, vector<int>& dealerHand, sf::Text& resultText, sf::Text& pointsText, int& points, int bet, sf::Text& playerHandText, sf::Text& dealerHandText, bool& playerTurn, bool& gameEnded, bool& dealerCardRevealed)
{
    if (hit) {
        playerHand.push_back(drawCard());
        if (calculateHandValue(playerHand) > 21) {
            resultText.setString("Busted! You lose.");
            playerTurn = false;
            gameEnded = true; // End the game after bust
            dealerCardRevealed = true; // Reveal dealer's hidden card
        }
    } else {
        playerTurn = false;
        int playerValue = calculateHandValue(playerHand);
        int dealerValue = calculateHandValue(dealerHand);

        if (dealerValue > 21 || playerValue > dealerValue) {
            resultText.setString("You win!");
            points += bet * 2; // Player wins, double the bet amount
        } else if (playerValue == dealerValue) {
            resultText.setString("It's a tie!");
            points += bet; // Tie, return the bet amount
        } else {
            resultText.setString("Dealer wins.");
            // No need to adjust points as the bet has already been deducted
        }
        gameEnded = true; // End the game after staying and comparison
        dealerCardRevealed = true; // Reveal dealer's hidden card
    }

    // Update points display
    pointsText.setString("Points: " + std::to_string(points));

    // Update player hand display
    string playerHandStr = "Player Hand: ";
    for (int card : playerHand) {
        playerHandStr += to_string(card) + " ";
    }
    playerHandText.setString(playerHandStr);

    // Update dealer hand display
    if (dealerCardRevealed) {
        string dealerHandStr = "Dealer Hand: ";
        for (int card : dealerHand) {
            dealerHandStr += to_string(card) + " ";
        }
        dealerHandText.setString(dealerHandStr);
    } else {
        dealerHandText.setString("Dealer Hand: " + to_string(dealerHand[0]) + " ?");
    }
}

bool isValidBet(int bet, int points)
{
    return bet > 0 && bet <= points;
}

