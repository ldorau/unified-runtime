/*
 *
 * Copyright (C) 2023 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#ifndef UMA_MEMORY_PROVIDER_H
#define UMA_MEMORY_PROVIDER_H 1

#include <uma/base.h>
#include <uma/memory_provider_ops.h>

#define UMA_ATTR_NORMAL 0          /* No further special treatment.  */
#define UMA_ATTR_RANDOM 1          /* Expect random page references.  */
#define UMA_ATTR_SEQUENTIAL 2      /* Expect sequential page references.  */
#define UMA_ATTR_WILLNEED 3        /* Will need these pages.  */
#define UMA_ATTR_DONTNEED 4        /* Don't need these pages.  */
#define UMA_ATTR_FREE 8            /* Free pages only if memory pressure.  */
#define UMA_ATTR_REMOVE 9          /* Remove these pages and resources.  */
#define UMA_ATTR_DONTFORK 10       /* Do not inherit across fork.  */
#define UMA_ATTR_DOFORK 11         /* Do inherit across fork.  */
#define UMA_ATTR_MERGEABLE 12      /* KSM may merge identical pages.  */
#define UMA_ATTR_UNMERGEABLE 13    /* KSM may not merge identical pages.  */
#define UMA_ATTR_HUGEPAGE 14       /* Worth backing with hugepages.  */
#define UMA_ATTR_NOHUGEPAGE 15     /* Not worth backing with hugepages.  */
#define UMA_ATTR_DONTDUMP 16       /* Explicity exclude from the core dump, */
                                   /* overrides the coredump filter bits.  */
#define UMA_ATTR_DODUMP 17         /* Clear the UMA_ATTR_DONTDUMP flag.  */
#define UMA_ATTR_WIPEONFORK 18     /* Zero memory on fork, child only.  */
#define UMA_ATTR_KEEPONFORK 19     /* Undo UMA_ATTR_WIPEONFORK.  */
#define UMA_ATTR_COLD 20           /* Deactivate these pages.  */
#define UMA_ATTR_PAGEOUT 21        /* Reclaim these pages.  */
#define UMA_ATTR_POPULATE_READ 22  /* Populate (prefault) page tables */
                                   /* readable. */
#define UMA_ATTR_POPULATE_WRITE 23 /* Populate (prefault) page tables */
                                   /* writable. */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct uma_memory_provider_t *uma_memory_provider_handle_t;

///
/// \brief Creates new memory provider
/// \param ops instance of uma_memory_provider_ops_t
/// \param params pointer to provider-specific parameters
/// \return UMA_RESULT_SUCCESS on success or appropriate error code on failure
///
enum uma_result_t
umaMemoryProviderCreate(struct uma_memory_provider_ops_t *ops, void *params,
                        uma_memory_provider_handle_t *hProvider);

///
/// \brief Destroys memory provider
/// \param hPool handle to the memory provider
///
void umaMemoryProviderDestroy(uma_memory_provider_handle_t hProvider);

///
/// \brief Allocates size bytes of uninitialized storage from memory provider
/// with
///        specified alignment
/// \param hProvider handle to the memory provider
/// \param size number of bytes to allocate
/// \param alignment alignment of the allocation
/// \param ptr returns pointer to the allocated memory
/// \return UMA_RESULT_SUCCESS on success or appropriate error code on failure
///
enum uma_result_t umaMemoryProviderAlloc(uma_memory_provider_handle_t hProvider,
                                         size_t size, size_t alignment,
                                         void **ptr);

///
/// \brief Frees the memory space pointed by ptr from the memory provider
/// \param hProvider handle to the memory provider
/// \param ptr pointer to the allocated memory
/// \param size size of the allocation
///
enum uma_result_t umaMemoryProviderFree(uma_memory_provider_handle_t hProvider,
                                        void *ptr, size_t size);

///
/// \brief Retrieve string representation of the underlying provider specific
///        result reported by the last API that returned
///        UMA_RESULT_ERROR_MEMORY_PROVIDER_SPECIFIC. Allows for a provider
///        independent way to return a provider specific result.
///
/// \details
///     - The string returned via the ppMessage is a NULL terminated C style
///       string.
///     - The string returned via the ppMessage is thread local.
///     - The memory in the string returned via the ppMessage is owned by the
///       adapter.
///     - The application may call this function from simultaneous threads.
///     - The implementation of this function should be lock-free.
///
/// \param hProvider handle to the memory provider
/// \param ppMessage [out] pointer to a string containing provider specific
///        result in string representation.
/// \return UMA_RESULT_SUCCESS if the result being reported is to be considered
///         a warning. Any other result code returned indicates that the
///         adapter specific result is an error.
enum uma_result_t
umaMemoryProviderGetLastResult(uma_memory_provider_handle_t hProvider,
                               const char **ppMessage);

///
/// \brief Set attributes of the memory space pointed by ptr from the memory provider
/// \param hProvider handle to the memory provider
/// \param ptr pointer to the allocated memory
/// \param size size of the allocation
/// \param attrs attributes of the memory space to be set
///
enum uma_result_t
umaMemoryProviderSetAttributes(uma_memory_provider_handle_t hProvider,
                               void *ptr, size_t size, int attrs);

#ifdef __cplusplus
}
#endif

#endif /* UMA_MEMORY_PROVIDER_H */
