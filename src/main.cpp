#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include <limits>

#include "../include/encoder.h"
#include "../include/channel.h"
#include "../include/decoder.h"
#include "../include/utils.h"

// Function to display a welcome message and instructions
void displayWelcome() {
    std::cout << "===============================================================\n";
    std::cout << "           SATELLITE COMMUNICATION SIMULATOR\n";
    std::cout << "===============================================================\n\n";
    std::cout << "This program simulates a satellite communication system with:\n";
    std::cout << "  1. Data encoding with Hamming(7,4) error correction\n";
    std::cout << "  2. Noisy channel simulation\n";
    std::cout << "  3. Data decoding with error correction\n\n";
}

// Function to get a valid double input for error rate
double getErrorRate() {
    double errorRate;
    bool validInput = false;

    do {
        std::cout << "Enter bit error rate (0.0 to 1.0): ";
        std::string input;
        std::getline(std::cin, input);

        std::stringstream ss(input);
        if (ss >> errorRate && errorRate >= 0.0 && errorRate <= 1.0) {
            validInput = true;
        } else {
            std::cout << "Invalid input. Please enter a number between 0.0 and 1.0.\n";
        }
    } while (!validInput);

    return errorRate;
}

// Function to run the simulation
void runSimulation() {
    // Get user message
    std::cout << "Enter message to transmit: ";
    std::string message;
    std::getline(std::cin, message);

    if (message.empty()) {
        std::cout << "Message cannot be empty.\n";
        return;
    }

    // Get error rate
    double errorRate = getErrorRate();

    // Create components
    Encoder encoder;
    Channel channel(errorRate);
    Decoder decoder;

    // Step 1: Convert message to bits
    std::vector<bool> originalBits = Utils::stringToBits(message);

    std::cout << "\n=== SIMULATION RESULTS ===\n\n";

    // Display original bits
    std::cout << "Original bits: ";
    Utils::printBits(originalBits);
    std::cout << std::endl;

    // Step 2: Encode the message
    std::vector<bool> encodedBits = encoder.encode(message);
    std::cout << "Encoded bits (with Hamming code): ";
    Utils::printBits(encodedBits, std::cout, 7);  // Group by 7 for Hamming(7,4) blocks
    std::cout << std::endl;

    // Step 3: Transmit through noisy channel
    std::vector<bool> transmittedBits = channel.transmit(encodedBits);

    // Count errors introduced by channel
    int channelErrors = Utils::countDifferences(encodedBits, transmittedBits);

    std::cout << "Transmitted bits (after noise): ";
    Utils::printBits(transmittedBits, std::cout, 7);
    std::cout << std::endl;

    std::cout << "Bit errors introduced: " << channelErrors << " of "
              << encodedBits.size() << " bits ("
              << std::fixed << std::setprecision(2)
              << (static_cast<double>(channelErrors) / encodedBits.size() * 100)
              << "%)\n";

    // Step 4: Decode the received message
    auto [decodedBits, errorsFixed] = decoder.decode(transmittedBits);

    std::cout << "Errors fixed by Hamming code: " << errorsFixed << std::endl;

    // Step 5: Convert bits back to string
    std::string receivedMessage = Utils::bitsToString(decodedBits);

    // Compare original and received message
    bool successful = (receivedMessage == message);

    std::cout << "Received message: \"" << receivedMessage << "\"\n";
    std::cout << "Transmission " << (successful ? "SUCCESSFUL" : "FAILED") << std::endl;

    // Show uncorrected errors if any
    if (!successful) {
        std::cout << "Uncorrected errors detected in the message.\n";
    }
}

// Main function
int main() {
    displayWelcome();

    bool running = true;

    while (running) {
        runSimulation();

        // Ask if the user wants to run another simulation
        std::cout << "\nRun another simulation? (y/n): ";
        std::string response;
        std::getline(std::cin, response);

        running = (response == "y" || response == "Y");

        if (running) {
            std::cout << "\n";
        }
    }

    std::cout << "\nThank you for using the Satellite Communication Simulator!\n";

    return 0;
}
