#pragma once

#ifdef M5STICKCPLUS
  // define display sizes for M5StickC Plus
  #define DISPLAY_WIDTH 240
  #define DISPLAY_HEIGHT 135
#endif
#ifndef M5STICKCPLUS
  // define display sizes for M5StickC
  #define DISPLAY_WIDTH 160
  #define DISPLAY_HEIGHT 80

#endif

enum State
{
  StateBooting,
  StateOnboarding,
  StateConnecting,
  StateShowTally,
  StateShowMenu
};
extern State state;