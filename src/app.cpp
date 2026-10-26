#include "joystick.h"
#include "lcd.h"
#include "assets/assets.h"
#include "transformation.h"
#include "fx.h"

#include <Arduino.h>

Joystick joystick(26, 27, 28);
LCD lcd(LCD::Params{.sck = 2, .tx = 3, .cs = 1, .res = 5, .dc = 4});

// Transformation circle_transform(Transformation::Params{
//     .img = circle,
//     .img_w = 128,
//     .img_h = 128,
//     .screen_w = 128,
//     .screen_h = 128,
//     .start_x = 1000,
//     .start_y = 1000,
//     .deadzone = 200,
//     .step = 2
// });

void app_init() {
    Serial.begin(9600);

    joystick.init();

    lcd.init();
    lcd.display_img(yogurt);
}

void app_update() {
    int x = joystick.xCentered();
    int y = joystick.yCentered();
    bool sw = joystick.pressed();

    Serial.printf("X=%d Y=%d SW=%d\n", x, y, sw);

    // circle_transform.update(lcd, x, y);

    if (y < 900) {
        ImageFX::darken(yogurt, 2000); // max 16384
        lcd.display_img(yogurt);
    }

    if (y > 1300) {
        ImageFX::brighten(yogurt, 2000); // max 16384
        lcd.display_img(yogurt);
    }

    if (sw) {
        // ImageFX::gravity(circle);

        // the arrays are created here otherwise in the fct it would allocate them on the stack
        // for ex: gravityMoved is 16kb and 
        // gravitySrc is 16384 * 2 bytes = 32kb
        // can even called them as static to avoid stack allocation entirely.
        // also: if they were created in the fct, it would repeatedly allocate huge buffers at each call >> inefficient
        uint16_t gravitySrc[16384];
        bool gravityMoved[16384];
        
        ImageFX::gravitySmoke(yogurt, gravitySrc, gravityMoved, 128, 128);
        lcd.display_img(yogurt);
    }

    // circle_transform.translate(lcd, x, y);

    // sleep_ms(30);
}

void app_run() {
    app_init();

    while (true) {
        app_update();
    }
}