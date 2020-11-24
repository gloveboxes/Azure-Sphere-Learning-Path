import requests
import json
import time
import os


# Generate DEVICE_TWIN_AUTHORIZATION with
# az iot hub generate-sas-token --hub-name "hub name" --duration 864000 (eg 864000 = ten days)

device_id = os.environ["DEVICE_TWIN_DEVICE_ID"]
authorization = os.environ["DEVICE_TWIN_AUTHORIZATION"]
hub_name = os.environ["DEVICE_TWIN_HUB_NAME"]

count = 0
DesiredSampleRateInSeconds_failed_count = 0
desired_sampleRateInSeconds_success_count = 0
desired_temperature_failed_count = 0
desired_temperature_success_count = 0

sample_rate = [1, 5, 25, 50]

desiredSampleRateInSecondsPayload = {
    "properties": {
        "desired": {
            "DesiredTemperature": 20
        }
    }
}

temperatures = [10, 50]

desiredTemperaturePayload = {
    "properties": {
        "desired": {
            "DesiredTemperature": 20,
            "DesiredSampleRateInSeconds":2
        }
    }
}

api_version = "2018-06-30"
url = f"https://{hub_name}.azure-devices.net/twins/{device_id}?api-version={api_version}"
headers = {'Authorization': authorization}

while True:

    try:

        # desiredSampleRateInSecondsPayload["properties"]["desired"]["DesiredSampleRateInSeconds"] = sample_rate[count % 4]

        # r = requests.patch(url, data=json.dumps(
        #     desiredSampleRateInSecondsPayload), headers=headers)

        # if r.status_code == 200:
        #     desired_sampleRateInSeconds_success_count += 1
        # else:
        #     DesiredSampleRateInSeconds_failed_count += 1

        desiredTemperaturePayload["properties"]["desired"]["DesiredTemperature"] = temperatures[count % 2]
        desiredTemperaturePayload["properties"]["desired"]["DesiredSampleRateInSeconds"] = sample_rate[count % 4]

        r = requests.patch(url, data=json.dumps(
            desiredTemperaturePayload), headers=headers)

        if r.status_code == 200:
            desired_temperature_success_count += 1
        else:
            desired_temperature_failed_count += 1
            
    except:
        pass

    count += 1

    print(
        f"Desired Temperature Total: {count} Success: {desired_temperature_success_count}, failed: {desired_temperature_failed_count}", flush=True)
    # print(
    #     f"Sample Rate Total: {count} Success: {desired_sampleRateInSeconds_success_count}, failed: {DesiredSampleRateInSeconds_failed_count}", flush=True)

    time.sleep(1)
