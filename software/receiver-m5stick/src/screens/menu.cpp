#include <screens/screens.h>
#include <screens/menu.h>
unsigned int menuItem = 0;

void batteryIcon(M5Canvas canvas, int32_t x, int32_t y)
{
    canvas.setCursor(x, y);
    canvas.printf("%d%%", getBatteryPercentage());
    x += 24;
    canvas.drawRect(x, y, 25, 7, TFT_DARKGREY);
    int blocks = getBatteryPercentage() / 34 + 1;
    for (int i = 0; i < blocks; i++)
    {
        canvas.fillRect(x + 1 + (i * 8), y + 1, 7, 5, TFT_DARKGREEN);
        if (i < 2)
        {
            canvas.drawLine(x + 1 + (i * 8) + 7, y + 1, x + 1 + (i * 8) + 7, y + 6, TFT_LIGHTGRAY);
        }
    }
}

void textLine(M5Canvas canvas, int x, int y, char *text, int index = -1)
{
    const int backColor = TFT_DARKCYAN;
    canvas.setTextColor(TFT_WHITE, TFT_BLACK);
    if (index >= 0 && menuItem == index)
    {
        canvas.setTextColor(TFT_WHITE, backColor);
        canvas.fillRect(x, y, DISPLAY_WIDTH, 11, backColor);
    }
    canvas.setCursor(x + 3, y + 2);
    canvas.printf(text);
}

M5Canvas menuScreen()
{
    M5Canvas canvas;
    canvas.createSprite(DISPLAY_WIDTH, DISPLAY_HEIGHT);
    //canvas.setFont(&fonts::Font2);
    if (M5.BtnA.wasPressed())
    {
        switch (menuItem)
        {
        case 0:
            settings.setEnableMiniOverview(!settings.getEnableMiniOverview());
            break;
        case 1:
            settings.setEnableLED(!settings.getEnableLED());
            break;
        case 2:
            settings.setBrightness((settings.getBrightness() - 7 + 1) % 9 + 7);
            break;
        case 3:
            state = StateShowTally;
            break;
        default:
            break;
        }
    }
    if (M5.BtnB.wasPressed())
    {
        menuItem = (menuItem + 1) % 4;
    }
    int scroll = 0;
    if ((26+(menuItem+4)*12) > DISPLAY_HEIGHT)
    {
        scroll = -1 * menuItem*12;
    }
    canvas.fillScreen(TFT_BLACK);
    int i = 0;
    int y = 13 + scroll;
    char idLine[7];
    sprintf(idLine, "ID: %d", settings.getID() + 1);
    textLine(canvas, 0, y, idLine);
    y += 12;
    char ipLine[20];
    sprintf(ipLine, "IP: %s", WiFi.localIP().toString().c_str());
    textLine(canvas, 0, y, ipLine);
    y += 12;
    char rssi[15];
    sprintf(rssi, "RSSI: %d dBm", WiFi.RSSI());
    textLine(canvas, 0, y, rssi);
    y += 12;
    char miniIcons[16];
    sprintf(miniIcons, "Mini Icons: %s", settings.getEnableMiniOverview() ? "yes" : "no");
    textLine(canvas, 0, y, miniIcons, i++);
    y += 12;
    char led[9];
    sprintf(led, "LED: %s", settings.getEnableLED() ? "yes" : "no");
    textLine(canvas, 0, y, led, i++);
    y += 12;
    // char backLed[14];
    // sprintf(backLed, "Back LED: %s", settings.getEnableLED() ? "yes" : "no");
    // textLine(canvas, 0, y, backLed, i++);
    // y += 12;
    char br[15];
    sprintf(br, "Brightness: %d", settings.getBrightness());
    textLine(canvas, 0, y, br, i++);
    y += 12;
    y += 3;
    textLine(canvas, 0, y, (char *)"Exit", i++);

    // Draw Menu Bar
    canvas.fillRect(0, 0, DISPLAY_WIDTH, 11, TFT_WHITE);
    canvas.setTextColor(TFT_BLACK, TFT_WHITE);
    batteryIcon(canvas, DISPLAY_WIDTH-50, 2);
    canvas.setCursor(3, 2);
    canvas.setTextSize(1);
    canvas.print("Settings");

    return canvas;
}