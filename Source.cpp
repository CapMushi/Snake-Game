﻿#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <SFML/Audio.hpp>
#include <fstream>
using namespace std;


// Constants
const int gridWidth = 40;
const int gridHeight = 40;
const int cellSize = 15;
const int wallThickness = 3;
const int maxHighScores = 5; // Maximum number of high scores


bool isPaused = false;  // Initially, the game is not paused
bool exitGame = false;
bool startNewGame = false;
int highScores[maxHighScores] = { 0 }; // High scores, initially all 0


sf::Texture backgroundTexture;
sf::Sprite backgroundSprite;

sf::Texture backgroundTexture3;
sf::Sprite backgroundSprite3;

sf::Texture backgroundTexture4;
sf::Sprite backgroundSprite4;
sf::Color snakeColor = sf::Color::Green;  // Default color is Green

bool customWalls[gridHeight][gridWidth] = { false }; // Grid to track custom walls
bool isDesignMode = true; // Flag to toggle between wall design and gameplay
int snakeSpeed = 9; // Default speed for the snake





enum Direction { UP, DOWN, LEFT, RIGHT };

// Function Declarations
void showTitleScreen(sf::RenderWindow& window, int& gameMode);
void updateHighScores(int score);
void saveHighScoresToFile();
void loadHighScoresFromFile();
bool showGameOverScreen(sf::RenderWindow& window);
void initializeGame(int snake[][2], int& snakeLength, Direction& dir, int& fruitX, int& fruitY);
void handleInput(Direction& dir, const Direction& currentDir);
bool updateGame(int snake[][2], int& snakeLength, Direction dir, int& fruitX, int& fruitY, int& score);
bool updateGameWithWalls(int snake[][2], int& snakeLength, Direction dir, int& fruitX, int& fruitY, int& score);
bool updateGameForNewLevel(int snake[][2], int& snakeLength, Direction dir, int& fruitX, int& fruitY, int& score);
void renderGame(sf::RenderWindow& window, const int snake[][2], int snakeLength, int fruitX, int fruitY);
void renderGameWithWalls(sf::RenderWindow& window, const int snake[][2], int snakeLength, int fruitX, int fruitY);
void renderGameForNewLevel(sf::RenderWindow& window, const int snake[][2], int snakeLength, int fruitX, int fruitY);
void runGame();
void runGame2();
void runGame3();
void generateFruit(int& fruitX, int& fruitY, const int snake[][2], int snakeLength);
void generateFruitWithWalls(int& fruitX, int& fruitY, const int snake[][2], int snakeLength);
void generateFruitForNewLevel(int& fruitX, int& fruitY, const int snake[][2], int snakeLength);
bool updateGameForLevel4(int snake[][2], int& snakeLength, Direction dir, int& fruitX, int& fruitY, int& score);
void renderGameForLevel4(sf::RenderWindow& window, const int snake[][2], int snakeLength, int fruitX, int fruitY);
void runGame4();
void generateFruitForLevel4(int& fruitX, int& fruitY, const int snake[][2], int snakeLength);


void renderWallDesign(sf::RenderWindow& window) {
    sf::RectangleShape cell(sf::Vector2f(cellSize, cellSize));
    cell.setFillColor(sf::Color::Blue);

    for (int y = 0; y < gridHeight; ++y) {
        for (int x = 0; x < gridWidth; ++x) {
            if (customWalls[y][x]) {
                cell.setPosition(static_cast<float>(x * cellSize), static_cast<float>(y * cellSize));
                window.draw(cell);
            }
        }
    }
}


void renderCustomGame(sf::RenderWindow& window, const int snake[][2], int snakeLength, int fruitX, int fruitY) {
    renderWallDesign(window); // Draw custom walls

    // Draw Snake
    sf::RectangleShape cell(sf::Vector2f(cellSize, cellSize));
    cell.setFillColor(sf::Color::Green);
    for (int i = 0; i < snakeLength; ++i) {
        cell.setPosition(static_cast<float>(snake[i][0] * cellSize), static_cast<float>(snake[i][1] * cellSize));
        window.draw(cell);
    }

    // Draw Fruit
    cell.setFillColor(sf::Color::Red);
    cell.setPosition(static_cast<float>(fruitX * cellSize), static_cast<float>(fruitY * cellSize));
    window.draw(cell);
}


void generateCustomFruit(int& fruitX, int& fruitY, const int snake[][2], int snakeLength) {
    bool positionValid;
    do {
        positionValid = true;
        fruitX = rand() % gridWidth;
        fruitY = rand() % gridHeight;

        // Check for collision with the snake
        for (int i = 0; i < snakeLength; ++i) {
            if (snake[i][0] == fruitX && snake[i][1] == fruitY) {
                positionValid = false;
                break;
            }
        }

        // Check for collision with custom walls or flower-like structures
        if (customWalls[fruitY][fruitX]) {
            positionValid = false;
        }
    } while (!positionValid);
}



bool updateCustomGame(int snake[][2], int& snakeLength, Direction dir, int& fruitX, int& fruitY, int& score) {
    int newHeadX = snake[0][0];
    int newHeadY = snake[0][1];
    switch (dir) {
    case UP:    --newHeadY; break;
    case DOWN:  ++newHeadY; break;
    case LEFT:  --newHeadX; break;
    case RIGHT: ++newHeadX; break;
    }

    // Wrap around edges
    if (newHeadX < 0) newHeadX = gridWidth - 1;
    if (newHeadX >= gridWidth) newHeadX = 0;
    if (newHeadY < 0) newHeadY = gridHeight - 1;
    if (newHeadY >= gridHeight) newHeadY = 0;

    // Collision with custom walls
    if (customWalls[newHeadY][newHeadX]) {
        return false; // Game over
    }

    // Collision with self
    for (int i = 0; i < snakeLength; ++i) {
        if (snake[i][0] == newHeadX && snake[i][1] == newHeadY) {
            return false; // Game over
        }
    }

    // Move the snake body
    for (int i = snakeLength - 1; i > 0; --i) {
        snake[i][0] = snake[i - 1][0];
        snake[i][1] = snake[i - 1][1];
    }

    snake[0][0] = newHeadX;
    snake[0][1] = newHeadY;

    // Fruit collision
    if (snake[0][0] == fruitX && snake[0][1] == fruitY) {
        ++snakeLength;
        score += 10;
        generateCustomFruit(fruitX, fruitY, snake, snakeLength);
    }

    return true;
}








