#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <cstdint>

class Controller {
public:
    Controller();
    void pollKeyboard();         // Poll keyboard for the state of all buttons
    uint8_t getButtonState();    // Return the current button states as an 8-bit value
    void setButtonState(uint8_t state); // Test helper method

private:
    uint8_t buttonState;         // Current state of all buttons
};

#endif // CONTROLLER_H