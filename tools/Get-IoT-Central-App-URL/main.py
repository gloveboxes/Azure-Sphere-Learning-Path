# pip install pip install azure-iot-device

import DeviceProvisionService as dps
import asyncio
import sys

import sys, getopt

async def main(argv):

    scope = None
    device = None
    key = None

    try:
        opts, args = getopt.getopt(argv,"s:d:k:")
    except getopt.GetoptError:
        print('expected main.py -s "scope id" -d "device id" -k "key"')
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-s':
            scope = arg
        elif opt in ("-d"):
            device = arg
        elif opt in ("-k"):
            key = arg

    device = dps.Device(scope, device, key)


    (IoTCentral) = await device.connection_string

    print("\n\nAzure IoT Central\n")
    print('"CmdArgs": [ "Fake", "{0}" ]'.format(scope))
    print('"AllowedConnections": [ "global.azure-devices-provisioning.net", "{0}"'.format(IoTCentral))
    print('"DeviceAuthentication": "--- YOUR AZURE SPHERE TENANT ID---"\n')

if __name__ == "__main__":
    asyncio.run(main(sys.argv[1:]))