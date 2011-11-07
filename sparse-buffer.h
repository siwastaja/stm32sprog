#ifndef STM32SPROG_SPARSE_BUFFER_H
#define STM32SPROG_SPARSE_BUFFER_H
/** \file sparse-buffer.h */

#include <stddef.h>
#include <stdint.h>

/** \defgroup SparseBufferType SparseBuffer
 * \brief A sparse data buffer.
 * @{
 */

/** Sparse buffer handle. */
typedef struct SparseBuffer SparseBuffer;

/** \brief Sparse buffer constructor.
 *
 * \return A new sparse buffer object, which must be freed by
 *         SparseBuffer_destroy().
 */
SparseBuffer *SparseBuffer_create();

/** \brief Sparse buffer destructor.
 *
 * Cleans up all resources used by a sparse buffer instance.
 *
 * \param self The sparse buffer.
 */
void SparseBuffer_destroy(SparseBuffer *self);

/** \brief Sets data in a sparse buffer.
 *
 * \param self The sparse buffer.
 * \param offset The position of the first byte to set.
 * \param data The data.
 * \param n The size of data in bytes.
 */
void SparseBuffer_set(SparseBuffer *self,
        size_t offset, uint8_t *data, size_t n);

/*@}*/

#endif /* STM32SPROG_SPARSE_BUFFER_H */

