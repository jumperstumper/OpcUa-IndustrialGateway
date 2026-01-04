#pragma once // Hindrar filen från att läsas dubbelt
#include <open62541/client_config_default.h>
#include <open62541/client_highlevel.h>
#include <string>

class OpcClient {
public:
    OpcClient(std::string url);
    ~OpcClient(); // Destruktor - här städar vi minnet!

    bool connect();
    void disconnect();
    double readTemperature(); // Vår test-metod

private:
    UA_Client* client; // En "Pointer" - välkommen till C++!
    std::string endpointUrl;
};