# Satellite Communication Simulator

A C++ program that simulates the core processes of a satellite communication system: encoding data, transmitting it over a noisy channel, and decoding it on the receiver side. This simulator demonstrates how information is protected, transmitted, and recovered despite errors caused by noise.

## Features

- **Data Encoding**
  - Converts text input into a bitstream
  - Implements Hamming(7,4) error-correcting code

- **Transmission Channel Simulation**
  - Simulates a noisy communication channel by randomly flipping bits
  - Configurable bit error rate

- **Data Decoding**
  - Receives the noisy bitstream
  - Uses Hamming code to detect and correct errors
  - Recovers the original data or reports failure

- **User Interface**
  - Command-line interface for user interaction
  - Detailed display of encoding, transmission, and decoding steps

## How It Works

1. **Encoding**: The input message is converted to bits, then encoded using Hamming(7,4) code, which adds parity bits to detect and correct errors.

2. **Transmission**: The encoded bitstream is passed through a simulated noisy channel where bits may be flipped based on the specified error rate.

3. **Decoding**: The received bitstream is decoded using the Hamming code to detect and correct any single-bit errors within each 7-bit code block.

## Building and Running

### Prerequisites

- C++17 compatible compiler (g++ or clang++)
- Make

### Build Instructions

1. Clone the repository
2. Run `make` in the project directory

```bash
git clone https://github.com/Albab-Hasan/Satellite-Communication-Simulator.git
cd Satellite-Communication-Simulator
make
```

### Running the Simulator

```bash
./sat-com
```

Or use the make run command:

```bash
make run
```

## Usage Example

```
===============================================================
           SATELLITE COMMUNICATION SIMULATOR
===============================================================

This program simulates a satellite communication system with:
  1. Data encoding with Hamming(7,4) error correction
  2. Noisy channel simulation
  3. Data decoding with error correction

Enter message to transmit: Hello, World!
Enter bit error rate (0.0 to 1.0): 0.05

=== SIMULATION RESULTS ===

Original bits: 01001000 01100101 01101100 01101100 01101111 00101100 00100000 01010111 01101111 01110010 01101100 01100100 00100001
Encoded bits (with Hamming code): 1101000 0011101 1110110 1001001 1110110 0011100 1101110 1110111 0101110 0011000 0001010 0101111 1011111 0111111 1110010 0111001 1110110 1001100 1101100 0111000 0100001
Transmitted bits (after noise): 1101000 0011101 1110110 1001001 1110110 0011000 1101110 1110111 0101110 0011000 0001010 0101111 1011111 0111111 1110010 0111001 1110110 1001100 1101100 0111000 0100001
Bit errors introduced: 1 of 147 bits (0.68%)
Errors fixed by Hamming code: 1
Received message: "Hello, World!"
Transmission SUCCESSFUL

Run another simulation? (y/n):
```

## Technical Details

- The implementation uses Hamming(7,4) code which encodes 4 data bits into 7 bits (4 data + 3 parity)
- Can detect up to 2-bit errors and correct 1-bit errors per 7-bit block
- The noisy channel simulator uses random number generation to flip bits according to the specified error rate