void runCustomGame() {
    sf::RenderWindow window(sf::VideoMode(gridWidth * cellSize, gridHeight * cellSize), "Snake Game - Custom Mode");
    window.setFramerateLimit(snakeSpeed);

    sf::Font font;
    if (!font.loadFromFile("C:/Users/hp/Desktop/SNAKE GAME SFML/arial.ttf")) {
        std::cerr << "Error loading font!" << std::endl;
        return;
    }

    sf::Text instructions;
    instructions.setFont(font);
    instructions.setCharacterSize(20);
    instructions.setFillColor(sf::Color::White);
    instructions.setPosition(10, 10);
    instructions.setString("Press 'A' to add walls. Press 'Enter' to start the game.");

    int snake[1600][2];
    int snakeLength;
    Direction dir = UP, currentDir = UP; // Snake starts moving upward
    int fruitX, fruitY;
    int score = 0;

    // Initialize the snake at the bottom center of the screen
    snakeLength = 3;
    for (int i = 0; i < snakeLength; ++i) {
        snake[i][0] = gridWidth / 2;           // Center column
        snake[i][1] = gridHeight - 1 - i;      // Bottom row moving upwards
    }

    initializeGame(snake, snakeLength, dir, fruitX, fruitY);
    currentDir = dir;

    int wallRow = 0;            // Starting row for wall design
    int leftColumn = 0;         // Starting column for left-side wall
    int rightColumn = gridWidth - 1; // Starting column for right-side wall
    int boxesAdded = 0;         // Track how many boxes are added in the current sequence

    bool isFlowerMode = false;  // Toggle to start the flower-like structure
    int flowerRadius = 0;       // Radius for the flower structure
    int flowerCenterX = gridWidth / 2;
    int flowerCenterY = gridHeight / 2;
    const int maxFlowerRadius = 8; // Limit for the flower-like structure

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (isDesignMode && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::A) {
                if (!isFlowerMode) {
                    // Add walls in the row-by-row logic
                    if (boxesAdded < 4) {
                        // Add 4 boxes from the left
                        customWalls[wallRow][leftColumn] = true;
                        leftColumn += 2;  // Move to the next cell (with a gap)
                        ++boxesAdded;
                    }
                    else if (boxesAdded < 8) {
                        // Add 4 boxes from the right
                        customWalls[wallRow][rightColumn] = true;
                        rightColumn -= 2;  // Move to the next cell (with a gap)
                        ++boxesAdded;
                    }
                    else {
                        // Reset for the next row
                        boxesAdded = 0;
                        wallRow += 3;  // Move down 2 rows (leave 2 rows in between)
                        leftColumn = 0;  // Reset left column
                        rightColumn = gridWidth - 1;  // Reset right column

                        // Check if the whole screen is processed
                        if (wallRow >= gridHeight) {
                            isFlowerMode = true;  // Start flower structure
                        }
                    }
                }
                else {
                    // Create flower-like structure
                    for (int y = -flowerRadius; y <= flowerRadius; ++y) {
                        for (int x = -flowerRadius; x <= flowerRadius; ++x) {
                            int newX = flowerCenterX + x;
                            int newY = flowerCenterY + y;

                            // Check for circular boundary to create a petal-like shape
                            if (newX >= 0 && newX < gridWidth && newY >= 0 && newY < gridHeight &&
                                (x * x + y * y <= flowerRadius * flowerRadius)) {
                                customWalls[newY][newX] = true;
                            }
                        }
                    }

                    // Increment the flower radius for the next layer
                    flowerRadius += 1;

                    // Check if flower radius exceeds the limit
                    if (flowerRadius > maxFlowerRadius) {
                        isDesignMode = false; // End design mode
                        std::cout << "Flower structure complete. Starting game..." << std::endl;
                    }
                }
            }

            if (isDesignMode && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                // End design mode manually and choose speed
                isDesignMode = false;

                sf::Text speedPrompt("Select Snake Speed (1-10):", font, 20);
                speedPrompt.setFillColor(sf::Color::White);
                speedPrompt.setPosition(10, 50);

                while (window.isOpen()) {
                    window.clear();
                    window.draw(speedPrompt);
                    window.display();

                    sf::Event speedEvent;
                    while (window.pollEvent(speedEvent)) {
                        if (speedEvent.type == sf::Event::KeyPressed && speedEvent.key.code >= sf::Keyboard::Num1 && speedEvent.key.code <= sf::Keyboard::Num9) {
                            snakeSpeed = speedEvent.key.code - sf::Keyboard::Num0 + 1;
                            window.setFramerateLimit(snakeSpeed);
                            break;
                        }
                    }
                    break;
                }
            }
        }

        if (!isDesignMode) {
            // Gameplay logic
            handleInput(dir, currentDir);

            if (!updateCustomGame(snake, snakeLength, dir, fruitX, fruitY, score)) {
                std::cout << "Game Over! Final Score: " << score << std::endl;
                return;
            }

            currentDir = dir;

            // Render gameplay
            window.clear();
            renderCustomGame(window, snake, snakeLength, fruitX, fruitY);
            window.display();
        }
        else {
            // Render wall design
            window.clear();
            renderWallDesign(window);
            window.display();
        }
    }
}


// Load high scores from a file
void loadHighScoresFromFile() {
    ifstream file("highscores.txt");
    if (file.is_open()) {
        for (int i = 0; i < maxHighScores; ++i) {
            file >> highScores[i];
            if (file.fail()) {
                highScores[i] = 0; // Default to 0 if reading fails
            }
        }
        file.close();
    }
}

// Save high scores to a file
void saveHighScoresToFile() {
    ofstream file("highscores.txt");
    if (file.is_open()) {
        for (int i = 0; i < maxHighScores; ++i) {
            file << highScores[i] << endl;
        }
        file.close();
    }
}

// Update the high scores
void updateHighScores(int score) {
    for (int i = 0; i < maxHighScores; ++i) {
        if (score > highScores[i]) {
            // Shift lower scores down
            for (int j = maxHighScores - 1; j > i; --j) {
                highScores[j] = highScores[j - 1];
            }
            // Insert the new score
            highScores[i] = score;
            break;
        }
    }
    saveHighScoresToFile();
}


