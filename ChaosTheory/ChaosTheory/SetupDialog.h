#pragma once

#include <stdio.h>
#include <windows.h>

typedef struct {
  int fullscreen;
  int music;
  DEVMODE mode;
  int vsync;
  int alwaysontop;
  float AspectRatio;

	int cancel;
} SETUPCFG;

extern SETUPCFG setupcfg;

int OpenSetupDialog(HINSTANCE hInstance);