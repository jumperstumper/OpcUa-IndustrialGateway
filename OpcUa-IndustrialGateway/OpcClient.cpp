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
    UA_Variant value;
    UA_Variant_init(&value);

    // Vi letar efter den NodeId vi skapade i servern: "boiler.temperature"
    UA_NodeId nodeId = UA_NODEID_STRING(1, (char*)"boiler.temperature");

    UA_StatusCode status = UA_Client_readValueAttribute(client, nodeId, &value);

    double tempValue = 0.0;
    if (status == UA_STATUSCODE_GOOD && UA_Variant_hasScalarType(&value, &UA_TYPES[UA_TYPES_DOUBLE])) {
        tempValue = *(UA_Double*)value.data;
    }

    UA_Variant_clear(&value);
    return tempValue;
}