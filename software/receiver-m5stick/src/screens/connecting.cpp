#include <screens/connecting.h>

M5Canvas connectingScreen()
{
    int y = DISPLAY_HEIGHT/2;
    int x = DISPLAY_WIDTH*0.032;
    M5Canvas canvas;
    canvas.createSprite(DISPLAY_WIDTH, DISPLAY_HEIGHT);
    canvas.setFont(&fonts::Font2);
    canvas.fillScreen(TFT_BLACK);
    canvas.setTextSize(2);
    canvas.setTextColor(TFT_WHITE, TFT_BLACK);
    canvas.setCursor(x, y-15);
    canvas.print("Connecting");
    int dotCount = (millis() % 1000) / 334;
    for (int i = 0; i < dotCount; i++)
        canvas.print(".");
    canvas.setTextSize(0.9);
    canvas.setCursor(x, y+15);
    canvas.printf("To SSID: %s", settings.getSSID());
    return canvas;
}