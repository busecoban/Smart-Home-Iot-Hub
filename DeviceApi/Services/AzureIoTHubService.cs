using Microsoft.Azure.Devices;

namespace DeviceApi.Services
{
    public class AzureIoTHubService
    {
        private readonly ServiceClient _client;
        private readonly string _deviceId;

        public AzureIoTHubService(IConfiguration config)
        {
            var connStr = config["AzureIoTHub:ConnectionString"];
            _deviceId = config["AzureIoTHub:DeviceId"];
            _client = ServiceClient.CreateFromConnectionString(connStr);
        }

        public async Task<bool> SendCommandAsync(string command)
        {
            var message = new Message(System.Text.Encoding.UTF8.GetBytes(command))
            {
                Ack = DeliveryAcknowledgement.Full
            };

            try
            {
                await _client.SendAsync(_deviceId, message);
                return true;
            }
            catch
            {
                return false;
            }
        }
    }
}