void showColorSelectionScreen(sf::RenderWindow& window) {
    sf::Font font;
    if (!font.loadFromFile("C:/Users/hp/Desktop/SNAKE GAME SFML/arial.ttf")) {
        std::cerr << "Error loading font!" << std::endl;
        return;
    }

    // Color options
    sf::Text redText("Red", font, 40);
    sf::Text greenText("Green", font, 40);
    sf::Text yellowText("Yellow", font, 40);

    // Positioning the text on the screen
    redText.setPosition(window.getSize().x / 2 - redText.getGlobalBounds().width / 2, 150);
    greenText.setPosition(window.getSize().x / 2 - greenText.getGlobalBounds().width / 2, 200);
    yellowText.setPosition(window.getSize().x / 2 - yellowText.getGlobalBounds().width / 2, 250);

    // Set the initial color
    redText.setFillColor(sf::Color::White);
    greenText.setFillColor(sf::Color::White);
    yellowText.setFillColor(sf::Color::White);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return;
            }

            if (event.type == sf::Event::MouseMoved) {
                // Change color when hovering over the options
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                if (redText.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    redText.setFillColor(sf::Color::Green);
                }
                else {
                    redText.setFillColor(sf::Color::White);
                }
                if (greenText.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    greenText.setFillColor(sf::Color::Green);
                }
                else {
                    greenText.setFillColor(sf::Color::White);
                }
                if (yellowText.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    yellowText.setFillColor(sf::Color::Green);
                }
                else {
                    yellowText.setFillColor(sf::Color::White);
                }
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                if (redText.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    snakeColor = sf::Color::Red;
                    return; // Color selected, exit
                }
                else if (greenText.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    snakeColor = sf::Color::Green;
                    return; // Color selected, exit
                }
                else if (yellowText.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    snakeColor = sf::Color::Yellow;
                    return; // Color selected, exit
                }
            }
        }

        window.clear(sf::Color::Black);
        window.draw(redText);
        window.draw(greenText);
        window.draw(yellowText);
        window.display();
    }
}




void showPauseMenu(sf::RenderWindow& window, bool& isPaused, bool& startNewGame, bool& exitGame) {
    sf::Font font;
    if (!font.loadFromFile("C:/Users/hp/Desktop/SNAKE GAME SFML/arial.ttf")) {
        std::cerr << "Error loading font!" << std::endl;
        return;
    }

    sf::Text menuOptions[4];
    std::string menuTexts[4] = { "Resume", "New Game", "Save Game", "Exit" };
    for (int i = 0; i < 4; i++) {
        menuOptions[i].setFont(font);
        menuOptions[i].setString(menuTexts[i]);
        menuOptions[i].setCharacterSize(40);
        menuOptions[i].setFillColor(sf::Color::White);
        menuOptions[i].setPosition(window.getSize().x / 2 - menuOptions[i].getGlobalBounds().width / 2,
            150 + i * 60);
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                exitGame = true;
                return;
            }

            if (event.type == sf::Event::MouseMoved) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                for (int i = 0; i < 4; i++) {
                    if (menuOptions[i].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        menuOptions[i].setFillColor(sf::Color::Green);
                    }
                    else {
                        menuOptions[i].setFillColor(sf::Color::White);
                    }
                }
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                for (int i = 0; i < 4; i++) {
                    if (menuOptions[i].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        if (i == 0) {
                            isPaused = false;  // Resume game
                            return;
                        }
                        else if (i == 1) {
                            startNewGame = true;  // Start a new game
                            window.close();
                            return;
                        }
                        else if (i == 2) {
                            std::cout << "Save Game functionality not implemented yet." << std::endl;
                        }
                        else if (i == 3) {
                            exitGame = true;  // Exit to start screen
                            window.close();
                            return;
                        }
                    }
                }
            }
        }

        window.clear(sf::Color::Black);
        for (int i = 0; i < 4; i++) {
            window.draw(menuOptions[i]);
        }
        window.display();
    }
}


void showTitleScreen(sf::RenderWindow& window, int& gameMode) {
    sf::Texture titleTexture;
    if (!titleTexture.loadFromFile("C:/Users/hp/Desktop/SNAKE GAME SFML/sg.jpg")) {
        cerr << "Error loading title screen image!" << endl;
        return;
    }

    sf::Sprite titleSprite;
    titleSprite.setTexture(titleTexture);

    sf::Font font;
    if (!font.loadFromFile("C:/Users/hp/Desktop/SNAKE GAME SFML/arial.ttf")) {
        cerr << "Error loading font!" << endl;
        return;
    }

    // Center the image in the window
    titleSprite.setPosition(
        (window.getSize().x - titleSprite.getGlobalBounds().width) / 2,
        (window.getSize().y - titleSprite.getGlobalBounds().height) / 2
    );

    sf::Text instructionText;
    instructionText.setFont(font);
    instructionText.setString("Press 1 for Zen\nPress 2 for EASY Mode\nPress 3 for MEDIUM Mode\nPress 4 for HARD Mode");
    instructionText.setCharacterSize(20); // Adjust size for visibility
    instructionText.setFillColor(sf::Color::White);

    // Position the instructions at the bottom center of the image
    sf::FloatRect imageBounds = titleSprite.getGlobalBounds();
    sf::FloatRect textBounds = instructionText.getLocalBounds();
    instructionText.setPosition(
        imageBounds.left + (imageBounds.width - textBounds.width) / 2,  // Center horizontally on the image
        imageBounds.top + imageBounds.height - textBounds.height - 20  // Position near the bottom of the image
    );


    sf::Text highScoreText("High Scores (Endless Mode):", font, 20);
    highScoreText.setFillColor(sf::Color::Yellow);
    highScoreText.setPosition(10, 10);

    sf::Text scores[maxHighScores];
    for (int i = 0; i < maxHighScores; ++i) {
        scores[i].setFont(font);
        scores[i].setCharacterSize(20);
        scores[i].setFillColor(sf::Color::White);
        scores[i].setPosition(10, 40 + i * 30);
        scores[i].setString(std::to_string(i + 1) + ". " + std::to_string(highScores[i]));
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return;
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Num1) {
                    gameMode = 1; // Endless mode
                    showColorSelectionScreen(window);
                    return;
                }
                else if (event.key.code == sf::Keyboard::Num2) {
                    gameMode = 2; // Walls mode
                    showColorSelectionScreen(window);
                    return;
                }
                else if (event.key.code == sf::Keyboard::Num3) {
                    gameMode = 3; // New level
                    showColorSelectionScreen(window);
                    return;
                }
                else if (event.key.code == sf::Keyboard::Num4) {
                    gameMode = 4; // Custom level
                    showColorSelectionScreen(window);
                    return;
                }
                else if (event.key.code == sf::Keyboard::Num5) {
                    gameMode = 5; // Custom level
                    showColorSelectionScreen(window);
                    return;
                }
            }
        }

        window.clear(sf::Color::Black);
        window.draw(titleSprite);
        window.draw(instructionText);

        // Draw High Scores
        window.draw(highScoreText);
        for (int i = 0; i < maxHighScores; ++i) {
            window.draw(scores[i]);
        }

        window.display();
    }
}

