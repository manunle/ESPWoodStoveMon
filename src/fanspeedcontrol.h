#ifndef FANSPEEDCONTROL
#define FANSPEEDCONTROL

class FanSpeedControler
{
    private:
        int pin;
        unsigned int maxspeed = 1024;
        unsigned int minspeed = 0;
        unsigned int currentspeed = 0;
        unsigned int newspeed = 0;
        unsigned int changerate = 1;
        int state = 0;

    public:
        FanSpeedControler(int pin,unsigned int maxspeed,unsigned int minspeed);
        void setSpeed(unsigned int speed);
        void setSpeedNow(unsigned int newspeed);
        String getDebugStr();
        unsigned int getcurrentspeed();
        void tick();
};

#endif