 
/***************************************************************************
 *  interface_mem_header.h - BlackBoard interface memory header
 *
 *  Generated: Fri Oct 20 13:41:29 2006
 *  Copyright  2006  Tim Niemueller [www.niemueller.de]
 *
 *  $Id$
 *
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __BLACKBOARD_INTERFACE_MEM_HEADER_H_
#define __BLACKBOARD_INTERFACE_MEM_HEADER_H_

#include <interface/interface.h>

#include <stdint.h>

/** This struct is used as header for interfaces in memory chunks.
 * This header is stored at the beginning of each allocated memory chunk.
 */
typedef struct {
  char             type[__INTERFACE_TYPE_SIZE];	/**< interface type */
  char             id[__INTERFACE_ID_SIZE];	/**< interface identifier */
  unsigned char    hash[__INTERFACE_HASH_SIZE];	/**< interface type version hash */
  uint16_t         flag_writer_active :  1;	/**< 1 if there is a writer, 0 otherwise */
  uint16_t         flag_reserved      : 15;	/**< reserved for future use */
  uint16_t         num_readers;			/**< number of active readers */
  uint32_t         refcount;			/**< reference count */
  uint32_t         serial;			/**< memory serial */
} interface_header_t;


#endif