bool showGameOverScreen(sf::RenderWindow& window) {
    sf::Texture gameOverTexture;
    if (!gameOverTexture.loadFromFile("C:/Users/hp/Desktop/SNAKE GAME SFML/sgo.jpg")) {
        cerr << "Error loading game over screen image!" << endl;
        return false;
    }

    sf::Sprite gameOverSprite;
    gameOverSprite.setTexture(gameOverTexture);

    // Center the image in the window
    gameOverSprite.setPosition(
        (window.getSize().x - gameOverSprite.getGlobalBounds().width) / 2,
        (window.getSize().y - gameOverSprite.getGlobalBounds().height) / 2
    );

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return false; // Exit the game
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Y) {
                    return true; // Replay the game
                }
                else {
                    return false; // Exit the game
                }
            }
        }

        window.clear(sf::Color::Black);
        window.draw(gameOverSprite);
        window.display();
    }

    return false; // Default exit if the window is closed
}

// Fruit Generation so does not spawn on file
void generateFruit(int& fruitX, int& fruitY, const int snake[][2], int snakeLength) {
    bool positionValid;
    do {
        positionValid = true;
        fruitX = rand() % gridWidth;
        fruitY = wallThickness + rand() % (gridHeight - wallThickness); // Spawn below walls

        for (int i = 0; i < snakeLength; ++i) {
            if (snake[i][0] == fruitX && snake[i][1] == fruitY) {
                positionValid = false;
                break;
            }
        }
    } while (!positionValid);
}

void generateFruitWithWalls(int& fruitX, int& fruitY, const int snake[][2], int snakeLength) {
    bool positionValid;
    do {
        positionValid = true;

        // Generate fruit position inside the playable area
        fruitX = wallThickness + rand() % (gridWidth - 2 * wallThickness);
        fruitY = wallThickness + rand() % (gridHeight - 2 * wallThickness);

        // Ensure the fruit does not overlap with the snake
        for (int i = 0; i < snakeLength; ++i) {
            if (snake[i][0] == fruitX && snake[i][1] == fruitY) {
                positionValid = false;
                break;
            }
        }
    } while (!positionValid);
}


// Game Initialization
void initializeGame(int snake[][2], int& snakeLength, Direction& dir, int& fruitX, int& fruitY) {
    snakeLength = 1;
    snake[0][0] = gridWidth / 2;
    snake[0][1] = gridHeight / 4;
    dir = UP;
    generateFruit(fruitX, fruitY, snake, snakeLength);
}

// Input Handling
void handleInput(Direction& dir, const Direction& currentDir) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && currentDir != DOWN) dir = UP;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && currentDir != UP) dir = DOWN;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && currentDir != RIGHT) dir = LEFT;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && currentDir != LEFT) dir = RIGHT;
}

// Game Update (Endless Mode)
bool updateGame(int snake[][2], int& snakeLength, Direction dir, int& fruitX, int& fruitY, int& score) {
    int newHeadX = snake[0][0];
    int newHeadY = snake[0][1];
    switch (dir) {
    case UP:    --newHeadY; break;
    case DOWN:  ++newHeadY; break;
    case LEFT:  --newHeadX; break;
    case RIGHT: ++newHeadX; break;
    }

    if (newHeadX < 0) newHeadX = gridWidth - 1;
    if (newHeadX >= gridWidth) newHeadX = 0;
    if (newHeadY < 0) newHeadY = gridHeight - 1;
    if (newHeadY >= gridHeight) newHeadY = 0;

    for (int i = 0; i < snakeLength; ++i) {
        if (snake[i][0] == newHeadX && snake[i][1] == newHeadY) {
            return false;
        }
    }

    for (int i = snakeLength - 1; i > 0; --i) {
        snake[i][0] = snake[i - 1][0];
        snake[i][1] = snake[i - 1][1];
    }

    snake[0][0] = newHeadX;
    snake[0][1] = newHeadY;

    if (snake[0][0] == fruitX && snake[0][1] == fruitY) {
        ++snakeLength;
        score += 10;
        generateFruit(fruitX, fruitY, snake, snakeLength);
    }

    return true;
}

// Game Update (Walls Mode)
bool updateGameWithWalls(int snake[][2], int& snakeLength, Direction dir, int& fruitX, int& fruitY, int& score) {
    int newHeadX = snake[0][0];
    int newHeadY = snake[0][1];
    switch (dir) {
    case UP:    --newHeadY; break;
    case DOWN:  ++newHeadY; break;
    case LEFT:  --newHeadX; break;
    case RIGHT: ++newHeadX; break;
    }

    // Collision detection with walls
    if (newHeadX < wallThickness || newHeadX >= gridWidth - wallThickness ||
        newHeadY < wallThickness || newHeadY >= gridHeight - wallThickness) {
        return false; // Game over on collision with walls
    }

    // Collision detection with self
    for (int i = 0; i < snakeLength; ++i) {
        if (snake[i][0] == newHeadX && snake[i][1] == newHeadY) {
            return false;
        }
    }

    // Move the snake body
    for (int i = snakeLength - 1; i > 0; --i) {
        snake[i][0] = snake[i - 1][0];
        snake[i][1] = snake[i - 1][1];
    }

    // Update the snake's head position
    snake[0][0] = newHeadX;
    snake[0][1] = newHeadY;

    // Fruit collision and growth
    if (snake[0][0] == fruitX && snake[0][1] == fruitY) {
        ++snakeLength;
        score += 10;
        generateFruitWithWalls(fruitX, fruitY, snake, snakeLength);
    }

    return true;
}


