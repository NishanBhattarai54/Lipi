#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <iostream>

int main()
{
    sf::RenderWindow window(sf::VideoMode(1080, 720), "Lipi");

    // Create a graphical text to display
    sf::Font font;
    if (!font.loadFromFile("res/fonts/arial.ttf"))
        return EXIT_FAILURE;
    sf::Text text("", font, 24);
    std::string buff = "";

    sf::RectangleShape cursor(sf::Vector2f(1.25f, 24.f));
    float cursorYOffset = 24.f - 20.f;
    float cursorXOffset = 24.f - 23.f;
    float cursorX = text.findCharacterPos(buff.length()).x;
    float cursorY = text.findCharacterPos(buff.length()).y + cursorYOffset;
    cursor.setPosition(cursorX, cursorY);

    

    sf::Clock clock;
    float cursorBlinkTime = 0.f;

    while (window.isOpen())
    {
        float fElapsedTime = clock.getElapsedTime().asSeconds();
        clock.restart();
        cursorBlinkTime += fElapsedTime;

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::TextEntered) {

                if (event.text.unicode >= 32 && event.text.unicode < 127) {
                    buff += event.text.unicode;
                    text.setString(buff);
                    cursorX = text.findCharacterPos(buff.length()).x + cursorXOffset;
                    cursorY = text.findCharacterPos(buff.length()).y + cursorYOffset;
                    cursor.setPosition(cursorX, cursorY);
                }
                else if (event.text.unicode == 8) {
                    if (!buff.empty()) {
                        buff.pop_back();
                        text.setString(buff);
                        cursorX = text.findCharacterPos(buff.length()).x + cursorXOffset;
                        cursorY = text.findCharacterPos(buff.length()).y + cursorYOffset;
                    }
                    cursor.setPosition(cursorX, cursorY);
                }
                else if (event.text.unicode == 13) {
                    buff += "\n";
                    text.setString(buff);
                    cursorX = text.findCharacterPos(buff.length()).x + cursorXOffset;
                    cursorY = text.findCharacterPos(buff.length()).y + cursorYOffset;
                    cursor.setPosition(cursorX, cursorY);
                }
            }
        }

        window.clear();

        // Draw the string
        window.draw(text);
        
        // Blink the cursor
        if (cursorBlinkTime  > 0.5f) {
            window.draw(cursor);
        }
        
        if (cursorBlinkTime > 1.f) {
            cursorBlinkTime = 0.f;
        }
        
        window.display();
    }

    return 0;
}