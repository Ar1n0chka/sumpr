#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <iostream>
#include <stdexcept>

const int CELL_SIZE = 20;

struct Cell {
    bool alive; 
    sf::RectangleShape shape;
};



class GameOfLife {
public:
    GameOfLife(int width, int height)
        : width(width), height(height), grid(width, std::vector<Cell>(height)) {
        window.create(sf::VideoMode(width * CELL_SIZE, height * CELL_SIZE), "Game of Life");
        initializeGrid();
    }

    void run() {
        bool running = true;
        bool paused = true;
        sf::Font font;
        if (!font.loadFromFile("arialmt.ttf")) 
        {
            std::cout <<"lol"; // Ошибка загрузки шрифта
        }

        // Текст для отображения
        sf::Text text;
        text.setFont(font);
        text.setCharacterSize(18);
        text.setFillColor(sf::Color::Black);
        text.setPosition(0, 0);

        sf::Text text2;
        text2.setString("");
        text2.setFont(font);
        text2.setCharacterSize(18);
        text2.setFillColor(sf::Color::Black);
        
        text2.setPosition(0, 20);

        // Прямоугольник для поля ввода
        sf::RectangleShape inputBox(sf::Vector2f(40, 40));
        inputBox.setFillColor(sf::Color(130, 130, 130));
        inputBox.setOutlineColor(sf::Color::Black);
        inputBox.setOutlineThickness(2);
        inputBox.setPosition(0, 0);

        // Строка для хранения введенного текста
        std::string inputText;

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
                else if (event.type == sf::Event::MouseButtonPressed) {
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        int x = event.mouseButton.x / CELL_SIZE;
                        int y = event.mouseButton.y / CELL_SIZE;
                        grid[x][y].alive = !grid[x][y].alive;
                        grid[x][y].shape.setFillColor(grid[x][y].alive ? sf::Color::Magenta : sf::Color::Black);
                    }
                }
                else if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Space) {
                        paused = !paused;
                    }
                    else if (event.key.code == sf::Keyboard::R) {
                        initializeGrid(); // Сброс поля
                    }
                }
                else if (event.type == sf::Event::TextEntered) 
                {
                    if (event.text.unicode < 128) 
                    {
                        if (event.text.unicode == '\b' && !inputText.empty()) {
                            // Обработка backspace
                            inputText.pop_back();
                        } else if (event.text.unicode == '\r') {
                            // Обработка клавиши Enter
                            std::cout << "Entered text: " << inputText << std::endl;
                            text2.setString(inputText); 
                            std::string temp = text2.getString().toAnsiString();
                            generateRandomCells(std::stoi(temp));
                            inputText.clear();
                        } else if (event.text.unicode == 'r')
                        {
                            //nothing
                        }
                        else if (isprint(event.text.unicode) && inputText.length() < width*height) {
                            // Добавление печатного символа в строку
                            inputText += static_cast<char>(event.text.unicode);
                        }
                        text.setString(inputText);
                    }
                }
            }

            if (!paused) {
                updateGrid();
            }

            window.clear();
            for (int x = 0; x < width; ++x) {
                for (int y = 0; y < height; ++y) {
                    window.draw(grid[x][y].shape);
                }
            }
            window.draw(inputBox);
            window.draw(text);
            window.draw(text2);
            window.display();

            sf::sleep(sf::milliseconds(100)); // Задержка для визуализации
        }
    }

    void generateRandomCells(int count) {
        for (int i = 0; i < count; ++i) {
            int x = rand() % width;
            int y = rand() % height;
            grid[x][y].alive = true;
            grid[x][y].shape.setFillColor(sf::Color::Magenta);
        }
    }

private:
    void initializeGrid() {
        for (int x = 0; x < width; ++x) {
            for (int y = 0; y < height; ++y) {
                grid[x][y].alive = false;
                grid[x][y].shape.setSize(sf::Vector2f(CELL_SIZE, CELL_SIZE));
                grid[x][y].shape.setPosition(x * CELL_SIZE, y * CELL_SIZE);
                grid[x][y].shape.setFillColor(sf::Color::Black);
                grid[x][y].shape.setOutlineThickness(1);
                grid[x][y].shape.setOutlineColor(sf::Color::White);
            }
        }
    }

    void updateGrid() {
        std::vector<std::vector<bool>> newGrid(width, std::vector<bool>(height, false));

        for (int x = 0; x < width; ++x) {
            for (int y = 0; y < height; ++y) {
                int liveNeighbors = countLiveNeighbors(x, y);

                if (grid[x][y].alive) {
                    if (liveNeighbors == 2 || liveNeighbors == 3) {
                        newGrid[x][y] = true;
                    } else {
                        newGrid[x][y] = false;
                    }
                } else {
                    if (liveNeighbors == 3) {
                        newGrid[x][y] = true;
                    }
                }
            }
        }

        for (int x = 0; x < width; ++x) {
            for (int y = 0; y < height; ++y) {
                grid[x][y].alive = newGrid[x][y];
                grid[x][y].shape.setFillColor(grid[x][y].alive ? sf::Color::Magenta : sf::Color::Black);
            }
        }
    }

    int countLiveNeighbors(int x, int y) {
        int count = 0;
        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                if (i == 0 && j == 0) continue;
                int neighborX = (x + i + width) % width;
                int neighborY = (y + j + height) % height;
                if (grid[neighborX][neighborY].alive) {
                    ++count;
                }
            }
        }
        return count;
    }

    int width, height;
    sf::RenderWindow window;
    std::vector<std::vector<Cell>> grid;
};

int main() {
    srand(static_cast<unsigned>(time(0)));

    int width, height;
    std::cout << "Enter width: ";
    std::cin >> width;
    std::cout << "Enter height: ";
    std::cin >> height;

    GameOfLife game(width, height);
    game.run();

    return 0;
}
