#ifndef FLASHROW_H
#define FLASHROW_H

#include "project.h"
#include <stdbool.h>

#define FLASH_ROW_NUMBER    (CY_FLASH_NUMBER_ROWS - 1u)
#define FLASH_BASE_ADDR     (CY_FLASH_BASE + (FLASH_ROW_NUMBER * CY_FLASH_SIZEOF_ROW))
#define FLASH_DATA_SIZE     (sizeof(FlashData_t))
#define MAGIC_NUMBER        0xA5A5A5A5  // Define a unique identifier for valid data

typedef struct {
    uint32_t magic;                  // Magic number for validation
    CYBLE_GAP_BD_ADDR_T bdAddr;      // Bluetooth device address
} FlashData_t;

FlashData_t gFlashData;              // Struct to hold flash data

bool LoadStoredPeripheralAddress(FlashData_t *fData);
bool WriteToFlashRow(const void *data, size_t dataSize, cystatus *status);
bool SavePeripheralAddress(const CYBLE_GAP_BD_ADDR_T *addr, cystatus *status);
bool ClearPeripheralAddress(cystatus *status);

#endif /* FLASHROW_H */