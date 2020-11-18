#pragma once 

#include <stdlib.h>
#include <cmath>

typedef unsigned char uchar;
typedef uchar pixel;

typedef union rgb_pixel {
    uchar data[3];
    struct {
        uchar r, g, b;
    } rgb;
} rgb_pixel;

//WS :
//  union rgb_pixel is not necessary for now

inline void sobel_operator(uchar* op, uchar* np, int width, int height)
{
    const int filter[] = { 1, 2, 1 };

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {

            int sum_x = 0, sum_y = 0;

            for (int row = -1; row < 2; row++) {
                for (int col = -1; col < 2; col++) {
                    if (x + row >= 0 && x + row < width && y + col >= 0 && y + col < height) {
                        sum_x += filter[row + 1] * col * op[width * (y + col) + x + row];
                        sum_y += filter[col + 1] * row * op[width * (y + col) + x + row];
                    }
                }
            }
            int color = sqrtf(sum_x * sum_x + sum_y * sum_y);
            if (color > 255) color = 255;
            else if (color < 0) color = 0;
            np[width * y + x] = (uchar)color;
        }
    }
}

inline void gaussian_blur_operator(pixel* op, pixel* np, int width, int height)
{
    const int filter[][5] = {
        { 2, 4, 5, 4, 2},
        { 4, 9,12, 9, 4},
        { 5,12,15,12, 5},
        { 4, 9,12, 9, 4},
        { 2, 4, 5, 4, 2}
    };
    const float divide_factor = 1 / 159.0f;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int sum = 0;
            for (int row = -2; row < 3; row++) {
                for (int col = -2; col < 3; col++) {
                    if (x + row >= 0 && x + row < width && y + col >= 0 && y + col < height) {
                        sum += filter[row + 2][col + 2] * op[width * (y + col) + x + row];
                    }
                }
            }

            int color = sum * divide_factor;
            if (color > 255) color = 255;
            else if (color < 0) color = 0;
            np[width * y + x] = (uchar)color;
        }
    }
}

inline void non_maximum_suppression_operator(pixel* op, pixel* np, int width, int height)
{
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            pixel current = op[width * y + x];

#define CHECK_NEIGHBOR(X, Y)                                        \
            if (x + X >= 0 && x + X < width && y + Y >= 0 && y + Y < height &&  \
                current < op[width * (y + Y) + x + X])                          \
            {                                                                   \
                np[width * y + x] = 0;                                          \
                continue;                                                       \
            }

            CHECK_NEIGHBOR(-1, 0)
                CHECK_NEIGHBOR(1, 0)
                CHECK_NEIGHBOR(0, -1)
                CHECK_NEIGHBOR(0, 1)

#undef CHECK_NEIGHBOR

                np[width * y + x] = current;
        }
    }
}

inline void double_threshold_operator(pixel* p, int width, int height)
{
    const pixel low = 0.2f * 256;
    const pixel high = 0.8f * 256;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            pixel current = p[width * y + x];
            if (current <= low)
            {
                p[width * y + x] = 0;
            }
            else if (current >= high)
            {
                p[width * y + x] = 255;
            }
            else
            {
                p[width * y + x] = 128;
            }
        }
    }
}