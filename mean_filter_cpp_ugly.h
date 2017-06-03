#include <SFML/Graphics.hpp>

class Point {
public:
    int x;
    int y;
};

class PixelData {
public:
    int r;
    int g;
    int b;
    PixelData() {
        this->r = 0;
        this->g = 0;
        this->b = 0;
    }
    void divide(int n) {
        this->r /= n;
        this->g /= n;
        this->b /= n;
    }

};

void mean_filter_cpp(int width, int height, int window, const sf::Image &srcImg, sf::Image &outImg)
{
    int window_size = window / 2;

    Point kernel;
    kernel.x = 0;
    kernel.y = 0;
    PixelData sum;
    Point cursor;

    start_new_kernel:
    cursor.x = kernel.x - window_size;
    cursor.y = kernel.y - window_size;

    process_next_column_in_row:
    if (cursor.y < 0)
        cursor.y = 0;
    if (cursor.x < 0)
        cursor.x = 0;

    if (cursor.x < (kernel.x + window_size)) {
        // load double to xmm1
        sum.r += srcImg.getPixel(cursor.x, cursor.y).r;
        sum.g += srcImg.getPixel(cursor.x, cursor.y).g;
        sum.b += srcImg.getPixel(cursor.x, cursor.y).b;

        sum.r += srcImg.getPixel(cursor.x + 1, cursor.y).r;
        sum.g += srcImg.getPixel(cursor.x + 1, cursor.y).g;
        sum.b += srcImg.getPixel(cursor.x + 1, cursor.y).b;

        // xmm2 += xmm1

        cursor.x += 2;

        if(cursor.x < width && cursor.x <= (kernel.x + window_size))
            goto process_next_column_in_row;

    } else {
        // load single to xmm1
        sum.r += srcImg.getPixel(cursor.x, cursor.y).r;
        sum.g += srcImg.getPixel(cursor.x, cursor.y).g;
        sum.b += srcImg.getPixel(cursor.x, cursor.y).b;

        // xmm2 += xmm1
    }

    move_cursor_to_next_row_in_kernel:
    cursor.y += 1;
    cursor.x = kernel.x - window_size;
    if (cursor.y <= kernel.y + window_size && cursor.y < height)
        goto process_next_column_in_row;

    sum.divide(window * window);
    outImg.setPixel(kernel.x, kernel.y, sf::Color(sum.r, sum.g, sum.b));
    sum = PixelData{};

    kernel.x += 1;
    if(kernel.x >= width) { // end of row reached, moving kernel to next row
        kernel.x = 0;
        kernel.y += 1;
    }

    if(kernel.y < height)
        goto start_new_kernel;
}


