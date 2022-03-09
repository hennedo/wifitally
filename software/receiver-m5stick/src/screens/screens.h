#pragma once
enum State
{
  StateBooting,
  StateOnboarding,
  StateConnecting,
  StateShowTally,
  StateShowMenu
};
extern State state;