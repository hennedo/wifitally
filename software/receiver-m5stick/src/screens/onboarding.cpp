#include <screens/onboarding.h>

M5Canvas onboardingScreen(char* hostname)
{
    int x = DISPLAY_WIDTH*0.032;
    M5Canvas canvas;
    canvas.createSprite(DISPLAY_WIDTH, DISPLAY_HEIGHT);
    canvas.setFont(&fonts::Font2);
    canvas.fillScreen(TFT_BLACK);
    #ifndef M5STICKCPLUS
    canvas.setTextSize(1.5);
    canvas.setTextColor(TFT_WHITE, TFT_BLACK);
    canvas.setCursor(x, 5);
    canvas.print("Onboarding");
    canvas.setTextSize(0.9);
    canvas.setCursor(x, 25);
    canvas.print("Connect to SSID:");
    canvas.setCursor(x, 37);
    canvas.setTextColor(TFT_RED);
    canvas.print(hostname);
    canvas.setTextColor(TFT_WHITE, TFT_BLACK);
    canvas.setCursor(x, 48);
    canvas.print("and go to:");
    canvas.setCursor(x, 60);
    #endif
    #ifdef M5STICKCPLUS
    canvas.setTextSize(2);
    canvas.setTextColor(TFT_WHITE, TFT_BLACK);
    canvas.setCursor(x, 10);
    canvas.print("Onboarding");
    canvas.setTextSize(1);
    canvas.setCursor(x, 40);
    canvas.print("Connect to SSID:");
    canvas.setCursor(x, 55);
    canvas.setTextColor(TFT_RED);
    canvas.print(hostname);
    canvas.setTextColor(TFT_WHITE, TFT_BLACK);
    canvas.setCursor(x, 70);
    canvas.print("and go to:");
    canvas.setCursor(x, 85);
    #endif
    canvas.setTextColor(TFT_RED);
    canvas.print("http://");
    WiFi.softAPIP().printTo(canvas);
    return canvas;
}