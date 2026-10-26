#pragma once

#include <Arduino.h>
#include <stdint.h>

class ImageFX {
public:
    static void darken(
        uint16_t* img, 
        int count, 
        int imgSize = 16384, 
        uint8_t amount = 1,
        bool randomPixels = true) { // static fct because toobox of fct, belongs to the class itself, not to be used on an obj  
        for (int i = 0; i < count; i++) {

            // RGB on 16bits: RRRRRGGGGGGBBBBB (green 6 bits because eyes more sensitive) 

            int idx = randomPixels ? random(imgSize) : i;
            uint16_t c = img[idx]; // gets the pixel
            
            // &: and binaire. 0x1F: hexadecimal 0x1F = 00011111
            uint8_t r = (c >> 11) & 0x1F; // extract the red: c>>11 translate red bits to he right. & 0x1F keeps on 5 bits
            uint8_t g = (c >> 5)  & 0x3F; // "& 0x3F" = masque binaire: keeps some bits, ignores others
            uint8_t b = c & 0x1F;

            // r = (condition) ? value if yes : value if no;
            // on utilise pas max() car r est uint8t unsigned. 3 - 5 donnerait pas -2 mais overflow en 254
            r = (r > amount) ? r - amount : 0; // either decrease, or put to 0 (avoid negative)
            g = (g > amount) ? g - amount : 0;
            b = (b > amount) ? b - amount : 0;

            img[idx] = (r << 11) | (g << 5) | b; // reconstructs the pixel, where | combines the bits parts
        }
    }

    static void brighten(
        uint16_t* img, 
        int count, int 
        imgSize = 16384, 
        uint8_t amount = 1,
        bool randomPixels = true) {
        for (int i = 0; i < count; i++) {

            int idx = randomPixels ? random(imgSize) : i;
            uint16_t c = img[idx];

            uint8_t r = (c >> 11) & 0x1F;
            uint8_t g = (c >> 5)  & 0x3F;
            uint8_t b = c & 0x1F;

            r = min(31, r + amount); // avoid going over max possible value
            g = min(63, g + amount);
            b = min(31, b + amount);

            img[idx] = (r << 11) | (g << 5) | b;
        }
    }

    static void gravitySmoke(uint16_t* img,
                             uint16_t* src,
                             bool* moved,
                             int width = 128,
                             int height = 128,
                             int smokeThreshold = 90, // brightness 0-125
                             int moveChance = 70, // percent, for randomness
                             int gravityMode = 0, // 0 Neighbours or 1 Directional
                             int searchRadius = 3) {

        int count = width * height;

        // copy frame to src buffer > avoid artefact > we read in src and write in img              
        for (int i = 0; i < count; i++) {
            src[i] = img[i];
            moved[i] = false; // reset
        }

        int border = (gravityMode == 0) ? 1 : searchRadius;

        for (int y = border; y < height - border; y++) {
            for (int x = border; x < width - border; x++) {

                int idx = y * width + x;

                // skip pixels if already swapped
                if (moved[idx]) continue;

                // randomness: to not move this round
                if (random(100) >= moveChance) continue;

                uint16_t c = src[idx];
                int b = brightness565(c); // get brightness of current pix

                // default: stay in place
                int targetIdx = idx;

                // smoke behaviour
                // smokeThreshold is brightness 0-125
                if (b > smokeThreshold) {
                    int dir = random(3) - 1; // -1, 0, +1, randomness: dir
                    targetIdx = (y - 1) * width + (x + dir);
                } else {

                    if (gravityMode == 0) {
                        targetIdx = gravityNeighbours(
                            src,
                            moved,
                            idx,
                            width,
                            b
                        );
                    } else {
                        targetIdx = gravityDirectional(
                            src,
                            moved,
                            x,
                            y,
                            idx,
                            width,
                            b,
                            searchRadius
                        );
                    }
                }

                if (targetIdx != idx && !moved[targetIdx]) {
                    img[targetIdx] = src[idx];
                    img[idx] = src[targetIdx];

                    moved[idx] = true;
                    moved[targetIdx] = true;
                }
            }
        }
    }

private:

    static int brightness565(uint16_t c) {
        uint8_t r = (c >> 11) & 0x1F;
        uint8_t g = (c >> 5)  & 0x3F;
        uint8_t b = c & 0x1F;

        return r + g + b;
    }

    static int gravityNeighbours(uint16_t* src,
                              bool* moved,
                              int idx,
                              int width,
                              int b) {

        // gravity behaviour
        int neighbors[4] = {
            idx - 1, // left
            idx + 1, // right
            idx - width, // up
            idx + width // down
        };

        // randomness: neighbour checking order
        int start = random(4);

        // default: stay in place
        int darkestIdx = idx;
        int darkestBrightness = b;

        // inspect neighbours
        for (int k = 0; k < 4; k++) {

            int ni = neighbors[(start + k) % 4];

            if (moved[ni]) continue; // skip if already moved pixls

            int nb = brightness565(src[ni]);

            // randomness: sometimes accept almost darkest neighbors too
            if (nb < darkestBrightness || random(100) < 10) {
                darkestBrightness = nb;
                darkestIdx = ni;
            }
        }

        return darkestIdx;
    }

    static int gravityDirectional(uint16_t* src,
                                  bool* moved,
                                  int x,
                                  int y,
                                  int idx,
                                  int width,
                                  int b,
                                  int searchRadius) {

        // gravity behaviour
        // inspect 8 directions
        int directions[8][2] = {
            {-1,  0}, // left
            { 1,  0}, // right
            { 0, -1}, // up
            { 0,  1}, // down
            {-1, -1}, // up-left
            { 1, -1}, // up-right
            {-1,  1}, // down-left
            { 1,  1}  // down-right
        };

        // randomness: direction checking order
        int start = random(8);

        // default: stay in place
        int darkestIdx = idx;
        int darkestBrightness = b;

        // inspect directions
        for (int k = 0; k < 8; k++) {

            int d = (start + k) % 8;

            int dirX = directions[d][0];
            int dirY = directions[d][1];

            int totalBrightness = 0;
            int samples = 0;

            // inspect pix a bit further in that dir
            for (int step = 1; step <= searchRadius; step++) {

                int nx = x + dirX * step;
                int ny = y + dirY * step;

                int ni = ny * width + nx;

                if (moved[ni]) continue; // skip if already moved

                int nb = brightness565(src[ni]);

                totalBrightness += nb;
                samples++;
            }

            if (samples == 0) continue;

            int averageBrightness = totalBrightness / samples;

            // randomness: sometimes accept almost darkest dirs too
            if (averageBrightness < darkestBrightness || random(100) < 10) {

                darkestBrightness = averageBrightness;

                // move only one pixel in the chosen direction (TODO as arg?)
                darkestIdx = (y + dirY) * width + (x + dirX);
            }
        }

        return darkestIdx;
    }
};