// Game Rendering (Endless Mode)
void renderGame(sf::RenderWindow& window, const int snake[][2], int snakeLength, int fruitX, int fruitY) {
    window.clear(); // Clear with default color

    // Draw the background first
    window.draw(backgroundSprite);

    // Render Snake
    sf::RectangleShape cell(sf::Vector2f(cellSize, cellSize));
    cell.setOutlineColor(sf::Color::White);
    cell.setOutlineThickness(1);
    cell.setFillColor(snakeColor);
    for (int i = 0; i < snakeLength; ++i) {
        cell.setPosition(static_cast<float>(snake[i][0] * cellSize), static_cast<float>(snake[i][1] * cellSize));
        window.draw(cell);
    }

    // Render Fruit
    cell.setFillColor(sf::Color::Red);
    cell.setPosition(static_cast<float>(fruitX * cellSize), static_cast<float>(fruitY * cellSize));
    window.draw(cell);
}


// Game Rendering (Walls Mode)
void renderGameWithWalls(sf::RenderWindow& window, const int snake[][2], int snakeLength, int fruitX, int fruitY) {
    // Draw the background first
    window.draw(backgroundSprite);

    sf::RectangleShape cell(sf::Vector2f(cellSize, cellSize));
    cell.setOutlineColor(sf::Color::White);
    cell.setOutlineThickness(1);

    // Render Walls (Top, Left, Right, and Bottom)
    cell.setFillColor(sf::Color::Blue);

    // Top Wall
    for (int x = 0; x < gridWidth; ++x) {
        for (int y = 0; y < wallThickness; ++y) {
            cell.setPosition(static_cast<float>(x * cellSize), static_cast<float>(y * cellSize));
            window.draw(cell);
        }
    }

    // Left Wall
    for (int y = 0; y < gridHeight; ++y) {
        for (int x = 0; x < wallThickness; ++x) {
            cell.setPosition(static_cast<float>(x * cellSize), static_cast<float>(y * cellSize));
            window.draw(cell);
        }
    }

    // Right Wall
    for (int y = 0; y < gridHeight; ++y) {
        for (int x = gridWidth - wallThickness; x < gridWidth; ++x) {
            cell.setPosition(static_cast<float>(x * cellSize), static_cast<float>(y * cellSize));
            window.draw(cell);
        }
    }

    // Bottom Wall
    for (int x = 0; x < gridWidth; ++x) {
        for (int y = gridHeight - wallThickness; y < gridHeight; ++y) {
            cell.setPosition(static_cast<float>(x * cellSize), static_cast<float>(y * cellSize));
            window.draw(cell);
        }
    }

    // Render Snake
    cell.setFillColor(snakeColor);
    for (int i = 0; i < snakeLength; ++i) {
        cell.setPosition(static_cast<float>(snake[i][0] * cellSize), static_cast<float>(snake[i][1] * cellSize));
        window.draw(cell);
    }

    // Render Fruit
    cell.setFillColor(sf::Color::Red);
    cell.setPosition(static_cast<float>(fruitX * cellSize), static_cast<float>(fruitY * cellSize));
    window.draw(cell);
}



// Main Game Loop (Endless Mode)
void runGame() {
    sf::RenderWindow window(sf::VideoMode(gridWidth * cellSize, gridHeight * cellSize), "Snake Game - Zen Mode");
    window.setFramerateLimit(9);

    if (!backgroundTexture.loadFromFile("z1.jpg")) {
        std::cerr << "Error loading z1.jpg!" << std::endl;
        return;
    }

    backgroundSprite.setTexture(backgroundTexture);

    // Scale the sprite to fit the window
    backgroundSprite.setScale(
        static_cast<float>(gridWidth * cellSize) / backgroundTexture.getSize().x,
        static_cast<float>(gridHeight * cellSize) / backgroundTexture.getSize().y
    );

    int snake[1600][2];
    int snakeLength;
    Direction dir, currentDir;
    int fruitX, fruitY;
    int score = 0;

    // Game initialization
    initializeGame(snake, snakeLength, dir, fruitX, fruitY);
    currentDir = dir;

    sf::Font font;
    if (!font.loadFromFile("C:/Users/hp/Desktop/SNAKE GAME SFML/arial.ttf")) {
        std::cerr << "Error loading font!" << std::endl;
        return;
    }

    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(20);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(10, 10);
    scoreText.setString("Score: 0");

    while (window.isOpen() && !exitGame) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // Pause the game if 'P' is pressed
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::P) {
                isPaused = !isPaused;
                if (isPaused) {
                    showPauseMenu(window, isPaused, startNewGame, exitGame);
                    if (startNewGame) {
                        return;
                    }
                }
            }
        }

        // Game logic (only if not paused)
        if (!isPaused) {
            handleInput(dir, currentDir);

            if (!updateGame(snake, snakeLength, dir, fruitX, fruitY, score)) {
                std::cout << "Game Over! Final Score: " << score << std::endl;
                updateHighScores(score);
                return;
            }

            currentDir = dir;

            scoreText.setString("Score: " + std::to_string(score));

            // Draw the background
            window.clear();
            window.draw(backgroundSprite);

            // Render game elements
            renderGame(window, snake, snakeLength, fruitX, fruitY);

            // Draw the score
            window.draw(scoreText);

            // Display everything
            window.display();
        }
    }
}



// Main Game Loop (Walls Mode)
void runGame2() {
    sf::RenderWindow window(sf::VideoMode(gridWidth * cellSize, gridHeight * cellSize), "Snake Game - Easy Mode");
    window.setFramerateLimit(8);




    // Load background texture
    if (!backgroundTexture.loadFromFile("z1.jpg")) {
        std::cerr << "Error loading z1.jpg!" << std::endl;
        return;
    }

    backgroundSprite.setTexture(backgroundTexture);

    // Scale the sprite to fit the window
    backgroundSprite.setScale(
        static_cast<float>(gridWidth * cellSize) / backgroundTexture.getSize().x,
        static_cast<float>(gridHeight * cellSize) / backgroundTexture.getSize().y
    );

    sf::Font font;
    if (!font.loadFromFile("C:/Users/hp/Desktop/SNAKE GAME SFML/arial.ttf")) {
        std::cerr << "Error loading font!" << std::endl;
        return;
    }

    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(20);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(10, 10);
    scoreText.setString("Score: 0");

    int snake[1600][2];
    int snakeLength;
    Direction dir, currentDir;
    int fruitX, fruitY;
    int score = 0;

    initializeGame(snake, snakeLength, dir, fruitX, fruitY);
    currentDir = dir;

    // Use generateFruitWithWalls for initial fruit generation
    generateFruitWithWalls(fruitX, fruitY, snake, snakeLength);

    while (window.isOpen() && !exitGame) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::P) {
                isPaused = !isPaused;
                if (isPaused) {
                    showPauseMenu(window, isPaused, startNewGame, exitGame);
                    if (startNewGame) return;
                }
            }
        }


        if (!isPaused) {
            handleInput(dir, currentDir);


            if (!updateGameWithWalls(snake, snakeLength, dir, fruitX, fruitY, score)) {
                std::cout << "Game Over! Final Score: " << score << std::endl;
                updateHighScores(score);
                return;
            }

            currentDir = dir;

            scoreText.setString("Score: " + std::to_string(score));

            // Draw the background first
            window.clear();
            window.draw(backgroundSprite);

            // Render game elements
            renderGameWithWalls(window, snake, snakeLength, fruitX, fruitY);

            // Draw the score
            window.draw(scoreText);

            // Display everything
            window.display();
        }
    }
}



