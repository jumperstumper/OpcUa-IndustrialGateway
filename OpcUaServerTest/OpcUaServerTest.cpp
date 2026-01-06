#include <open62541/server.h>
#include <open62541/server_config_default.h>
#include <iostream>
#include <signal.h> // Behövs för att stänga av snyggt

// Gör variabeln statisk så att den lever under hela programmets gång
static volatile UA_Boolean running = true;

void updateTemperature(UA_Server* server, void* data) {
    static double currentTemp = 25.0;
    currentTemp += (rand() % 100 - 50) / 10.0;

    UA_Variant value;
    UA_Variant_init(&value);
    UA_Variant_setScalar(&value, &currentTemp, &UA_TYPES[UA_TYPES_DOUBLE]);

    UA_NodeId nodeId = UA_NODEID_STRING(1, (char*)"boiler.temperature");
    UA_Server_writeValue(server, nodeId, value);

     std::cout << "Server uppdaterad: " << currentTemp << std::endl;
}

int main() {
    UA_Server* server = UA_Server_new();
    UA_ServerConfig_setDefault(UA_Server_getConfig(server));

    UA_VariableAttributes attr = UA_VariableAttributes_default;
    UA_Double initialValue = 25.0;
    UA_Variant_setScalar(&attr.value, &initialValue, &UA_TYPES[UA_TYPES_DOUBLE]);
    attr.displayName = UA_LOCALIZEDTEXT((char*)"en-US", (char*)"Boiler Temperature");

    UA_Server_addVariableNode(server,
        UA_NODEID_STRING(1, (char*)"boiler.temperature"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
        UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
        UA_QUALIFIEDNAME(1, (char*)"Boiler Temperature"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
        attr, NULL, NULL);

    // 3. Registrera timern så funktionen körs varje sekund
    UA_Server_addRepeatedCallback(server, (UA_ServerCallback)updateTemperature, NULL, 1000, NULL);

    std::cout << "Server startad på opc.tcp://localhost:4840" << std::endl;

    UA_StatusCode retval = UA_Server_run(server, &running);

    UA_Server_delete(server);
    return (int)retval;
}
