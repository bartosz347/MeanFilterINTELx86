#include <iostream>
#include <SFML/Graphics.hpp>
using namespace std;

int main(int argc, char* argv[]) {

    sf::Image BMPimage;
    BMPimage.loadFromFile("../input_big.bmp");

    sf::Vector2u BMPsize= BMPimage.getSize();
    sf::RenderWindow window(sf::VideoMode(BMPsize.x, BMPsize.y),"MeanFilter");

    sf::Texture imageTexture;
    imageTexture.loadFromImage(BMPimage);
    sf::Sprite imageSprite(imageTexture);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        imageTexture.loadFromImage(BMPimage);
        window.clear();
        window.draw(imageSprite);
        window.display();
    }

    return 0;
}