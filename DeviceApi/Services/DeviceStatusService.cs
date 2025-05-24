using Microsoft.Data.SqlClient;

namespace DeviceApi.Services
{
    public class DeviceStatusService
    {
        private readonly string _connectionString;

        public DeviceStatusService(IConfiguration config)
        {
            _connectionString = config.GetConnectionString("DefaultConnection")!;
        }

        public async Task UpdateDeviceStatusAsync(int deviceId, bool status)
        {
            using var conn = new SqlConnection(_connectionString);
            await conn.OpenAsync();

            var cmd = new SqlCommand("UPDATE Devices SET Status = @status WHERE Id = @id", conn);
            cmd.Parameters.AddWithValue("@status", status ? 1 : 0);
            cmd.Parameters.AddWithValue("@id", deviceId);

            await cmd.ExecuteNonQueryAsync();
        }

        public async Task<bool?> GetDeviceStatusAsync(int deviceId)
        {
            using var conn = new SqlConnection(_connectionString);
            await conn.OpenAsync();

            var cmd = new SqlCommand("SELECT Status FROM Devices WHERE Id = @id", conn);
            cmd.Parameters.AddWithValue("@id", deviceId);

            var result = await cmd.ExecuteScalarAsync();
            return result == null ? null : Convert.ToBoolean(result);
        }
    }
}
