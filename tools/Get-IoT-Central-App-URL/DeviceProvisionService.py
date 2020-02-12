import asyncio
from azure.iot.device.aio import ProvisioningDeviceClient


class Device():
    def __init__(self, scope, device_id, key):
        self.scope = scope
        self.device_id = device_id
        self.key = key

    async def __register_device(self):
        provisioning_device_client = ProvisioningDeviceClient.create_from_symmetric_key(
            provisioning_host='global.azure-devices-provisioning.net',
            registration_id=self.device_id,
            id_scope=self.scope,
            symmetric_key=self.key,
        )

        return await provisioning_device_client.register()

    @property
    async def connection_string(self):
        results = await asyncio.gather(self.__register_device())
        registration_result = results[0]

        # build the connection string
        # conn_str = 'HostName=' + registration_result.registration_state.assigned_hub + \
        #     ';DeviceId=' + self.device_id + \
        #     ';SharedAccessKey=' + self.key

        return (registration_result.registration_state.assigned_hub)

        # return conn_str