void generateFruitForNewLevel(int& fruitX, int& fruitY, const int snake[][2], int snakeLength) {
    bool positionValid;
    do {
        positionValid = true;

        fruitX = rand() % gridWidth;
        fruitY = 3 + rand() % (gridHeight - 3); // Spawn only below the third row

        for (int i = 0; i < snakeLength; ++i) {
            if (snake[i][0] == fruitX && snake[i][1] == fruitY) {
                positionValid = false;
                break;
            }
        }

        // Ensure the fruit does not spawn in horizontal walls
        int wallLength = (2 * gridWidth) / 3;
        int leftSpace = (gridWidth - wallLength) / 2;
        if ((fruitY == gridHeight / 2 - 2 || fruitY == gridHeight / 2 + 2) &&
            fruitX >= leftSpace && fruitX < leftSpace + wallLength) {
            positionValid = false;
        }
    } while (!positionValid);
}

void renderGameForNewLevel(sf::RenderWindow& window, const int snake[][2], int snakeLength, int fruitX, int fruitY) {
    // Draw the background first
    window.draw(backgroundSprite3); // backgroundSprite3 is declared static in runGame3()

    sf::RectangleShape cell(sf::Vector2f(cellSize, cellSize));
    cell.setOutlineColor(sf::Color::White);
    cell.setOutlineThickness(1);

    // Render Horizontal Walls
    cell.setFillColor(sf::Color::Blue);
    int wallLength = (2 * gridWidth) / 3;
    int leftSpace = (gridWidth - wallLength) / 2;

    for (int x = leftSpace; x < leftSpace + wallLength; ++x) {
        // Horizontal wall 1
        cell.setPosition(static_cast<float>(x * cellSize), static_cast<float>((gridHeight / 2 - 2) * cellSize));
        window.draw(cell);

        // Horizontal wall 2
        cell.setPosition(static_cast<float>(x * cellSize), static_cast<float>((gridHeight / 2 + 2) * cellSize));
        window.draw(cell);
    }

    // Render Snake
    cell.setFillColor(snakeColor);
    for (int i = 0; i < snakeLength; ++i) {
        cell.setPosition(static_cast<float>(snake[i][0] * cellSize), static_cast<float>(snake[i][1] * cellSize));
        window.draw(cell);
    }

    // Render Fruit
    cell.setFillColor(sf::Color::Red);
    cell.setPosition(static_cast<float>(fruitX * cellSize), static_cast<float>(fruitY * cellSize));
    window.draw(cell);
}




bool updateGameForNewLevel(int snake[][2], int& snakeLength, Direction dir, int& fruitX, int& fruitY, int& score) {
    int newHeadX = snake[0][0];
    int newHeadY = snake[0][1];
    switch (dir) {
    case UP:    --newHeadY; break;
    case DOWN:  ++newHeadY; break;
    case LEFT:  --newHeadX; break;
    case RIGHT: ++newHeadX; break;
    }

    int wallLength = (2 * gridWidth) / 3;
    int leftSpace = (gridWidth - wallLength) / 2;

    // Restrict snake from entering the first three rows
    if (newHeadY < 3) {
        newHeadY = gridHeight - 1; // Spawn at the bottom
    }

    // Screen wrapping (top/bottom)
    if (newHeadY >= gridHeight) newHeadY = 3; // Reappear at the top after bottom exit

    // Screen wrapping (left/right)
    if (newHeadX < 0) newHeadX = gridWidth - 1;
    if (newHeadX >= gridWidth) newHeadX = 0;

    // Collision detection with horizontal walls
    if ((newHeadY == gridHeight / 2 - 2 || newHeadY == gridHeight / 2 + 2) &&
        newHeadX >= leftSpace && newHeadX < leftSpace + wallLength) {
        return false; // Game over on collision with horizontal walls
    }

    // Collision detection with self
    for (int i = 0; i < snakeLength; ++i) {
        if (snake[i][0] == newHeadX && snake[i][1] == newHeadY) {
            return false; // Game over on collision with self
        }
    }

    // Move the snake body
    for (int i = snakeLength - 1; i > 0; --i) {
        snake[i][0] = snake[i - 1][0];
        snake[i][1] = snake[i - 1][1];
    }

    // Update the snake's head position
    snake[0][0] = newHeadX;
    snake[0][1] = newHeadY;

    // Fruit collision and growth
    if (snake[0][0] == fruitX && snake[0][1] == fruitY) {
        ++snakeLength;
        score += 10;
        generateFruitForNewLevel(fruitX, fruitY, snake, snakeLength);
    }

    return true;
}

