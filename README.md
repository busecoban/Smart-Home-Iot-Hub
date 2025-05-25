# Smart-Home-Iot-Hub


sass tokens

(base) busecoban@Buses-MacBook-Pro ~ % az iot hub generate-sas-token --hub-name senior-project-iothub --device-id Lamp1 --duration 31536000

{
  "sas": "SharedAccessSignature sr=senior-project-iothub.azure-devices.net%2Fdevices%2FLamp1&sig=5xIWfQb6hS4qfg%2Bws17eWSJD14EAUNXoUHm7DG15680%3D&se=1779641129"
}
(base) busecoban@Buses-MacBook-Pro ~ % az iot hub generate-sas-token --hub-name senior-project-iothub --device-id Cam1 --duration 31536000

{
  "sas": "SharedAccessSignature sr=senior-project-iothub.azure-devices.net%2Fdevices%2FCam1&sig=UmRx1%2BgMgpE1187BUWCvcGsONSMQiMN%2FmO3RkKIjT2g%3D&se=1779641174"
}
(base) busecoban@Buses-MacBook-Pro ~ % az iot hub generate-sas-token --hub-name senior-project-iothub --device-id Door1 --duration 31536000

{
  "sas": "SharedAccessSignature sr=senior-project-iothub.azure-devices.net%2Fdevices%2FDoor1&sig=FtrPni87A%2By0sqksMXg7e58xHCn8QL7DeA7vy6dFCGU%3D&se=1779641184"
}
