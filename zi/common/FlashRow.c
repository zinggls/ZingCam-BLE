#include "FlashRow.h"

static bool stored = false;
static FlashData_t flashData;

bool IsAddressStored()
{
    return stored;
}

const FlashData_t* GetFlashData()
{
    return &flashData;
}

const FlashData_t* LoadStoredPeripheralAddress()
{
    // Read data directly from flash memory
    const FlashData_t *flashPtr = (const FlashData_t *)FLASH_BASE_ADDR;

    // Validate magic number
    if (flashPtr->magic == MAGIC_NUMBER) {
        // Copy data if valid
        memcpy(&flashData, flashPtr, FLASH_DATA_SIZE);
        stored = true;
        return &flashData;
    } else {
        stored = false;
        return NULL;
    }
}

bool WriteToFlashRow(const void *data, size_t dataSize, cystatus *status)
{
    uint8_t flashRow[CY_FLASH_SIZEOF_ROW] = {0xFF}; // Prepare a buffer for the full flash row

    // Copy the provided data into the row buffer
    if (data != NULL && dataSize > 0) {
        memcpy(flashRow, data, dataSize);
    }

    // Write the flash row
    *status = CySysFlashWriteRow(FLASH_ROW_NUMBER, flashRow);

    // Check the status of the write operation
    return (*status == CY_SYS_FLASH_SUCCESS);
}

bool SavePeripheralAddress(const CYBLE_GAP_BD_ADDR_T *addr, cystatus *status)
{
    FlashData_t fd;
    fd.magic = MAGIC_NUMBER;
    memcpy(&fd.bdAddr, addr, sizeof(CYBLE_GAP_BD_ADDR_T));

    if (WriteToFlashRow(&fd, FLASH_DATA_SIZE, status)) {
        return true;
    } else {
        return false;
    }
}

bool ClearPeripheralAddress(cystatus *status)
{
    if (WriteToFlashRow(NULL, 0, status)) {
        return true;
    } else {
        return false;
    }
}