#include "heap.h"

#define HDR_SIZE sizeof(struct block_header)

void k_heap_init(struct k_heap *heap, void *buf, size_t size)
{
	if (size <= HDR_SIZE) return;

	struct block_header *hdr = buf;

	heap->free_list = hdr;

	hdr->next = NULL;
	hdr->size = size - HDR_SIZE;
}

void *k_heap_alloc(struct k_heap *heap, size_t alloc_size)
{
	const uint8_t key = irq_lock();

	struct block_header *hdr  = heap->free_list;
	struct block_header *prev = NULL;

	while (hdr) {
		if (alloc_size > hdr->size) {
			/* Block not large enough to be split */
			prev = hdr;
			hdr  = hdr->next;
			continue;
		}

		struct block_header *next_block;

		/* Increase the size to allocate if remaining space is not large
		 * enough for a new header.
		 */
		if (hdr->size <= alloc_size + HDR_SIZE) {
			/* Remove the block from the free list */
			next_block = hdr->next;
		} else {
			next_block = hdr;
			next_block->size -= alloc_size + HDR_SIZE;
			sys_ptr_shift(&hdr, next_block->size);
		}

		if (prev) {
			prev->next = next_block;
		} else {
			heap->free_list = next_block;
		}

		hdr->size = alloc_size;
		break;
	}

	irq_unlock(key);

	return hdr;
}

/**
 * @brief Check if two blocks are adjacent
 *
 * @param a Left block
 * @param b Right block
 * @return true
 * @return false
 */
bool block_is_adjacent(struct block_header *a, struct block_header *b)
{
	return sys_ptr_diff(a, b) == a->size + HDR_SIZE;
}

void blocks_assemble(struct block_header *a, struct block_header *b)
{
}

void k_heap_free(struct k_heap *heap, void *ptr)
{
	const uint8_t key = irq_lock();

	struct block_header *hdr = ptr;

	hdr->size += HDR_SIZE;

	struct block_header *prev = NULL;
	struct block_header *next = heap->free_list;

	while (next) {
		if (block_is_adjacent(hdr, next)) {
			hdr->size += next->size + HDR_SIZE;
			hdr->next = next->next;
			break;
		}

		prev = next;
		next = next->next;
	}

	if (prev) {
		if (block_is_adjacent(prev, hdr)) {
			prev->size += hdr->size + HDR_SIZE;
			prev->next = hdr->next;
		} else {
			prev->next = hdr;
		}
	} else {
		heap->free_list = hdr;
	}

	irq_unlock(key);
}