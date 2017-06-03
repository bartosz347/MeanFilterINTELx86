#include <iostream>
#include <SFML/Graphics.hpp>
#include "mean_filter_cpp_ugly.h"

#define USE_ASSEMBLER_VERSION 1

using namespace std;

extern "C" int mean_filter(int width, int height, int window, const sf::Uint8 *inImg, const sf::Uint8 *outImg);


int main(int argc, char *argv[])
{
    std::string inputFileName = "../images/input.bmp";
    int inputWindow = 3;

    std::cout << "MeanFilter" << std::endl;
    std::cout << "Podaj nazwe pliku wejsciowego" << std::endl;
    std::cin >> inputFileName;
    std::cout << "Podaj rozmiar okna (liczba nieparzysta <3-15>)" << std::endl;
    std::cin >> inputWindow;

    if(inputWindow > 15 || inputWindow % 2 == 0) {
        std::cout << "nieprawidlowy rozmiar okna" << std::endl;
        return -1;
    }

    sf::Image srcImg;
    sf::Image outImg;

    if (!srcImg.loadFromFile(inputFileName)) {
        std::cout << "failed to open image" << std::endl;
        return -1;
    }

    sf::Vector2u srcImgSize = srcImg.getSize();
    sf::RenderWindow window(sf::VideoMode(srcImgSize.x, srcImgSize.y), "MeanFilter");

    outImg.create(srcImgSize.x, srcImgSize.y);
    sf::Texture imageTexture;
    imageTexture.loadFromImage(outImg);
    sf::Sprite imageSprite(imageTexture);

    if (USE_ASSEMBLER_VERSION) {
        mean_filter(srcImgSize.x, srcImgSize.y, inputWindow, srcImg.getPixelsPtr(), outImg.getPixelsPtr());
    } else {
        mean_filter_cpp(srcImgSize.x, srcImgSize.y, inputWindow, srcImg, outImg);
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




