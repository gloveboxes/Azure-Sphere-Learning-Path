using System;
using System.Text;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Azure.Devices;
using Microsoft.Azure.WebJobs;
using Microsoft.Azure.WebJobs.Extensions.Http;
using Microsoft.Extensions.Logging;


namespace Glovebox.Function
{
    public static class RoverControl
    {
        private static string connectionString = Environment.GetEnvironmentVariable("IOT_HUB_SERVICE_CONNECTION_STRING");
        private static string deviceId = Environment.GetEnvironmentVariable("AZURE_SPHERE_IOT_HUB_DEVICE_ID");
        private static TransportType transportType = TransportType.Amqp;
        private static ServiceClient serviceClient = ServiceClient.CreateFromConnectionString(connectionString, transportType);

        [FunctionName("control")]
        public static async Task<IActionResult> Run(
            [HttpTrigger(AuthorizationLevel.Anonymous, "get", "post", Route = null)] HttpRequest req,
            ILogger log)
        {
            StringBuilder sb = new StringBuilder();

            string cmd = req.Query["cmd"];

            if (string.IsNullOrEmpty(cmd))
            {
                return new BadRequestObjectResult("failed, missing cmd");
            }

            sb.Append("{\"light_state\":");
            sb.Append(cmd);
            sb.Append("}");

            var dm = new CloudToDeviceMethod(deviceId)
            {
                MethodName = "LightControl",
            };

            dm.SetPayloadJson(sb.ToString());

            await serviceClient.InvokeDeviceMethodAsync(deviceId, dm).ConfigureAwait(false);

            return new OkObjectResult("success");
        }
    }
}
