#include <iostream>
#include <thread>
#include <chrono>
#include "OpcClient.hpp"

int main() {
    OpcClient myGateway("opc.tcp://localhost:4840");

    if (myGateway.connect()) {
        while (true) {
            double temp = myGateway.readTemperature();
            std::cout << "Aktuell temperatur: " << temp << " C" << std::endl;

            // Vänta 1 sekund innan nästa läsning
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }

    return 0;
}