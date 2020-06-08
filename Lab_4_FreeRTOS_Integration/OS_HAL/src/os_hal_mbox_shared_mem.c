/* Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 */

#ifdef OSAI_FREERTOS
#include "FreeRTOS.h"
#include <semphr.h>
#endif
#include "os_hal_mbox.h"
#include "os_hal_mbox_shared_mem.h"

#ifdef OSAI_FREERTOS
extern SemaphoreHandle_t blockFifoSema;
#else
volatile u8 blockFifoSema;
#endif

static void ReceiveMessage(u32 *command, u32 *data);
static u32 GetBufferSize(u32 bufferBase);
static BufferHeader *GetBufferHeader(u32 bufferBase);
static uint8_t *DataAreaOffset8(BufferHeader *header, u32 offset);
static u32 *DataAreaOffset32(BufferHeader *header, u32 offset);
static u32 RoundUp(u32 value, u32 alignment);

static void ReceiveMessage(u32 *command, u32 *data)
{
	struct mbox_fifo_item buf;
	u32 count;
	int ret;

	/* FIFO_POP_CNT */
	mtk_os_hal_mbox_ioctl(OS_HAL_MBOX_CH0, MBOX_IOGET_ACPT_FIFO_CNT,
			&count);
	while (count == 0) {
#ifdef OSAI_FREERTOS
		xSemaphoreTake(blockFifoSema, portMAX_DELAY);
#else
		while (blockFifoSema == 0)
			;
		blockFifoSema--;
#endif
		mtk_os_hal_mbox_ioctl(OS_HAL_MBOX_CH0,
			MBOX_IOGET_ACPT_FIFO_CNT, &count);
	}

	ret = mtk_os_hal_mbox_fifo_read(OS_HAL_MBOX_CH0, &buf,
					MBOX_TR_DATA_CMD);
	if (ret != MBOX_OK)
		printf("ReceiveMessage: read fifo failed\n");

	/* DATA_POP0 */
	*data = buf.data;
	/* CMD_POP0 */
	*command = buf.cmd;
}

static u32 GetBufferSize(u32 bufferBase)
{
	return (0x1 << (bufferBase & 0x1F));
}

static BufferHeader *GetBufferHeader(u32 bufferBase)
{
	return (BufferHeader *)(bufferBase & ~0x1F);
}

int GetIntercoreBuffers(BufferHeader **outbound,
			BufferHeader **inbound, u32 *bufSize)
{
	/* Wait for the mailbox to be set up. */
	u32 baseRead = 0, baseWrite = 0;

	while (true) {
		u32 cmd, data;

		ReceiveMessage(&cmd, &data);
		if (cmd == 0xba5e0001)
			baseWrite = data;
		else if (cmd == 0xba5e0002)
			baseRead = data;
		else if (cmd == 0xba5e0003)
			break;
	}

	u32 inboundBufferSize = GetBufferSize(baseRead);
	u32 outboundBufferSize = GetBufferSize(baseWrite);

	if (inboundBufferSize != outboundBufferSize) {
		printf("GetIntercoreBuffers: Mismatched buffer sizes\r\n");
		return -1;
	}

	if (inboundBufferSize <= sizeof(BufferHeader)) {
		printf("GetIntercoreBuffers: buffer size smaller than header\n");
		return -1;
	}

	*bufSize = inboundBufferSize - sizeof(BufferHeader);
	*inbound = GetBufferHeader(baseRead);
	*outbound = GetBufferHeader(baseWrite);

	return 0;
}

static uint8_t *DataAreaOffset8(BufferHeader *header, u32 offset)
{
	/* Data storage area following header in buffer. */
	uint8_t *dataStart = (uint8_t *)(header + 1);

	/* Offset within data storage area. */
	return dataStart + offset;
}

static u32 *DataAreaOffset32(BufferHeader *header, u32 offset)
{
	return (u32 *)DataAreaOffset8(header, offset);
}

static u32 RoundUp(u32 value, u32 alignment)
{
	/* alignment must be a power of two. */
	return (value + (alignment - 1)) & ~(alignment - 1);
}

