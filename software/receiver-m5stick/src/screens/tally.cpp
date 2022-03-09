#include <screens/tally.h>

int letterHeight = 18;
int letterWidth = 9;

void miniOverview(M5Canvas canvas, byte data[512], int32_t x, int32_t y, int32_t w, int32_t h)
{
    canvas.setTextSize(1);
    int spacingX = 4;
    if (w % 2 == 1) // width is odd, so set spacing to something odd.
    {
        spacingX = 3;
    }
    int thumbWidth = (w - spacingX) / 2; // 14
    int spacingY = 3;
    while ((h - 3 * spacingY) % 4 != 0)
    {
        spacingY += 1;
    }
    int thumbHeight = (h - 3 * spacingY) / 4;
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            int index = i + (2 * j);
            int col = TFT_BLACK;
            int x_i = x + (thumbWidth + spacingX) * i;
            int y_i = y + (thumbHeight + spacingY) * j;
            if (data[index] == '1')
                col = TFT_GREEN;
            if (data[index] == '2')
                col = TFT_RED;
            canvas.fillRect(x_i + 1, y_i + 1, thumbWidth - 2, thumbHeight - 2, col);
            canvas.setCursor(x_i + ((thumbWidth - letterWidth) / 2) + 1, y_i + ((thumbHeight - letterHeight) / 2) + 1);
            canvas.setTextColor(TFT_WHITE, col);
            canvas.print(index + 1);
            if (settings.getID() == index)
                canvas.drawRect(x_i, y_i, thumbWidth, thumbHeight, TFT_YELLOW);
            else
                canvas.drawRect(x_i, y_i, thumbWidth, thumbHeight, TFT_DARKGREY);
        }
    }
}

M5Canvas tallyScreen(byte packetBuffer[512])
{
    M5Canvas canvas;
    canvas.setFont(&fonts::Font2);
    canvas.createSprite(DISPLAY_WIDTH, DISPLAY_HEIGHT);
    canvas.fillScreen(TFT_BLACK);
    // Set own tally color
    int ownColor = TFT_BLACK;
    if (packetBuffer[settings.getID()] == '1')
        ownColor = TFT_GREEN;
    if (packetBuffer[settings.getID()] == '2')
        ownColor = TFT_RED;

    int dX = 35;
    if (settings.getEnableMiniOverview())
    {
        miniOverview(canvas, packetBuffer, 35, 3, 34, DISPLAY_HEIGHT - 6);
        dX = 35+34+4;
    }
    else
    {
        canvas.drawRect(35, 3, 121, DISPLAY_HEIGHT - 6, TFT_DARKGREY);
        canvas.fillRect(36, 4, 119, DISPLAY_HEIGHT - 8, ownColor);
    }
    canvas.drawRect(dX, 3, DISPLAY_WIDTH-dX-4, DISPLAY_HEIGHT - 6, TFT_DARKGREY);
    canvas.fillRect(dX+1, 4, DISPLAY_WIDTH-dX-6, DISPLAY_HEIGHT - 8, ownColor);
    canvas.setTextSize(3);
    canvas.setTextColor(TFT_WHITE, TFT_BLACK);
    canvas.setCursor(10, (DISPLAY_HEIGHT-41)/2);
    if (settings.isIDWritten())
        canvas.print(settings.getID() + 1);
    else
    {
        if (millis() % 1000 < 500)
            canvas.print(settings.getID() + 1);
        else
            canvas.fillRect(0, 0, 34, DISPLAY_HEIGHT, TFT_BLACK);
    }
    return canvas;
}