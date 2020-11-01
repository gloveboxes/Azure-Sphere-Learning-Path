using System;
using System.IO;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Azure.WebJobs;
using Microsoft.Azure.WebJobs.Extensions.Http;
using Microsoft.AspNetCore.Http;
using Microsoft.Extensions.Logging;
using Newtonsoft.Json;
using System.Net.Http;

namespace Glovebox.Function
{
    public static class GetState
    {
        private static string authorization = Environment.GetEnvironmentVariable("IOT_CENTRAL_API_TOKEN");
        private static string deviceId = Environment.GetEnvironmentVariable("IOT_CENTRAL_DEVICE_ID");
        private static string iotCentralUrl = Environment.GetEnvironmentVariable("IOT_CENTRAL_URL");

        static string cachedResponse = string.Empty;
        static DateTime lastQueryTime = DateTime.Now;


        [FunctionName("GetState")]
        public static async Task<IActionResult> Run(
            [HttpTrigger(AuthorizationLevel.Anonymous, "get", "post", Route = null)] HttpRequest req,
            ILogger log)
        {
            log.LogInformation("C# HTTP trigger function processed a request.");
            string response;

            var cacheTimeDiff = DateTime.Now.Subtract(lastQueryTime);

            if (cacheTimeDiff.TotalSeconds < 15 && !string.IsNullOrEmpty(cachedResponse))
            {
                return new OkObjectResult(cachedResponse);
            }


            using (var client = new HttpClient())
            {
                client.BaseAddress = new Uri(iotCentralUrl);
                client.DefaultRequestHeaders.Add("Authorization", authorization);

                var api = $"api/preview/devices/{deviceId}/properties";

                response = await client.GetStringAsync(api);
            }

            lastQueryTime = DateTime.Now;
            cachedResponse = response;

            return new OkObjectResult(response);
        }
    }
}
