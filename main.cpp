#include <iostream>
#include <SFML/Graphics.hpp>
#include "mean_filter_cpp_ugly.h"

#define USE_ASSEMBLER_VERSION 1

using namespace std;

extern "C" int mean_filter(int width, int height, int window, const sf::Uint8 *inImg, const sf::Uint8 *outImg);


int main(int argc, char *argv[])
{
    sf::Image srcImg;
    sf::Image outImg;

    if (!srcImg.loadFromFile("../images/input.bmp")) {
        std::cout << "failed to open image" << std::endl;
        return -1;
    }

    sf::Vector2u BMPsize = srcImg.getSize();
    sf::RenderWindow window(sf::VideoMode(BMPsize.x, BMPsize.y), "MeanFilter");

    outImg.create(BMPsize.x, BMPsize.y);
    sf::Texture imageTexture;
    imageTexture.loadFromImage(outImg);
    sf::Sprite imageSprite(imageTexture);

    if (USE_ASSEMBLER_VERSION) {
        mean_filter(BMPsize.x, BMPsize.y, 3, srcImg.getPixelsPtr(), outImg.getPixelsPtr());
    } else {
        mean_filter_cpp(BMPsize.x, BMPsize.y, 3, srcImg, outImg);
    }


    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            imageTexture.loadFromImage(outImg);
            window.clear();
            window.draw(imageSprite);
            window.display();
        }
    }

    outImg.saveToFile("../output.bmp");
    return 0;
}




