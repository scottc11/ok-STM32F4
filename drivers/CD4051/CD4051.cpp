#include "CD4051.h"

/**
 * @brief activate/deactivate mux
 *
 * @param state true will deactivate mux
 */
void CD4051::inhibit(bool state)
{
    inhibitPin.write(state);
}

/**
 * @brief activate a channel between 0 and 7
 * 
 * @param channel number 0 to 7
 */
void CD4051::activateChannel(int channel) {

    if (channel > 7) {
        return;
    }
    activeChannel = channel;
    switch (channel) {
        case 0:
            pinA.write(0);
            pinB.write(0);
            pinC.write(0);
            break;
        case 1:
            pinA.write(1);
            pinB.write(0);
            pinC.write(0);
            break;
        case 2:
            pinA.write(0);
            pinB.write(1);
            pinC.write(0);
            break;
        case 3:
            pinA.write(1);
            pinB.write(1);
            pinC.write(0);
            break;
        case 4:
            pinA.write(0);
            pinB.write(0);
            pinC.write(1);
            break;
        case 5:
            pinA.write(1);
            pinB.write(0);
            pinC.write(1);
            break;
        case 6:
            pinA.write(0);
            pinB.write(1);
            pinC.write(1);
            break;
        case 7:
            pinA.write(1);
            pinB.write(1);
            pinC.write(1);
            break;
        default:
            break;
    }
}