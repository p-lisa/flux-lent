#pragma once 

#include <Arduino.h>

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

class LCD{
    public:

    struct Params{
        int sck, tx, cs, res, dc;
    };

    LCD(Params p){
        pin_sck = p.sck;
        pin_tx = p.tx;
        tft = new Adafruit_ST7735(p.cs, p.dc, p.res); // new pour crée moi un bloc mémoire sur le hip qui crée un objet adafruit. chiant: elle va jamais disparaitre toute seule, il faudra le del manuellement. 
    };

    ~LCD(){ // fct called when object go out of scope ou est delete
        delete tft;
    }

    void init(){
        SPI.begin();
        SPI.setSCK(pin_sck);
        SPI.setTX(pin_tx);

        tft->initR(INITR_GREENTAB);
        tft->setRotation(1);
        tft->fillScreen(ST77XX_BLACK);
    }

    void display_img(const uint16_t *img, int x = 0, int y = 0, int w = 128, int h = 128) { // *: pointeur vers des uint16t
        tft->startWrite(); // start tnx spi
        tft->setAddrWindow(x, y, w, h); // define rect
        // tft.writePixels(img, w * h); // >> not working, needs cast
        tft->writePixels((uint16_t*)img, w * h); // uint16_t: pixel 16 bits, (uint16_t*) is a cast, remove the const. writePixels(pixel start, n of pixels)
        tft->endWrite(); // end tnx (start/end to avoid starting bus for eahc pixel)
    }

    void clear_rect(int x, int y, int w, int h) {
        tft->fillRect(x, y, w, h, ST77XX_BLACK);
    }

    private:
    int pin_sck;
    int pin_tx;
    Adafruit_ST7735* tft;
};
