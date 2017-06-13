#include <iostream>
#include <thread>
#include <SFML/Graphics.hpp>
#include "mean_filter_cpp_ugly.h"

//#define USE_CPP_VERSION

using namespace std;

extern "C" int mean_filter(int width, int height, int window, const sf::Uint8 *inImg, const sf::Uint8 *outImg);

int main(int argc, char *argv[])
{
    std::string inputFileName = "../images/input_big.bmp";
    int inputWindow = 9;

    std::cout << "MeanFilter" << std::endl;
    std::cout << "Podaj nazwe pliku wejsciowego" << std::endl;
    std::cin >> inputFileName;
    std::cout << "Podaj rozmiar okna (liczba nieparzysta <3,15>)" << std::endl;
    std::cin >> inputWindow;

    if (inputWindow > 15 || inputWindow % 2 == 0) {
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
    outImg.create(srcImgSize.x, srcImgSize.y, sf::Color{0,0,0,0});
    sf::Texture imageTexture, backgroundImageTexture;
    backgroundImageTexture.loadFromImage(srcImg);
    imageTexture.loadFromImage(outImg);
    sf::Sprite imageSprite(imageTexture), backgroundImageSprite(backgroundImageTexture);

    // window size and scaling
    if (srcImgSize.x > sf::VideoMode::getDesktopMode().width || srcImgSize.y > sf::VideoMode::getDesktopMode().height) {
        imageSprite.setScale(sf::VideoMode::getDesktopMode().width / (srcImgSize.x * 1.5f),
                             sf::VideoMode::getDesktopMode().height / (srcImgSize.y * 1.5f));
        backgroundImageSprite.setScale(sf::VideoMode::getDesktopMode().width / (srcImgSize.x * 1.5f),
                                       sf::VideoMode::getDesktopMode().height / (srcImgSize.y * 1.5f));
    }
    unsigned int windowWidth = static_cast<unsigned int>(srcImgSize.x < sf::VideoMode::getDesktopMode().width
                                                         ? srcImgSize.x : srcImgSize.x *
                                                                          imageSprite.getScale().x);
    unsigned int windowHeight = static_cast<unsigned int>(srcImgSize.y < sf::VideoMode::getDesktopMode().height
                                                          ? srcImgSize.y : srcImgSize.y *
                                                                           imageSprite.getScale().y);

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "MeanFilter");

#ifdef USE_CPP_VERSION
    std::thread threadObj(mean_filter_cpp, srcImgSize.x, srcImgSize.y, inputWindow, std::ref(srcImg), std::ref(outImg));
#else
    std::thread threadObj(mean_filter, srcImgSize.x, srcImgSize.y, inputWindow, srcImg.getPixelsPtr(), outImg.getPixelsPtr());
#endif

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        imageTexture.loadFromImage(outImg);
        window.clear();
        window.draw(backgroundImageSprite);
        window.draw(imageSprite);
        window.display();
    }

    if (threadObj.joinable()) {
        threadObj.join();
    }
    outImg.saveToFile("../output.bmp");

    return 0;
}
