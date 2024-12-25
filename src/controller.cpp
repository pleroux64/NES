#include "controller.h"
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h> // Mac-specific keyboard polling
#endif

Controller::Controller() : buttonState(0) {}

// Map keyboard keys to NES buttons (simplified)
void Controller::pollKeyboard()
{
    buttonState = 0;

#ifdef __APPLE__
    // Update buttonState directly based on key states
    if (CGEventSourceKeyState(kCGEventSourceStateCombinedSessionState, (CGKeyCode)12)) // Key: A
        buttonState |= (1 << 0);                                                       // NES A button
    if (CGEventSourceKeyState(kCGEventSourceStateCombinedSessionState, (CGKeyCode)38)) // Key: S
        buttonState |= (1 << 1);                                                       // NES B button
    if (CGEventSourceKeyState(kCGEventSourceStateCombinedSessionState, (CGKeyCode)56)) // Key: Shift
        buttonState |= (1 << 2);                                                       // NES Select
    if (CGEventSourceKeyState(kCGEventSourceStateCombinedSessionState, (CGKeyCode)36)) // Key: Enter
        buttonState |= (1 << 3);                                                       // NES Start
    if (CGEventSourceKeyState(kCGEventSourceStateCombinedSessionState, (CGKeyCode)13)) // Key: W
        buttonState |= (1 << 4);                                                       // NES Up
    if (CGEventSourceKeyState(kCGEventSourceStateCombinedSessionState, (CGKeyCode)1))  // Key: S
        buttonState |= (1 << 5);                                                       // NES Down
    if (CGEventSourceKeyState(kCGEventSourceStateCombinedSessionState, (CGKeyCode)0))  // Key: A
        buttonState |= (1 << 6);                                                       // NES Left
    if (CGEventSourceKeyState(kCGEventSourceStateCombinedSessionState, (CGKeyCode)2))  // Key: D
        buttonState |= (1 << 7);                                                       // NES Right
#endif
}

// Return the current button state
uint8_t Controller::getButtonState()
{
    return buttonState;
}

void Controller::setButtonState(uint8_t state)
{
    buttonState = state; // Directly set the button state
}
