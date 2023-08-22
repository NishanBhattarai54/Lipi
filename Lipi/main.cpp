#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <iostream>

class Editor {
    sf::Font font;
    std::string s_textbuff;
    sf::RectangleShape cursor;
    float cursorYOffset;
    float cursorXOffset;
    float cursorX;
    float cursorY;
public: 
    sf::Text sfml_text;

    Editor() {
        if (!font.loadFromFile("res/fonts/arial.ttf")) {
            std::cout << "ERROR:: Cannot open font" << std::endl;
            return;
        }

        sfml_text.setFont(font);
        sfml_text.setCharacterSize(24);

        cursor.setSize(sf::Vector2(1.25f, 24.f));
        cursorYOffset = 24.f - 20.f;
        cursorXOffset = 24.f - 23.f;
        cursorX = 0;
        cursorY = 0;
        cursor.setPosition(cursorX, cursorY);
    }

    void updateText(sf::Uint32 unicode) {
        if (unicode >= 32 && unicode < 127) {
            s_textbuff += unicode;
            sfml_text.setString(s_textbuff);
        }
        else if (unicode == 8 && !s_textbuff.empty()) {
            s_textbuff.pop_back();
            sfml_text.setString(s_textbuff);
        }
        else if (unicode == 13) {
            s_textbuff += "\n";
            sfml_text.setString(s_textbuff);
        }
    }

    void updateCursor(float x = -1.f, float y = -1.f) {
        if (x == -1.f)
            cursorX = sfml_text.findCharacterPos(s_textbuff.length()).x + cursorXOffset;
        if (x == -1.f)
            cursorY = sfml_text.findCharacterPos(s_textbuff.length()).y + cursorYOffset;

        if (x != -1.f && y != -1.f) {
            cursorX = x + cursorXOffset;
            cursorY = y + cursorYOffset;
        }
        cursor.setPosition(cursorX, cursorY);
    }

    void draw(sf::RenderWindow& window, float& cursorBlinkTime) {
        // Draw the string
        window.draw(sfml_text);

        // Blink the cursor
        if (cursorBlinkTime > 0.5f) {
            window.draw(cursor);
        }

        if (cursorBlinkTime > 1.f) {
            cursorBlinkTime = 0.f;
        }
    }
};

void run() {
    sf::RenderWindow window(sf::VideoMode(1080, 720), "Lipi");
   
    sf::Clock clock;
    float cursorBlinkTime = 0.f;

    Editor editor;

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
                editor.updateText(event.text.unicode);
                editor.updateCursor();
            }
            
            // handle keypressed
            if (event.type == sf::Event::KeyPressed)
            {
                // Check for ctrl + x for exit
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::X) && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                {
                    std::cout << "Exiting... " << std::endl;
                    window.close();
                }
            }
        }

        window.clear();

        editor.draw(window, cursorBlinkTime);
        
        window.display();
    }
}

int main()
{
    run();

    return 0;
}