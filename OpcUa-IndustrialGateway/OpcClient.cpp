#include "OpcClient.hpp"
#include <iostream>

OpcClient::OpcClient(std::string url) : endpointUrl(url) {
    client = UA_Client_new();
    UA_ClientConfig_setDefault(UA_Client_getConfig(client));
}

OpcClient::~OpcClient() {
    UA_Client_delete(client); // Viktigt! Annars får vi minnesläcka.
}

bool OpcClient::connect() {
    UA_StatusCode status = UA_Client_connect(client, endpointUrl.c_str());
    if (status != UA_STATUSCODE_GOOD) {
        std::cout << "Kunde inte ansluta till OPC-servern!" << std::endl;
        return false;
    }
    std::cout << "Ansluten till: " << endpointUrl << std::endl;
    return true;
}

double OpcClient::readTemperature() {
    // För tillfället simulerar vi ett värde
    return 22.5 + (rand() % 10);
}