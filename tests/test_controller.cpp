#include "controller.h"
#include "doctest.h" // Testing framework
#include <bitset>
#include <iostream>

// Mock method for simulating button presses
void simulateKeyPress(Controller& controller, uint8_t buttonState) {
    // Overwrite private state using a helper function (optional for testing)
    // Assume you modify Controller to expose a "setButtonState" method.
    controller.setButtonState(buttonState);

    // Debugging: Log simulated button state
    std::cout << "[DEBUG] Simulated Button State: " << std::bitset<8>(buttonState) << std::endl;
}

TEST_CASE("Controller - Button State Updates") {
    Controller controller;

    SUBCASE("All Buttons Released") {
        controller.pollKeyboard();
        uint8_t state = controller.getButtonState();
        std::cout << "[DEBUG] Button State (All Released): " << std::bitset<8>(state) << std::endl;
        CHECK(state == 0x00);
    }

    SUBCASE("A Button Pressed") {
        simulateKeyPress(controller, 0b00000001);
        uint8_t state = controller.getButtonState();
        std::cout << "[DEBUG] Button State (A Pressed): " << std::bitset<8>(state) << std::endl;
        CHECK(state == 0b00000001);
    }

    SUBCASE("Multiple Buttons Pressed") {
        simulateKeyPress(controller, 0b00001111); // A, B, Select, Start
        uint8_t state = controller.getButtonState();
        std::cout << "[DEBUG] Button State (Multiple Pressed): " << std::bitset<8>(state) << std::endl;
        CHECK(state == 0b00001111);
    }

    SUBCASE("All Buttons Pressed") {
        simulateKeyPress(controller, 0xFF);
        uint8_t state = controller.getButtonState();
        std::cout << "[DEBUG] Button State (All Pressed): " << std::bitset<8>(state) << std::endl;
        CHECK(state == 0xFF);
    }
}

TEST_CASE("Controller - Bitset Debugging") {
    Controller controller;
    simulateKeyPress(controller, 0b10101010); // Example state

    std::bitset<8> state(controller.getButtonState());
    std::cout << "[DEBUG] Bitset Representation: " << state << std::endl;

    CHECK(state[0] == false); // A
    CHECK(state[1] == true);  // B
    CHECK(state[2] == false); // Select
    CHECK(state[3] == true);  // Start
    CHECK(state[4] == false); // Up
    CHECK(state[5] == true);  // Down
    CHECK(state[6] == false); // Left
    CHECK(state[7] == true);  // Right
}
