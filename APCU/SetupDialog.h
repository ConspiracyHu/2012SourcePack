#pragma once

#include <stdio.h>
#include <windows.h>

typedef struct {
  int fullscreen;
  int music;
  int resolution;
  int texturedetail;
  int alwaysontop;

	int cancel;
} SETUPCFG;

extern SETUPCFG setupcfg;

int OpenSetupDialog(HINSTANCE hInstance);