int EnqueueData(BufferHeader *inbound, BufferHeader *outbound,
			u32 bufSize, const void *src, u32 dataSize)
{
	u32 remoteReadPosition = inbound->readPosition;
	u32 localWritePosition = outbound->writePosition;

	if (remoteReadPosition >= bufSize) {
		printf("EnqueueData: remoteReadPosition invalid\r\n");
		return -1;
	}

	/* If the read pointer is behind the write pointer,
	 * then the free space wraps around.
	 */
	u32 availSpace;

	if (remoteReadPosition <= localWritePosition)
		availSpace = remoteReadPosition - localWritePosition + bufSize;
	else
		availSpace = remoteReadPosition - localWritePosition;

	/* If there isn't enough space to enqueue a block,
	 * then abort the operation.
	 */
	if (availSpace < sizeof(u32) + dataSize + RINGBUFFER_ALIGNMENT) {
		printf("EnqueueData: not enough space to enqueue block\r\n");
		return -1;
	}

	/* Write up to end of buffer. If the block ends before then,
	 * only write up to the end of the block.
	 */
	u32 dataToEnd = bufSize - localWritePosition;

	/* There must be enough space between the write pointer and the end of
	 * the buffer to store the block size as a contiguous 4-byte value. The
	 * remainder of message can wrap around.
	 */
	if (dataToEnd < sizeof(u32)) {
		printf("EnqueueData: not enough space for block size\r\n");
		return -1;
	}

	u32 writeToEnd = sizeof(u32) + dataSize;

	if (dataToEnd < writeToEnd)
		writeToEnd = dataToEnd;

	/* Write block size to first word in block. */
	*DataAreaOffset32(outbound, localWritePosition) = dataSize;
	writeToEnd -= sizeof(u32);

	const uint8_t *src8 = src;
	uint8_t *dest8 = DataAreaOffset8(outbound,
				localWritePosition + sizeof(u32));

	__builtin_memcpy(dest8, src8, writeToEnd);
	__builtin_memcpy(DataAreaOffset8(outbound, 0), src8 + writeToEnd,
				dataSize - writeToEnd);

	/* Advance write position. */
	localWritePosition =
		RoundUp(localWritePosition + sizeof(u32) +
			dataSize, RINGBUFFER_ALIGNMENT);
	if (localWritePosition >= bufSize)
		localWritePosition -= bufSize;

	outbound->writePosition = localWritePosition;

	/* SW_TX_INT_PORT[0] = 1 -> indicate message received. */
	u32 sw_trig_int = 0;

	mtk_os_hal_mbox_ioctl(OS_HAL_MBOX_CH0,
				MBOX_IOSET_SWINT_TRIG, &sw_trig_int);
	return 0;
}

int DequeueData(BufferHeader *outbound, BufferHeader *inbound,
			u32 bufSize, void *dest, u32 *dataSize)
{
	u32 remoteWritePosition = inbound->writePosition;
	u32 localReadPosition = outbound->readPosition;

	if (remoteWritePosition >= bufSize) {
		printf("DequeueData: remoteWritePosition invalid\r\n");
		return -1;
	}

	u32 availData;

	/* If data is contiguous in buffer then difference
	 * between write and read positions...
	 */
	if (remoteWritePosition >= localReadPosition)
		availData = remoteWritePosition - localReadPosition;
	/* ...else data wraps around end and resumes at start of buffer */
	else
		availData = remoteWritePosition - localReadPosition + bufSize;

	/* There must be at least four contiguous bytes to hold the block
	 * size.
	 */
	if (availData < sizeof(u32)) {
		if (availData > 0)
			printf("DequeueData: availData < 4 bytes\r\n");

		return -1;
	}

	u32 dataToEnd = bufSize - localReadPosition;

	if (dataToEnd < sizeof(u32)) {
		printf("DequeueData: dataToEnd < 4 bytes\r\n");
		return -1;
	}

	u32 blockSize = *DataAreaOffset32(inbound, localReadPosition);

	/* Ensure the block size is no greater than the available data. */
	if (blockSize + sizeof(u32) > availData) {
		printf("DequeueData: message size greater than available data\r\n");
		return -1;
	}

	/* Abort if the caller-supplied buffer is not large enough
	 *to hold the message.
	 */
	if (blockSize > *dataSize) {
		printf("DequeueData: message too large for buffer\r\n");
		*dataSize = blockSize;
		return -1;
	}

	/* Tell the caller the actual block size. */
	*dataSize = blockSize;

	/* Read up to the end of the buffer. If the block ends before then,
	 * only read up to the end of the block.
	 */
	u32 readFromEnd = dataToEnd - sizeof(u32);

	if (blockSize < readFromEnd)
		readFromEnd = blockSize;

	const uint8_t *src8 =
		DataAreaOffset8(inbound, localReadPosition + sizeof(u32));
	uint8_t *dest8 = dest;

	__builtin_memcpy(dest8, src8, readFromEnd);
	/* If block wrapped around the end of the buffer,
	 * then read remainder from start.
	 */
	__builtin_memcpy(dest8 + readFromEnd,
		DataAreaOffset8(inbound, 0), blockSize - readFromEnd);

	/* Round read position to next aligned block,
	 * and wraparound end of buffer if required.
	 */
	localReadPosition =
		RoundUp(localReadPosition + sizeof(u32) +
			blockSize, RINGBUFFER_ALIGNMENT);
	if (localReadPosition >= bufSize)
		localReadPosition -= bufSize;

	outbound->readPosition = localReadPosition;

	/* SW_TX_INT_PORT[1] = 1 -> indicate message received. */
	u32 sw_trig_int = 1;

	mtk_os_hal_mbox_ioctl(OS_HAL_MBOX_CH0,
				MBOX_IOSET_SWINT_TRIG, &sw_trig_int);

	return 0;
}
