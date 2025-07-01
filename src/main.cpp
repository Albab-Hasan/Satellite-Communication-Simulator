#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include <limits>
#include <memory>
#include <map>
#include <random>
#include <chrono>

#include "../include/coding_strategy.h"
#include "../include/coding_factory.h"
#include "../include/utils.h"
#include "../include/channel.h"
#include "../include/visualizer.h"
#include "../include/metrics.h"

// Function to display a welcome message and instructions
void displayWelcome() {
    std::cout << "===============================================================\n";
    std::cout << "           SATELLITE COMMUNICATION SIMULATOR\n";
    std::cout << "===============================================================\n\n";
    std::cout << "This program simulates a satellite communication system with:\n";
    std::cout << "  1. Data encoding with various error correction codes\n";
    std::cout << "  2. Noisy channel simulation\n";
    std::cout << "  3. Data decoding with error detection and correction\n";
    std::cout << "  4. Performance metrics and visualization\n\n";
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

// Function to select a coding strategy
std::string selectCodingStrategy() {
    CodingFactory& factory = CodingFactory::getInstance();
    std::vector<std::string> strategyNames = factory.getStrategyNames();

    std::cout << "Available error correction codes:\n";
    for (size_t i = 0; i < strategyNames.size(); i++) {
        std::cout << "  " << (i + 1) << ". " << strategyNames[i] << "\n";
        std::cout << "     " << factory.getStrategyDescription(strategyNames[i]) << "\n";
    }

    int selection = 0;
    bool validInput = false;

    do {
        std::cout << "Select error correction code (1-" << strategyNames.size() << "): ";
        std::string input;
        std::getline(std::cin, input);

        std::stringstream ss(input);
        if (ss >> selection && selection >= 1 && selection <= static_cast<int>(strategyNames.size())) {
            validInput = true;
        } else {
            std::cout << "Invalid input. Please enter a number between 1 and " << strategyNames.size() << ".\n";
        }
    } while (!validInput);

    return strategyNames[selection - 1];
}

// Function to run the simulation
void runSimulation() {
    // Create components
    Visualizer visualizer;
    Metrics metrics;

    // Get user message
    std::cout << "Enter message to transmit: ";
    std::string message;
    std::getline(std::cin, message);

    if (message.empty()) {
        std::cout << "Message cannot be empty.\n";
        return;
    }

    // Select coding strategy
    std::string strategyName = selectCodingStrategy();
    std::shared_ptr<CodingStrategy> codingStrategy = CodingFactory::getInstance().createStrategy(strategyName);

    // Get error rate
    double errorRate = getErrorRate();

    // Create channel
    Channel channel(errorRate);

    // Step 1: Convert message to bits
    metrics.startOperation("String to Bits Conversion");
    std::vector<bool> originalBits = Utils::stringToBits(message);
    metrics.endOperation("String to Bits Conversion");

    std::cout << "\n=== SIMULATION RESULTS ===\n\n";

    // Display original bits
    visualizer.visualizeBits(originalBits, "Original bits", std::cout, 8);

    // Step 2: Encode the message
    metrics.startOperation("Encoding");
    std::vector<bool> encodedBits = codingStrategy->encode(originalBits);
    metrics.endOperation("Encoding");
    metrics.recordTransmission(originalBits.size(), encodedBits.size());

    visualizer.visualizeBits(encodedBits, "Encoded bits (" + strategyName + ")", std::cout,
                           strategyName.find("Hamming") != std::string::npos ? 7 : 8);

    // Step 3: Transmit through noisy channel
    metrics.startOperation("Channel Transmission");
    std::vector<bool> transmittedBits = channel.transmit(encodedBits);
    metrics.endOperation("Channel Transmission");

    // Count errors introduced by channel
    int channelErrors = Utils::countDifferences(encodedBits, transmittedBits);
    metrics.recordErrors(encodedBits, transmittedBits);

    visualizer.visualizeDifferences(encodedBits, transmittedBits, "Transmission with noise", std::cout,
                                  strategyName.find("Hamming") != std::string::npos ? 7 : 8);

    std::cout << "Bit errors introduced: " << channelErrors << " of "
              << encodedBits.size() << " bits ("
              << std::fixed << std::setprecision(2)
              << (static_cast<double>(channelErrors) / encodedBits.size() * 100)
              << "%)\n\n";

    // Step 4: Decode the received message
    metrics.startOperation("Decoding");
    auto [decodedBits, errorsFixed] = codingStrategy->decode(transmittedBits);
    metrics.endOperation("Decoding");
    metrics.recordErrorCorrection(channelErrors, errorsFixed);

    std::cout << "Errors fixed by " << strategyName << ": " << errorsFixed << std::endl << std::endl;

    // Step 5: Convert bits back to string
    metrics.startOperation("Bits to String Conversion");
    std::string receivedMessage = Utils::bitsToString(decodedBits);
    metrics.endOperation("Bits to String Conversion");

    // Compare original and received message
    bool successful = (receivedMessage == message);

    std::cout << "Received message: \"" << receivedMessage << "\"\n";
    std::cout << "Transmission " << (successful ? "SUCCESSFUL" : "FAILED") << std::endl;

    // Show uncorrected errors if any
    if (!successful) {
        std::cout << "Uncorrected errors detected in the message.\n";
    }

    // Display performance metrics
    std::cout << std::endl;
    visualizer.drawSeparator();
    metrics.printSummary();
    visualizer.drawSeparator();
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
