using System;
using System.Net.Http;
using System.Text;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Azure.WebJobs;
using Microsoft.Azure.WebJobs.Extensions.Http;
using Microsoft.Extensions.Logging;


// https://docs.microsoft.com/en-us/rest/api/iotcentral/devices/getproperties


namespace Glovebox.Function
{
    public static class HvacControl
    {
        private static string authorization = Environment.GetEnvironmentVariable("IOT_CENTRAL_API_TOKEN");
        private static string deviceId = Environment.GetEnvironmentVariable("IOT_CENTRAL_DEVICE_ID");
        private static string iotCentralUrl = Environment.GetEnvironmentVariable("IOT_CENTRAL_URL");

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

            sb.Append("{\"LearningPath\":{\"DesiredTemperature\":");
            sb.Append(cmd);
            sb.Append("}}");

            using (var client = new HttpClient())
            {
                client.BaseAddress = new Uri(iotCentralUrl);
                client.DefaultRequestHeaders.Add("Authorization", authorization);

                var content = new StringContent(sb.ToString(), Encoding.UTF8, "application/json");

                var api = $"api/preview/devices/{deviceId}/properties";

                var response = await client.PutAsync(api, content);
                if (response.IsSuccessStatusCode)
                {
                    return new OkObjectResult("success");
                }
                else
                {
                    return new OkObjectResult("failed");
                }
            }
        }
    }
}
