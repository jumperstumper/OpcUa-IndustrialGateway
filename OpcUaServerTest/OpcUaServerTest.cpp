#include <open62541/server.h>
#include <open62541/server_config_default.h>
#include <iostream>

int main()
{
    std::cout << "Starting OPC UA Server..." << std::endl;

    UA_Server* server = UA_Server_new();
    UA_ServerConfig_setDefault(UA_Server_getConfig(server));

    UA_Boolean running = true;
    UA_Server_run(server, &running);

    UA_Server_delete(server);
    return 0;
}
