import requests
import json
import time
import os

device_id = os.environ["DEVICE_TWIN_DEVICE_ID"]
authorization = os.environ["DEVICE_TWIN_AUTHORIZATION"]
hub_name = os.environ["DEVICE_TWIN_HUB_NAME"]

count = 0
failed_count = 0
success_count = 0

temperatures = [5, 25, 50]

payload = {
    "properties": {
        "desired": {
            "DesiredTemperature": {
                "value": 20
            }
        }
    }
}

api_version = "2018-06-30"
url = f"https://{hub_name}.azure-devices.net/twins/{device_id}?api-version={api_version}"
headers = { 'Authorization': authorization }

while True:

    payload["properties"]["desired"]["DesiredTemperature"]["value"] = temperatures[count % 3]

    r = requests.patch(url, data=json.dumps(payload), headers=headers)

    if r.status_code == 200:
        success_count+=1
    else:
        failed_count+=1

    count+=1

    print(f"Total: {count} Success: {success_count}, failed: {failed_count}")

    time.sleep(0.25)
