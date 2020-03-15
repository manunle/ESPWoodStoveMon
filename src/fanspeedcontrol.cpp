#include <Arduino.h>
#include "fanspeedcontrol.h"

FanSpeedControler::FanSpeedControler(int Pin,unsigned int maxSpeed,unsigned int minSpeed)
{
    pin=Pin;
    maxspeed=maxSpeed;
    minspeed=minSpeed;
    currentspeed=minspeed;
    analogWriteFreq(100);
}

void FanSpeedControler::setSpeed(unsigned int Speed)
{
    newspeed = Speed;
    if(newspeed > minspeed)
        state = 1;
    else
        newspeed = minspeed;
}

void FanSpeedControler::setSpeedNow(unsigned int newSpeed)
{
    if(newSpeed < minspeed)
        currentspeed = newspeed = minspeed;
    else
        if(newSpeed > maxspeed)
            currentspeed = newspeed = maxspeed;
        else
            currentspeed = newspeed = newSpeed;
}

String FanSpeedControler::getDebugStr()
{
    String retstring;
    char chstr[80];
    sprintf(chstr,"speed=%d newspeed=%d changerate=%d state=%d",currentspeed,newspeed,changerate,state);
    return String(chstr);
}

unsigned int FanSpeedControler::getcurrentspeed()
{
    return currentspeed;
}

void FanSpeedControler::tick()
{
    if(state == 0)
    {
        analogWrite(pin,0);
    }
    else
    {
        if(newspeed != currentspeed)
        {
            if(newspeed < currentspeed)
            {
                if(currentspeed < minspeed + changerate)
                    currentspeed = minspeed;
                else
                    currentspeed -= changerate;
            }
            else
            {
                if(currentspeed > maxspeed - changerate)
                    currentspeed = maxspeed;
                else
                    currentspeed += changerate;
            }
        }
        if(currentspeed <= minspeed)
            state = 0;
        analogWrite(pin,currentspeed);
    }
}