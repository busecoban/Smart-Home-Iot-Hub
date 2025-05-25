using Microsoft.Azure.Devices;
using Microsoft.Extensions.Configuration;

namespace DeviceApi.Services
{
    public class AzureIoTHubService
    {
        private readonly IConfiguration _config;

        public AzureIoTHubService(IConfiguration config)
        {
            _config = config;
        }

        public async Task<bool> SendCommandAsync(string deviceId, string command)
        {
            var connStr = _config[$"AzureIoTHub:Devices:{deviceId}"];
            if (string.IsNullOrEmpty(connStr))
                throw new ArgumentException($"Device '{deviceId}' için bağlantı bulunamadı");

            var client = ServiceClient.CreateFromConnectionString(connStr);

            var message = new Message(System.Text.Encoding.UTF8.GetBytes(command))
            {
                Ack = DeliveryAcknowledgement.Full
            };

            try
            {
                await client.SendAsync(deviceId, message);
                return true;
            }
            catch
            {
                return false;
            }
        }
    }
}
