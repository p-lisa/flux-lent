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

    // static void gravity(uint16_t* img, int width = 128, int height = 128) {

    //     for (int y = 1; y < height - 1; y++) {
    //         for (int x = 1; x < width - 1; x++) {

    //             // convert 2D coordinates into 1D array index
    //             int idx = y * width + x;
    //             uint16_t c = img[idx];

    //             uint8_t r = (c >> 11) & 0x1F;
    //             uint8_t g = (c >> 5)  & 0x3F;
    //             uint8_t b = c & 0x1F;

    //             int brightness = r + g + b;

    //             int darkestIdx = idx;
    //             int darkestBrightness = brightness;

    //             // check left
    //             int leftIdx = idx - 1;
    //             uint16_t cl = img[leftIdx]; // neighbour color
    //             int bl = // neighbour brightness
    //                 ((cl >> 11) & 0x1F) +
    //                 ((cl >> 5)  & 0x3F) +
    //                 (cl & 0x1F);

    //             if (bl < darkestBrightness) {  // if left neighbor darker, remember it
    //                 darkestBrightness = bl;
    //                 darkestIdx = leftIdx;
    //             }

    //             // check right
    //             int rightIdx = idx + 1;
    //             uint16_t cr = img[rightIdx];
    //             int br =
    //                 ((cr >> 11) & 0x1F) +
    //                 ((cr >> 5)  & 0x3F) +
    //                 (cr & 0x1F);

    //             if (br < darkestBrightness) {
    //                 darkestBrightness = br;
    //                 darkestIdx = rightIdx;
    //             }

    //             // check up
    //             int upIdx = idx - width;
    //             uint16_t cu = img[upIdx];
    //             int bu =
    //                 ((cu >> 11) & 0x1F) +
    //                 ((cu >> 5)  & 0x3F) +
    //                 (cu & 0x1F);

    //             if (bu < darkestBrightness) {
    //                 darkestBrightness = bu;
    //                 darkestIdx = upIdx;
    //             }

    //             // check down
    //             int downIdx = idx + width;
    //             uint16_t cd = img[downIdx];
    //             int bd =
    //                 ((cd >> 11) & 0x1F) +
    //                 ((cd >> 5)  & 0x3F) +
    //                 (cd & 0x1F);

    //             if (bd < darkestBrightness) {
    //                 darkestBrightness = bd;
    //                 darkestIdx = downIdx;
    //             }

    //             // move pixel one step
    //             if (darkestIdx != idx) {
    //                 uint16_t temp = img[idx];
    //                 img[idx] = img[darkestIdx];
    //                 img[darkestIdx] = temp;
    //             }
    //         }
    //     }
    // }

    static int brightness565(uint16_t c) {
        uint8_t r = (c >> 11) & 0x1F;
        uint8_t g = (c >> 5)  & 0x3F;
        uint8_t b = c & 0x1F;

        return r + g + b;
    }

    // static void gravity(uint16_t* img,
    //                     uint16_t* src,
    //                     bool* moved,
    //                     int width,
    //                     int height) {

    //     int count = width * height;

    //     for (int i = 0; i < count; i++) {
    //         src[i] = img[i];
    //         moved[i] = false;
    //     }

    //     for (int y = 1; y < height - 1; y++) {
    //         for (int x = 1; x < width - 1; x++) {

    //             int idx = y * width + x;

    //             if (moved[idx]) continue;

    //             uint16_t c = src[idx];
    //             int currentBrightness = brightness565(c);

    //             int darkestIdx = idx;
    //             int darkestBrightness = currentBrightness;

    //             int neighbors[4] = {
    //                 idx - 1,
    //                 idx + 1,
    //                 idx - width,
    //                 idx + width
    //             };

    //             for (int n = 0; n < 4; n++) {
    //                 int ni = neighbors[n];

    //                 if (moved[ni]) continue;

    //                 int b = brightness565(src[ni]);

    //                 if (b < darkestBrightness) {
    //                     darkestBrightness = b;
    //                     darkestIdx = ni;
    //                 }
    //             }

    //             if (darkestIdx != idx) {
    //                 img[darkestIdx] = src[idx];
    //                 img[idx] = src[darkestIdx];

    //                 moved[idx] = true;
    //                 moved[darkestIdx] = true;
    //             }
    //         }
    //     }
    // }

    static void gravitySmoke(uint16_t* img,
                            uint16_t* src,
                            bool* moved,
                            int width = 128,
                            int height = 128,
                            int smokeThreshold = 90, // btw 0 and 125
                            int moveChance = 70) { // percent, for randomness

        int count = width * height;

        // copy current frame into source buffer
        // we read from src and write into img
        // this avoids scanline artefacts
        for (int i = 0; i < count; i++) {
            src[i] = img[i];
            // reset movement tracking
            moved[i] = false;
        }

        for (int y = 1; y < height - 1; y++) {
            for (int x = 1; x < width - 1; x++) {

                int idx = y * width + x;

                // skip pixels if already swapped
                if (moved[idx]) continue;

                // randomness: chance to not move this pixel
                if (random(100) >= moveChance) continue;

                uint16_t c = src[idx];
                int b = brightness565(c); // get brightness of current pixel

                // default: stay in place
                int targetIdx = idx;

                // smoke behaviour
                // smokeTreshold is a treshold of brightness, from 0 to 125
                if (b > smokeThreshold) {
                    int dir = random(3) - 1; // -1, 0, +1
                    targetIdx = (y - 1) * width + (x + dir);
                } else {
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

                        if (moved[ni]) continue; // skip if already moved pixels

                        int nb = brightness565(src[ni]);

                        // randomness: sometimes accept almost-darker neighbors too
                        if (nb < darkestBrightness || random(100) < 10) {
                            darkestBrightness = nb;
                            darkestIdx = ni;
                        }
                    }

                    targetIdx = darkestIdx;
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
};