// New Level Main Loop
void runGame3() {
    sf::RenderWindow window(sf::VideoMode(gridWidth * cellSize, gridHeight * cellSize), "Snake Game - Medium Mode");
    window.setFramerateLimit(9);

    // Load background texture for Medium Mode
    static sf::Texture backgroundTexture3; // Declare static to ensure it stays loaded for this mode
    if (!backgroundTexture3.loadFromFile("lvl3.png")) {
        std::cerr << "Error loading lvl3.jpeg!" << std::endl;
        return;
    }

    static sf::Sprite backgroundSprite3; // Declare static to persist the sprite
    backgroundSprite3.setTexture(backgroundTexture3);

    // Scale the sprite to fit the window
    backgroundSprite3.setScale(
        static_cast<float>(gridWidth * cellSize) / backgroundTexture3.getSize().x,
        static_cast<float>(gridHeight * cellSize) / backgroundTexture3.getSize().y
    );

    sf::Font font;
    if (!font.loadFromFile("C:/Users/hp/Desktop/SNAKE GAME SFML/arial.ttf")) {
        std::cerr << "Error loading font!" << std::endl;
        return;
    }

    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(20);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(10, 10);
    scoreText.setString("Score: 0");

    int snake[1600][2];
    int snakeLength;
    Direction dir, currentDir;
    int fruitX, fruitY;
    int score = 0;

    initializeGame(snake, snakeLength, dir, fruitX, fruitY);
    currentDir = dir;

    // Use generateFruitForNewLevel for initial fruit generation
    generateFruitForNewLevel(fruitX, fruitY, snake, snakeLength);

    while (window.isOpen() && !exitGame) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::P) {
                isPaused = !isPaused;
                if (isPaused) {
                    showPauseMenu(window, isPaused, startNewGame, exitGame);
                    if (startNewGame) return;
                }
            }
        }


        if (!isPaused) {
            handleInput(dir, currentDir);

            if (!updateGameForNewLevel(snake, snakeLength, dir, fruitX, fruitY, score)) {
                std::cout << "Game Over! Final Score: " << score << std::endl;
                updateHighScores(score);
                return;
            }

            currentDir = dir;

            scoreText.setString("Score: " + std::to_string(score));

            // Draw the background first
            window.clear();
            window.draw(backgroundSprite3);

            // Render game elements
            renderGameForNewLevel(window, snake, snakeLength, fruitX, fruitY);

            // Draw the score
            window.draw(scoreText);

            // Display everything
            window.display();
        }
    }
}





// Fruit Generation for Level 4
void generateFruitForLevel4(int& fruitX, int& fruitY, const int snake[][2], int snakeLength) {
    bool positionValid;
    do {
        positionValid = true;
        fruitX = rand() % gridWidth;
        fruitY = 3 + rand() % (gridHeight - 3); // Spawn only below the third row

        for (int i = 0; i < snakeLength; ++i) {
            if (snake[i][0] == fruitX && snake[i][1] == fruitY) {
                positionValid = false;
                break;
            }
        }

        // Ensure the fruit does not spawn in walls or boxes
        if ((fruitY == gridHeight / 2 && fruitX >= gridWidth / 3 && fruitX <= 2 * gridWidth / 3) ||  // Horizontal bar of T
            (fruitX == gridWidth / 2 && fruitY >= gridHeight / 4 && fruitY <= gridHeight / 2) ||      // Vertical bar of T
            (fruitX >= 5 && fruitX <= 10 && fruitY >= 5 && fruitY <= 10) ||                         // Box 1
            (fruitX >= gridWidth - 10 && fruitX <= gridWidth - 5 && fruitY >= 5 && fruitY <= 10) ||  // Box 2
            (fruitX >= 5 && fruitX <= 10 && fruitY >= gridHeight - 10 && fruitY <= gridHeight - 5) || // Box 3
            (fruitX >= gridWidth - 10 && fruitX <= gridWidth - 5 && fruitY >= gridHeight - 10 && fruitY <= gridHeight - 5)) { // Box 4
            positionValid = false;
        }
    } while (!positionValid);
}



// Render Game for Level 4
void renderGameForLevel4(sf::RenderWindow& window, const int snake[][2], int snakeLength, int fruitX, int fruitY) {
    // Draw the background first
    window.draw(backgroundSprite4);

    sf::RectangleShape cell(sf::Vector2f(cellSize, cellSize));
    cell.setOutlineColor(sf::Color::White);
    cell.setOutlineThickness(1);

    // Render Inverted T
    cell.setFillColor(sf::Color::Blue);

    // Horizontal bar of T
    for (int x = gridWidth / 3; x <= 2 * gridWidth / 3; ++x) {
        cell.setPosition(static_cast<float>(x * cellSize), static_cast<float>((gridHeight / 2) * cellSize));
        window.draw(cell);
    }

    // Vertical bar of T
    for (int y = gridHeight / 4; y <= gridHeight / 2; ++y) {
        cell.setPosition(static_cast<float>((gridWidth / 2) * cellSize), static_cast<float>(y * cellSize));
        window.draw(cell);
    }

    // Render Separate Boxes
    for (int x = 5; x <= 10; ++x) {
        for (int y = 5; y <= 10; ++y) {
            cell.setPosition(static_cast<float>(x * cellSize), static_cast<float>(y * cellSize));
            window.draw(cell);
        }
    }

    for (int x = gridWidth - 10; x <= gridWidth - 5; ++x) {
        for (int y = 5; y <= 10; ++y) {
            cell.setPosition(static_cast<float>(x * cellSize), static_cast<float>(y * cellSize));
            window.draw(cell);
        }
    }

    for (int x = 5; x <= 10; ++x) {
        for (int y = gridHeight - 10; y <= gridHeight - 5; ++y) {
            cell.setPosition(static_cast<float>(x * cellSize), static_cast<float>(y * cellSize));
            window.draw(cell);
        }
    }

    for (int x = gridWidth - 10; x <= gridWidth - 5; ++x) {
        for (int y = gridHeight - 10; y <= gridHeight - 5; ++y) {
            cell.setPosition(static_cast<float>(x * cellSize), static_cast<float>(y * cellSize));
            window.draw(cell);
        }
    }

    // Render Snake
    cell.setFillColor(snakeColor);
    for (int i = 0; i < snakeLength; ++i) {
        cell.setPosition(static_cast<float>(snake[i][0] * cellSize), static_cast<float>(snake[i][1] * cellSize));
        window.draw(cell);
    }

    // Render Fruit
    cell.setFillColor(sf::Color::Red);
    cell.setPosition(static_cast<float>(fruitX * cellSize), static_cast<float>(fruitY * cellSize));
    window.draw(cell);
}




