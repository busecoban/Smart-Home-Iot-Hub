using Microsoft.AspNetCore.Mvc;
using DeviceApi.Models;
using DeviceApi.Services;

namespace DeviceApi.Controllers
{
    [ApiController]
    [Route("api/door")]
    public class DoorController : ControllerBase
    {
        private readonly AzureIoTHubService _iotService;
        private readonly DeviceStatusService _dbService;
        private const int DoorDeviceId = 6;

        public DoorController(AzureIoTHubService iotService, DeviceStatusService dbService)
        {
            _iotService = iotService;
            _dbService = dbService;
        }

[HttpPost("command")]
public async Task<IActionResult> SendCommand([FromBody] CommandRequest request)
{
    var command = request.Command.ToLower();
    if (command != "on" && command != "off")
        return BadRequest("Komut geçersiz. Sadece 'on' veya 'off' desteklenir.");

    var sent = await _iotService.SendCommandAsync("Door1", command); // ✅ burada düzeltildi
    if (!sent)
        return StatusCode(500, "IoT Hub'a komut gönderilemedi.");

    bool newStatus = command == "on";
    await _dbService.UpdateDeviceStatusAsync(DoorDeviceId, newStatus);

    return Ok(new { message = $"Komut gönderildi: {command}" });
}


        [HttpGet("status")]
        public async Task<IActionResult> GetStatus()
        {
            var status = await _dbService.GetDeviceStatusAsync(DoorDeviceId);
            if (status == null)
                return NotFound(new { error = "Kapı cihazı bulunamadı." });

            return Ok(new { status });
        }
    }
}
