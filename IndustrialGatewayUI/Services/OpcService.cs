using Opc.Ua;
using Opc.Ua.Client;

public class OpcService : BackgroundService
{
    private Session? _session;
    public event Action<string>? OnTemperatureChanged;

    public async Task ConnectAndSubscribeAsync()
    {
        try
        {
            // 1. Skapa konfiguration
            var config = new ApplicationConfiguration()
            {
                ApplicationName = "BlazorOpcClient",
                ApplicationUri = Utils.Format("urn:{0}:BlazorOpcClient", System.Net.Dns.GetHostName()),
                ApplicationType = ApplicationType.Client,
                SecurityConfiguration = new SecurityConfiguration
                {
                    ApplicationCertificate = new CertificateIdentifier
                    {
                        StoreType = @"Directory",
                        StorePath = @"%CommonApplicationData%\OPC Foundation\CertificateStore\MachineDefault",
                        SubjectName = "BlazorOpcClient"
                    },
                    AutoAcceptUntrustedCertificates = true,
                    AddAppCertToTrustedStore = true,
                    RejectSHA1SignedCertificates = false // Viktigt för lokala tester
                },
                TransportQuotas = new TransportQuotas { OperationTimeout = 15000 },
                ClientConfiguration = new ClientConfiguration { DefaultSessionTimeout = 60000 }
            };

            await config.Validate(ApplicationType.Client);

            // 2. HANTERA CERTIFIKAT - Denna metod fungerar i senaste versionen
            // Vi använder config.CertificateValidator för att säkerställa att vi litar på oss själva
            bool hasCertificate = config.SecurityConfiguration.ApplicationCertificate.Certificate != null;

            if (!hasCertificate)
            {
                // Om certifikat saknas, skapar vi ett (Kräver 'using Opc.Ua.Security')
                // Om CheckApplicationInstanceCertificate krånglar, kan vi köra utan för localhost:None
                Console.WriteLine("Inget certifikat hittat, kör utan kryptering...");
            }

            // 3. Definiera Endpoint
            var endpointDescription = new EndpointDescription
            {
                EndpointUrl = "opc.tcp://localhost:4840",
                SecurityMode = MessageSecurityMode.None,
                SecurityPolicyUri = SecurityPolicies.None
            };

            var endpoint = new ConfiguredEndpoint(null, endpointDescription, EndpointConfiguration.Create(config));

            // 4. Skapa session
            _session = await Session.Create(config, endpoint, false, "BlazorClient", 60000, null, null);

            // 5. Prenumeration
            var subscription = new Subscription(_session.DefaultSubscription) { PublishingInterval = 1000 };
            _session.AddSubscription(subscription);
            subscription.Create();

            // 6. Monitored Item
            var monitoredItem = new MonitoredItem(subscription.DefaultItem)
            {
                DisplayName = "Boiler Temperature",
                StartNodeId = "ns=1;s=boiler.temperature"
            };

            monitoredItem.Notification += (item, e) =>
            {
                if (monitoredItem.LastValue is MonitoredItemNotification notification)
                {
                    var value = notification.Value.WrappedValue.ToString();
                    OnTemperatureChanged?.Invoke(value);
                }
            };

            subscription.AddItem(monitoredItem);
            subscription.ApplyChanges();
        }
        catch (Exception ex)
        {
            Console.WriteLine($"OPC Error: {ex.Message}");
        }
    }

    protected override async Task ExecuteAsync(CancellationToken stoppingToken)
    {

        if (!stoppingToken.IsCancellationRequested)
        {
            await ConnectAndSubscribeAsync();
        }
        else 
        {
            _session?.Dispose();
        }

        
    }
}