// Update Game for Level 4
bool updateGameForLevel4(int snake[][2], int& snakeLength, Direction dir, int& fruitX, int& fruitY, int& score) {
    int newHeadX = snake[0][0];
    int newHeadY = snake[0][1];
    switch (dir) {
    case UP:    --newHeadY; break;
    case DOWN:  ++newHeadY; break;
    case LEFT:  --newHeadX; break;
    case RIGHT: ++newHeadX; break;
    }

    // Wrap around when the snake goes out of bounds
    if (newHeadX < 0) newHeadX = gridWidth - 1;
    if (newHeadX >= gridWidth) newHeadX = 0;
    if (newHeadY < 3) newHeadY = gridHeight - 1;
    if (newHeadY >= gridHeight) newHeadY = 3;

    // Collision detection with inverted T and new boxes
    if ((newHeadY == gridHeight / 2 && newHeadX >= gridWidth / 3 && newHeadX <= 2 * gridWidth / 3) ||  // Horizontal bar of T
        (newHeadX == gridWidth / 2 && newHeadY >= gridHeight / 4 && newHeadY <= gridHeight / 2) ||      // Vertical bar of T
        (newHeadX >= 5 && newHeadX <= 10 && newHeadY >= 5 && newHeadY <= 10) ||                        // Box 1
        (newHeadX >= gridWidth - 10 && newHeadX <= gridWidth - 5 && newHeadY >= 5 && newHeadY <= 10) || // Box 2
        (newHeadX >= 5 && newHeadX <= 10 && newHeadY >= gridHeight - 10 && newHeadY <= gridHeight - 5) || // Box 3
        (newHeadX >= gridWidth - 10 && newHeadX <= gridWidth - 5 && newHeadY >= gridHeight - 10 && newHeadY <= gridHeight - 5)) { // Box 4
        return false;
    }

    // Collision detection with self
    for (int i = 0; i < snakeLength; ++i) {
        if (snake[i][0] == newHeadX && snake[i][1] == newHeadY) {
            return false;
        }
    }

    // Move the snake body
    for (int i = snakeLength - 1; i > 0; --i) {
        snake[i][0] = snake[i - 1][0];
        snake[i][1] = snake[i - 1][1];
    }

    // Update the snake's head position
    snake[0][0] = newHeadX;
    snake[0][1] = newHeadY;

    // Fruit collision and growth
    if (snake[0][0] == fruitX && snake[0][1] == fruitY) {
        ++snakeLength;
        score += 10;
        generateFruitForLevel4(fruitX, fruitY, snake, snakeLength);
    }

    return true;
}



// Game Initialization for Level 4
void initializeGameForLevel4(int snake[][2], int& snakeLength, Direction& dir, int& fruitX, int& fruitY) {
    snakeLength = 1;

    // Ensure the snake spawns outside the walls
    snake[0][0] = gridWidth / 2;         // Center of the grid horizontally
    snake[0][1] = gridHeight / 4 - 1;    // Just above the vertical wall

    dir = RIGHT;
    generateFruitForLevel4(fruitX, fruitY, snake, snakeLength);
}

// Main Game Loop for Level 4
void runGame4() {
    sf::RenderWindow window(sf::VideoMode(gridWidth * cellSize, gridHeight * cellSize), "Snake Game - Level 4");
    window.setFramerateLimit(10);

    // Load background texture for Level 4
    if (!backgroundTexture4.loadFromFile("lvl4.jpeg")) {
        std::cerr << "Error loading lvl4.jpg!" << std::endl;
        return;
    }

    backgroundSprite4.setTexture(backgroundTexture4);

    // Scale the sprite to fit the window
    backgroundSprite4.setScale(
        static_cast<float>(gridWidth * cellSize) / backgroundTexture4.getSize().x,
        static_cast<float>(gridHeight * cellSize) / backgroundTexture4.getSize().y
    );

    sf::Font font;
    if (!font.loadFromFile("C:/Users/hp/Desktop/SNAKE GAME SFML/arial.ttf")) {
        std::cerr << "Error loading font!" << std::endl;
        return;
    }

    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(20);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(10, 10);
    scoreText.setString("Score: 0");

    int snake[1600][2];
    int snakeLength;
    Direction dir, currentDir;
    int fruitX, fruitY;
    int score = 0;

    initializeGameForLevel4(snake, snakeLength, dir, fruitX, fruitY);
    currentDir = dir;

    // Use generateFruitForLevel4 for initial fruit generation
    generateFruitForLevel4(fruitX, fruitY, snake, snakeLength);

    while (window.isOpen() && !exitGame) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::P) {
                isPaused = !isPaused;
                if (isPaused) {
                    showPauseMenu(window, isPaused, startNewGame, exitGame);
                    if (startNewGame) return;
                }
            }
        }


        if (!isPaused) {
            handleInput(dir, currentDir);

            if (!updateGameForLevel4(snake, snakeLength, dir, fruitX, fruitY, score)) {
                std::cout << "Game Over! Final Score: " << score << std::endl;
                updateHighScores(score);
                return;
            }

            currentDir = dir;

            scoreText.setString("Score: " + std::to_string(score));

            // Draw the background first
            window.clear();
            window.draw(backgroundSprite4);

            // Render game elements
            renderGameForLevel4(window, snake, snakeLength, fruitX, fruitY);

            // Draw the score
            window.draw(scoreText);

            // Display everything
            window.display();
        }
    }
}


// Main Function
int main() {
    srand(static_cast<unsigned>(time(0)));
    loadHighScoresFromFile();


    while (true) {
        isPaused = false;  // Initially, the game is not paused
        exitGame = false;
        startNewGame = false;
        int gameMode = 0;
        sf::RenderWindow window(sf::VideoMode(gridWidth * cellSize, gridHeight * cellSize), "Snake Game");

        // Show title screen and select game mode
        showTitleScreen(window, gameMode);

        // Load and play music based on game mode
        sf::Music music;

        if (gameMode == 1) {
            // Endless mode
            if (!music.openFromFile("lvl3.mp3")) {
                std::cerr << "Error loading lvl2.mp3" << std::endl;
            }
            music.play();
            runGame();  // Endless mode
        }
        else if (gameMode == 2) {
            // Walls mode
            if (!music.openFromFile("lvl3.mp3")) {
                std::cerr << "Error loading level.mp3" << std::endl;
            }
            music.play();
            runGame2(); // Walls mode
        }
        else if (gameMode == 3) {
            // New level
            if (!music.openFromFile("lvl2.mp3")) {
                std::cerr << "Error loading newlevel.mp3" << std::endl;
            }
            music.play();
            runGame3(); // New level
        }
        else if (gameMode == 4) {
            // Custom level
            if (!music.openFromFile("lvl4.mp3")) {
                std::cerr << "Error loading customlevel.mp3" << std::endl;
            }
            music.play();
            runGame4(); // Custom level
        }
        else if (gameMode == 5) {
           
            runCustomGame(); // Custom level
        }

        // Check if "New Game" is selected during the pause
        if (startNewGame) {
            // Reset the game by returning to the title screen
            std::cout << "Starting new game..." << std::endl;
            continue;  // Skip the rest of the loop to return to the title screen
        }

        // Game Over Screen
        if (!showGameOverScreen(window)) {
            break;  // Exit if user decides not to replay
        }
    }

    return 0;
}