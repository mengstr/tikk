#pragma once

#define LOW 0
#define HIGH 1
#define FLOAT 2
#define PU 3
#define PD 4

void SetPortA(int bitno, int mode);
void SetPortC(int bitno, int mode);
void SetPortD(int bitno, int mode);
