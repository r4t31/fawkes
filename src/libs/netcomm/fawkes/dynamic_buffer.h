
/***************************************************************************
 *  dynamic_buffer.h - A dynamic buffer
 *
 *  Created: Fri Jun 01 13:28:02 2007
 *  Copyright  2007  Daniel Beck
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
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02111-1307, USA.
 */

#include <netcomm/fawkes/message_datatypes.h>

#include <sys/types.h>
#include <stdint.h>

class DynamicBuffer
{
 public:
  DynamicBuffer(dynamic_list_t *db, size_t initial_buffer_size = 1024);
  DynamicBuffer(dynamic_list_t *db, void *buf, size_t size);
  virtual ~DynamicBuffer();

  void   append(const void *data, size_t data_size);
  void * buffer();
  size_t buffer_size();
  unsigned int num_elements();

  size_t real_buffer_size();

  bool   has_next();
  void * next(size_t *size);
  void   reset_iterator();

 private:

  typedef uint16_t element_header_t;

  void   increase();

  bool               _read_only;

  dynamic_list_t    *_db;
  void              *_buffer;
  size_t             _buffer_size;
  element_header_t  *_curhead;
  void              *_curdata;

  uint16_t           _it_curel;
  element_header_t  *_it_curhead;
  void              *_it_curdata;

};
