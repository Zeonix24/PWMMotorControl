#include "non-volatile_storage.hpp"

// Define a struct to store calibration data
struct __attribute__((aligned(4))) CalibrationData
{
    float zero_voltage;
    uint32_t magic_number;  // Used to validate if data has been written
};

// We'll use the last sector of flash memory for storing our data
#define FLASH_TARGET_OFFSET \
    ((PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE) & ~(FLASH_SECTOR_SIZE - 1))
#define CALIBRATION_MAGIC_NUMBER 0xAA55AA55

// Function to perform flash erase/program operations from RAM
// This ensures code execution does not halt when erasing flash
static bool __not_in_flash_func(save_data_to_flash)(const CalibrationData* data)
{
    // Disable interrupts before accessing flash
    uint32_t interrupts = save_and_disable_interrupts();

    // Erase the target sector
    printf("Erasing flash sector at offset 0x%lX\n", FLASH_TARGET_OFFSET);
    flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);

    // Add a delay to ensure erase completes
    sleep_ms(10);

    // Write the data to flash
    printf("Programming data to flash...\n");
    flash_range_program(FLASH_TARGET_OFFSET,
                        reinterpret_cast<const uint8_t*>(data),
                        sizeof(CalibrationData));

    // Add a delay to ensure program completes
    sleep_ms(10);

    // Restore interrupts
    restore_interrupts(interrupts);
    return true;
}

// Function to erase flash from RAM
static bool __not_in_flash_func(erase_flash_sector)()
{
    // Disable interrupts before accessing flash
    uint32_t interrupts = save_and_disable_interrupts();

    // Erase the target sector
    printf("Erasing flash sector at offset 0x%lX\n", FLASH_TARGET_OFFSET);
    flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);

    // Add a delay to ensure erase completes
    sleep_ms(10);

    // Restore interrupts
    restore_interrupts(interrupts);
    return true;
}

// Save zero voltage to flash memory
bool NonVolatileStorage::save_zero_voltage(float zero_voltage)
{
    // Prepare calibration data
    CalibrationData data;
    data.zero_voltage = zero_voltage;
    data.magic_number = CALIBRATION_MAGIC_NUMBER;

    printf("Preparing to save: zero_voltage=%f, magic=0x%lX\n",
           data.zero_voltage, data.magic_number);

    // Call the RAM-based function to save data
    bool result = save_data_to_flash(&data);

    if (result)
    {
        printf("Save operation completed successfully\n");
    } else
    {
        printf("Error during save operation\n");
    }

    return result;
}

// Retrieve zero voltage from flash memory
float NonVolatileStorage::get_zero_voltage()
{
    // Cast the flash memory location to our calibration data struct
    const CalibrationData* stored_data =
        reinterpret_cast<const CalibrationData*>(XIP_BASE +
                                                 FLASH_TARGET_OFFSET);

    // Check if the magic number is correct
    if (stored_data->magic_number == CALIBRATION_MAGIC_NUMBER)
    {
        printf("Valid calibration found: zero_voltage=%f\n",
               stored_data->zero_voltage);
        return stored_data->zero_voltage;
    }

    // If no valid calibration found, return a default value
    printf("No valid calibration found (magic=0x%lX), using default\n",
           stored_data->magic_number);
    return 2.50f;
}

// Check if calibration has been performed
bool NonVolatileStorage::is_calibrated()
{
    const CalibrationData* stored_data =
        reinterpret_cast<const CalibrationData*>(XIP_BASE +
                                                 FLASH_TARGET_OFFSET);
    bool is_valid = (stored_data->magic_number == CALIBRATION_MAGIC_NUMBER);
    printf("Calibration check: %s (magic=0x%lX)\n",
           is_valid ? "valid" : "invalid", stored_data->magic_number);

    return is_valid;
}

// Clear calibration data
void NonVolatileStorage::clear_calibration()
{
    printf("Clearing calibration data...\n");

    // Call the RAM-based function to erase flash
    bool result = erase_flash_sector();

    if (result)
    {
        printf("Calibration data cleared successfully\n");
    } else
    {
        printf("Error clearing calibration data\n");
    }
}