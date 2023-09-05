#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include "portable-file-dialogs.h"

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
            // std::cout << "ERROR:: Cannot open font" << std::endl;
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

    void updateBuffer(std::string content) {
        s_textbuff = content;

        // update sfml_text according to buffer
        sfml_text.setString(s_textbuff);

        updateCursor();
    }

    void addCharacter(sf::Uint32 unicode) {
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

    const std::string getText() {
        return sfml_text.getString();
    }
};

class FileHandler {
private:
    std::string filename;

public:
    FileHandler() {
        filename = "";
    }
    FileHandler(std::string name) {
        filename = name;
    }

    void setFilename(std::string name) {
        this->filename = name;
    }

    // Function to save text content to a file
    void saveToFile(const std::string& content)
    {
        if (this->filename == "") {
            return;
        }

        std::ofstream file(filename);
        if (file.is_open())
        {
            file << content;
            file.close();
            std::cout << "File saved: " << filename << std::endl;
        }
        else
        {
            // set filename to null as invalid name of file can also cause error
            this->filename = "";
            std::cerr << "Unable to save file: " << filename << std::endl;
        }

    }
    
    std::string getContentFromFile(const std::string filename) {
        if (filename == "") {
            std::cerr << "Error: filename is null " << std::endl;
            return "";
        }

        // Create an input file stream
        std::ifstream inputFile(filename);

        // Check if the file is open
        if (!inputFile.is_open()) {
            std::cerr << "Error: Unable to open file: " << filename << std::endl;
            return ""; // Exit with an error code
        }

        // Create a string stream to hold the file content
        std::stringstream buffer;
        buffer << inputFile.rdbuf();

        // Close the input file stream
        inputFile.close();

        // Extract the content from the string stream
        std::string fileContent = buffer.str();

        // Now, 'fileContent' contains the content of the file

        // You can use 'fileContent' as needed, for example, display it or process it

        std::cout << "File content:\n" << fileContent << std::endl;

        return fileContent;
    }

    bool hasFilename() {
        if (filename == "") {
            return false;
        }
        return true;
    }
};

void run() {
    sf::RenderWindow window(sf::VideoMode(1080, 720), "Lipi");
   
    sf::Clock clock;
    float cursorBlinkTime = 0.f;

    FileHandler file;
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
                editor.addCharacter(event.text.unicode);
                editor.updateCursor();
            }
            
            // handle keypressed
            if (event.type == sf::Event::KeyPressed)
            {
                // Check for ctrl is pressed or not
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                {
                    // Check for ctrl + x for exit
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::X)) {
                        //std::cout << "Exiting... " << std::endl;
                        window.close();
                    }

                    // Check for ctrl + s for saving text to file
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {

                        // File save
                        if (!file.hasFilename()) {
                            auto f = pfd::save_file("Choose file to save",
                                pfd::path::home() + pfd::path::separator() + "untitled.txt",
                                { "Text Files (.txt .text)", "*.txt *.text" },
                                pfd::opt::force_overwrite);
                            std::cout << "Selected file: " << f.result() << "\n";
                            file.setFilename(f.result());
                        }

                        file.saveToFile(editor.getText());
                    }

                    // open a file ctrl + o
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::O)) {
                        // File open
                        auto f = pfd::open_file("Choose files to read", pfd::path::home(),
                            { "Text Files (.txt .text)", "*.txt *.text",
                              "All Files", "*" },
                            pfd::opt::multiselect);
                        std::cout << "Selected files:";
                        for (auto const& name : f.result()) {
                            std::cout << " " + name;
                            file.setFilename(name);
                            editor.updateBuffer(file.getContentFromFile(name));
                        }
                        std::cout << "\n";
                    }
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