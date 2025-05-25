using Microsoft.AspNetCore.Mvc;
using DeviceApi.Models;
using DeviceApi.Services;

namespace DeviceApi.Controllers
{
    [ApiController]
    [Route("api/lamp")]
    public class LampController : ControllerBase
    {
        private readonly AzureIoTHubService _iotService;
        private readonly DeviceStatusService _dbService;
        private const int LampDeviceId = 4;

        public LampController(AzureIoTHubService iotService, DeviceStatusService dbService)
        {
            _iotService = iotService;
            _dbService = dbService;
        }
[HttpPost("command")]
public async Task<IActionResult> SendCommand([FromBody] CommandRequest request)
{
    var command = request.Command.ToUpper();
    var isOnOff = command == "ON" || command == "OFF";
    var isColor = System.Text.RegularExpressions.Regex.IsMatch(command, "^#[0-9A-F]{6}$");

    if (!isOnOff && !isColor)
        return BadRequest("Komut geçersiz. ON, OFF veya HEX renk girilmeli.");

    var sent = await _iotService.SendCommandAsync("Lamp1", command); // ✅ burada düzeltildi
    if (!sent)
        return StatusCode(500, "IoT Hub'a komut gönderilemedi.");

    if (isOnOff)
    {
        var status = command == "ON";
        await _dbService.UpdateDeviceStatusAsync(LampDeviceId, status);
    }

    return Ok(new { message = $"Komut gönderildi: {command}" });
}

        [HttpGet("status")]
        public async Task<IActionResult> GetStatus()
        {
            var status = await _dbService.GetDeviceStatusAsync(LampDeviceId);
            if (status == null)
                return NotFound(new { error = "Lamba cihazı bulunamadı." });

            return Ok(new { status });
        }
    }
}
