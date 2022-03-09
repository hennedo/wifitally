#include <screens/booting.h>

M5Canvas bootingScreen()
{
    M5Canvas canvas;
    canvas.createSprite(DISPLAY_WIDTH, DISPLAY_HEIGHT);
    canvas.fillScreen(TFT_BLACK);
    canvas.setTextSize(2);
    canvas.setTextColor(TFT_WHITE, TFT_BLACK);
    canvas.setCursor(5, 30);
    canvas.print("Booting");
    int dotCount = millis() / 300 % 3;
    for (int i = 0; i < dotCount; i++)
        canvas.print(".");

    return canvas;
}