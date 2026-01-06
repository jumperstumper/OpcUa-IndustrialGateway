#include <open62541/server.h>
#include <open62541/server_config_default.h>
#include <iostream>
#include <signal.h> // Behövs för att stänga av snyggt

// Gör variabeln statisk så att den lever under hela programmets gång
static volatile UA_Boolean running = true;

int main() {
    UA_Server* server = UA_Server_new();
    UA_ServerConfig_setDefault(UA_Server_getConfig(server));


    void updateTemperature(UA_Server * server, void* data); 
    {
        static double currentTemp = 25.0;
        currentTemp += (rand() % 100 - 50) / 10.0; // Slumpar temperaturen lite upp/ner

        UA_Variant value;
        UA_Variant_init(&value);
        UA_Variant_setScalar(&value, &currentTemp, &UA_TYPES[UA_TYPES_DOUBLE]);

        UA_NodeId nodeId = UA_NODEID_STRING(1, (char*)"boiler.temperature");
        UA_Server_writeValue(server, nodeId, value);
    }

    std::cout << "OPC UA Test Server startad på opc.tcp://localhost:4840" << std::endl;
    std::cout << "Tryck Ctrl+C för att avsluta." << std::endl;

    // Detta är det moderna sättet att köra servern
    UA_StatusCode retval = UA_Server_run(server, &running);

    // Om UA_Server_run mot förmodan inte hittas av din kompilator, 
    // kan du använda denna manuella loop istället:
    /*
    while(running) {
        UA_Server_run_iterate(server, true);
    }
    */

    UA_Server_delete(server);
    return (int)retval;
}
