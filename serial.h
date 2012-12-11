#ifndef STM32SPROG_SERIAL_H
#define STM32SPROG_SERIAL_H
/** \file serial.h
 *
 * Provides a serial communication interface.
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/** Serial device handle. */
typedef struct SSerialDev SerialDev;

/** \brief Open a serial device.
 *
 * \param devName The name of the device to open.
 * \param baud The baud rate.
 *
 * \return A new \ref SerialDev, or NULL if the device could not be opened.
 */
SerialDev *serialOpen(const char *devName, int baud);

/** \brief Close a serial device.  Frees resources used by the device.
 *
 * \param dev An open serial device.
 */
void serialClose(SerialDev *dev);

/** \brief Read data from a serial device.
 *
 * Blocks until all data has been read or an error has occurred.
 *
 * \param dev An open serial device.
 * \param buffer The data buffer to fill.
 * \param n The number of bytes to read.
 *
 * \return \c true on success, \c false if any error occurred.
 */
bool serialRead(SerialDev *dev, uint8_t *buffer, size_t n);

/** \brief Write data to a serial device.
 *
 * Blocks until all data has been written or an error has occurred.
 *
 * \param dev An open serial device.
 * \param buffer The data to write.
 * \param n The number of bytes to write.
 *
 * \return \c true on success, \c false if any error occurred.
 */
bool serialWrite(SerialDev *dev, const uint8_t *buffer, size_t n);

/** \brief Set the state of the DTR signal for a serial device.
 *
 * \param dev An open serial device.
 * \param dtr The new DTR state.
 *
 * \return \c true on success, \c false if any error occurred.
 */
bool serialSetDtr(SerialDev *dev, bool dtr);

#endif /* STM32SPROG_SERIAL_H */

