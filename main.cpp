#include "mbed.h"
#define RESET 0
#define SET 1
#define READY 1
#define SL_DIS 2.0
#define LL_DIS 0.8
#define HAND_WIDTH 0.25

typedef struct
{
    uint8_t Frame;
    uint8_t RepetitionN;
    uint8_t RotationN;
    float PieceN;
} strPattern_t;

typedef struct
{
    float Hour;
    float Minute;
    float Second;
} strClock_t;

/*BusOut ControlLED(PTE4, PTE5, PTE20, PTE21, PTE22, PTE23, PTE29);
InterruptIn Sensor(PTD7);*/
/*DigitalIn Sensor(PTD3);
BusOut LED_ON(PTA17,PTA16,PTC17,PTC16,PTC13,PTC12,PTC11,PTC10,PTC6,PTC5,PTC4,PTC3,PTC0,PTC7);*/
BusOut ControlLED(PTC7, PTC0, PTC3, PTC4, PTC5, PTC6, PTC10, PTC11, PTC12, PTC13, PTC16, PTC17, PTA16, PTA17);
InterruptIn Sensor(PTD3);

Timer Watch;
Serial pc(USBTX, USBRX);

void SensorISR(void);
float CalPos(float pieceN);
void PlayClock(void);

float CurrentPosT = 0.0;
float RotationT = 0.0;
float Position = 0.0;
uint8_t RepetitionN = 0;
uint8_t RotationN = 0;
uint8_t State = RESET;
uint16_t TopLED;
strPattern_t strPattern1, strPattern2;
strClock_t strClock;

int main(void)
{
    Watch.start();
    Sensor.rise(&SensorISR);
    while(State != READY)
    {
        wait(0.1);
    }
    PlayClock();
}

void SensorISR(void)
{
    RotationT = Watch.read_us();
    Watch.reset();
    RotationN++;
    State = READY;
}
float CalPos(float pieceN)
{
    CurrentPosT = Watch.read_us();
    return (CurrentPosT/RotationT * pieceN);
}

void PlayClock(void)
{
    int i;
    //float theta;
    float theta[9] = {7.162, 5.166, 3.979, 3.255, 2.755, 2.387, 2.106, 1.885, 1.705};
    strPattern2.Frame = 0;
    strPattern2.RepetitionN = 0;
    strPattern2.RotationN = 20;
    strPattern2.PieceN = 360.0;
    strClock.Second = 90.0;
    strClock.Hour = 312.0;
    strClock.Minute = 192.0;
    while(1)
    {
        Position = CalPos(strPattern2.PieceN);
        TopLED = 0;
        if (RotationN > strPattern2.RotationN)
        {
            RotationN = RESET;
            strClock.Second = strClock.Second + 6.0;
            if (strClock.Second > 360.0)
            {
                strClock.Second = strClock.Second - 360.0;
                strClock.Minute = strClock.Minute + 6.0;
                strClock.Hour = strClock.Hour + 0.5;
            }
            if (strClock.Minute > 360.0)
            {
                strClock.Minute = 0.0;
            }
            if (strClock.Hour > 360.0)
            {
                strClock.Hour = 0.0;
            }
        }
        
        if (Position < 3.0)
        {
            TopLED = 0x000E;
        }
        else if (Position > 87.0 && Position < 93.0)
        {
            TopLED = 0x000E;
        }
        else if (Position > 177.0 && Position < 183.0)
        {
            TopLED = 0x000E;
        }
        else if (Position > 267.0 && Position < 273.0)
        {
            TopLED = 0x000E;
        }
        else
        {
            TopLED = 0;
        }
        
        if (Position < strClock.Second)
        {
            TopLED = (TopLED | 0x0001);
        }
        
        
        /*
        for (i = 0; i < 9; ++i){
            theta = atan(HAND_WIDTH/(SL_DIS + LL_DIS * (float) i)) * 180.0 / 3.141592;
            if (Position > strClock.Minute - theta && Position < strClock.Minute + theta){
                TopLED = (TopLED | (0x2000 >> i));
                }
            if (i < 4){
                if (Position > strClock.Hour - theta && Position < strClock.Hour + theta)
                    TopLED = (TopLED | (0x2000 >> i));
                    }
        }*/
        
        for (i = 0; i < 9; ++i){
            if (Position > strClock.Minute - theta[i] && Position < strClock.Minute + theta[i]){
                TopLED = (TopLED | (0x2000 >> i));
                }
            if (i < 4){
                if (Position > strClock.Hour - theta[i] && Position < strClock.Hour + theta[i]){
                    TopLED = (TopLED | (0x2000 >> i));
                    }
                }
            }
        
        /*if (Position > strClock.Minute - 3 && Position < strClock.Minute + 3)
        {
            TopLED = (TopLED | 0x3FC0);
        }
        if (Position > strClock.Hour - 3 && Position < strClock.Hour + 3)
        {
            TopLED = (TopLED | 0x3E00);
        }*/
        ControlLED = TopLED;
        
    }
}