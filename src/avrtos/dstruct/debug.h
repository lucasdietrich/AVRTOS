/*
 * Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _DSTRUCT_DEBUG_H
#define _DSTRUCT_DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <avrtos/misc/serial.h>

#include "dlist.h"
#include "slist.h"
#include "tqueue.h"

//
// SList
//
void print_slist(struct slist *list, void (*qitem_printer)(struct snode *item));

//
// DList
//
void print_dlist(struct dnode *dlist, void (*ditem_printer)(struct dnode *item));

//
// TQueue
//
void print_tqueue(struct titem *root, void (*titem_printer)(struct titem *item));

#ifdef __cplusplus
}
#endif

#endif