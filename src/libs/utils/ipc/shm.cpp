
/***************************************************************************
 *  shm.cpp - shared memory segment
 *
 *  Generated: Thu Jan 12 14:10:43 2006
 *  Copyright  2005-2006  Tim Niemueller [www.niemueller.de]
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

#include <utils/ipc/shm.h>
#include <utils/ipc/shm_exceptions.h>
#include <utils/ipc/shm_lister.h>
#include <utils/ipc/semset.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <cstring>
#include <limits.h>

/** @class SharedMemoryHeader utils/ipc/shm.h
 * Interface for shared memory header.
 * This class has to be implemented to be able to use shared memory segments.
 * It defines a set of properties for the shared memory segment that can be
 * searched for and printed out by an appropriate lister.
 *
 * @see SharedMemory
 * @see SharedMemoryLister
 * @ingroup IPC
 * @author Tim Niemueller
 *
 *
 * @fn SharedMemoryHeader::~SharedMemoryHeader()
 * Virtual destructor
 *
 * @fn bool SharedMemoryHeader::matches(void *memptr)
 * Method to check if the given memptr matches this header.
 * This method is called when searching for a shared memory segment to
 * open, list or erase it.
 * Implement this to distuinguish several shared memory segments that share
 * the same magic token.
 * @param memptr The memory chunk in the shared memory segment where to start
 * checking.
 * @return true, if the given data in the memory chunk matches this header, false
 * otherwise.
 *
 * @fn unsigned int SharedMemoryHeader::size()
 * Size of the header.
 * The size that is needed in the shared memory memptr to accomodate the
 * header data. This size has to fit all the data that will be stored in the
 * header. It must return the same size every time.
 *
 * @fn void SharedMemoryHeader::initialize(void *memptr)
 * Initialize the header.
 * This should initialize the header data in the given memptr from the
 * data of this SharedMemoryHeader derivate instance. It has to write out
 * all state information that is needed to identify the shared memory
 * segment later on.
 * @param memptr the memptr where the header data shall be written to.
 *
 * @fn void SharedMemoryHeader::set(void *memptr)
 * Set information from memptr.
 * Set the information stored in this SharedMemoryHeader derivate instance
 * from the data stored in the given memptr.
 * @param memptr The memptr where to copy data from.
 *
 * @fn size_t SharedMemoryHeader::data_size()
 * Return the size of the data.
 * The size of the data that will be stored in the shared memory segment.
 * This method has to return the same value everytime and may only depend
 * on the other data set in the header and written to the shared memory
 * segment.
 * @return the size of the data segment
 */


/** @class SharedMemory utils/ipc/shm.h
 * Shared memory segment.
 * This class gives access to shared memory segment to store arbitrary data.
 * With shared memory data can be shared between several applications. Special
 * means like semaphores have to be used to control access to the storage
 * to prevent data corruption.
 *
 * The shared memory segment is divided into three parts.
 * 1. General shared memory header
 * 2. Data-specific header
 * 3. Data
 *
 * The general header consists of a magic token of MagicTokenSize that is used
 * to find the basically compatible shared memory segments out of all existing
 * shared memory segments. This is done for convenience. Although in general
 * shared memory is accessed via keys or IDs it is easier from the maintenance
 * side to just scan the segments to find the correct one, especially if there
 * may be more than just one segment for the same application.
 * The header also includes a semaphore ID which is unused at the moment.
 *
 * The data-specific header is generated from a given SharedMemoryHeader
 * implementation. It can be used to store any information that is needed to
 * identify a specific shared memory segment and to store management data for
 * the data segment. It should always contain enough information to derive
 * the data segment size or if needed an explicit information about the memory
 * size.
 *
 * The data segment can be filled with any data you like.
 *
 * Semaphores have a simple protection mechanism using IPC semaphores. If a
 * shared memory segment already has a semaphore assigned at the time it is
 * opened this semaphore is automatically opened. In any case addSemaphore()
 * can be used to create (or open if it already exists) a semaphore for the
 * shared memory segment. Information about the semaphore is stored in the
 * shared memory general header.
 *
 * This class provides utilities to list, erase and check existence of given
 * shared memory segments. For this often a SharedMemoryLister is used that
 * takes care of formatting the output of the specific information about the
 * shared memory segment.
 *
 * @see SharedMemoryHeader
 * @see SharedMemorySegment
 * @see qa_shmem.cpp
 * @ingroup IPC
 *
 * @author Tim Niemueller
 */

/** @var SharedMemory::_memptr
 * Pointer to the data segment.
 */
/** @var SharedMemory::_mem_size
 * Total size of the segment, including headers
 */
/** @fn SharedMemory::_data_size
 * Size of the data segment only
 */
/** @var SharedMemory::_header
 * Data-specific header
 */
/** @var SharedMemory::_is_read_only
 * Read-only.
 * if true before attach() open segment read-only
 */
/** @var SharedMemory::_destroy_on_delete
 * destroy on delete.
 * If true before free() segment is destroyed.
 */
/** @var SharedMemory::_should_create
 * Create shared memory segment.
 * If true before attach shared memory segment is created if it does
 * not exist.
 */
/** @var SharedMemory::_magic_token
 * Magic token
 */
/** @var SharedMemory::_shm_magic_token
 * Magic token as stored in the shared memory segment
 */
/** @var SharedMemory::_shm_header
 * general header as stored in the shared memory segment
 */
/** @var SharedMemory::_shm_upper_bound
 * Upper bound of memory. Used by ptr to determine if the given address is valid.
 */
/** @var SharedMemory::_shm_offset
 * Offset to the master's base addr.
 */

/** The magic token size.
 * Your magic token identifier may have an arbitrary size. It is truncated
 * at MagicTokenSize bytes or filled with zeros up to a length of
 * MagicTokenSize bytes.
 */
const unsigned int SharedMemory::MagicTokenSize = 16;


/** Constructor for derivates.
 * This constructor may only be used by derivatives. It can be used to delay
 * the call to attach() to do other preparations like creating a
 * SharedMemoryHeader object.
 * @param magic_token magic token of the shared memory segment
 * @param is_read_only if true the shared memory segment is opened in
 *                     read-only mode
 * @param create       if true the shared memory segment is created if
 *                     no one matching the headers was found
 * @param destroy_on_delete if true the shared memory segment is destroyed
 *                          when this SharedMemory instance is deleted.
 */
SharedMemory::SharedMemory(char *magic_token,
			   bool is_read_only,
			   bool create,
			   bool destroy_on_delete)
{
  _magic_token = new char[MagicTokenSize];
  memset(_magic_token, 0, MagicTokenSize);
  strncpy(_magic_token, magic_token, MagicTokenSize);

  _is_read_only      = is_read_only;
  _destroy_on_delete = destroy_on_delete;
  _should_create     = create;

  _memptr          = NULL;
  _shm_magic_token = NULL;
  _shm_header      = NULL;
  _header          = NULL;
  _data_size       = 0;

  __semset         = NULL;
  __created        = false;
  __shared_mem     = NULL;
  __shared_mem_id  = 0;
  __shared_mem_upper_bound = NULL;
}


/** Create a new shared memory segment.
 * This will open a shared memory segment that exactly fits the given
 * SharedMemoryHeader. It the segment does not exist and create is assured
 * the segment is created from the given data, otherwise the SharedMemory
 * instance remains in an invalid state and an exception is thrown.
 * The segment can be destroyed automatically if the instance is destroyed.
 * Shared memory segments can be opened read-only.
 * @param magic_token This is the magic token discussed above that is used
 *                    to identify the shared memory segment. The magic_token
 *                    can be of arbitrary size but at most MagicTokenSize
 *                    bytes are used.
 * @param header      The data-sepcific header used for this shared memory
 *                    segment
 * @param is_read_only if true the shared memory segment is opened in
 *                     read-only mode
 * @param create       if true the shared memory segment is created if
 *                     no one matching the headers was found
 * @param destroy_on_delete if true the shared memory segment is destroyed
 *                          when this SharedMemory instance is deleted.
 * @exception ShmNoHeaderException No header has been set
 * @exception ShmInconsistentSegmentSizeException The memory size is not the
 *                                                expected memory size
 * @exception ShmCouldNotAttachException Could not attach to shared
 *                                       memory segment
 */
SharedMemory::SharedMemory(const char *magic_token,
			   SharedMemoryHeader *header,
			   bool is_read_only, bool create, bool destroy_on_delete)
{
  _magic_token = new char[MagicTokenSize];
  memset(_magic_token, 0, MagicTokenSize);
  strncpy(_magic_token, magic_token, MagicTokenSize);

  _header            = header;
  _is_read_only      = is_read_only;
  _destroy_on_delete = destroy_on_delete;
  _should_create     = create;

  _memptr          = NULL;
  _shm_magic_token = NULL;
  _shm_header      = NULL;
  _data_size       = 0;

  __created         = false;
  __semset          = NULL;
  __shared_mem     = NULL;
  __shared_mem_id  = 0;
  __shared_mem_upper_bound = NULL;

  try {
    attach();
  } catch (Exception &e) {
    e.append("SharedMemory public constructor");
    throw;
  }

  if (_memptr == NULL) {
    throw ShmCouldNotAttachException("Could not attach to created shared memory segment");
  }
}


/** Destructor */
SharedMemory::~SharedMemory()
{
  delete[] _magic_token;
  free();
  if ( __semset != NULL ) {
    // if we destroy the shared memory region we can as well delete the semaphore,
    // it is not necessary anymore.
    __semset->setDestroyOnDelete( _destroy_on_delete );
    delete __semset;
  }
}


/** Detach from and maybe destroy the shared memory segment.
 * This will detach from the shared memory segment. If destroy_on_delete is
 * true this will destroy the shared memory segment before detaching.
 */
void
SharedMemory::free()
{
  _memptr = NULL;
  _shm_header = NULL;
  _shm_magic_token = NULL;

  if ((__shared_mem_id != -1) && !_is_read_only && _destroy_on_delete ) {
    shmctl(__shared_mem_id, IPC_RMID, NULL);
    __shared_mem_id = -1;
  }
  if (__shared_mem != NULL) {
    shmdt(__shared_mem);
    __shared_mem = NULL;
  }
}


/** Attach to the shared memory segment.
 * This method will try to open and/or create the shared memory segment.
 * @exception ShmNoHeaderException No header has been set
 * @exception ShmInconsistentSegmentSizeException The memory size is not the
 *                                                expected memory size
 * @exception ShmCouldNotAttachException Could not attach to shared
 *                                       memory segment
 */
void
SharedMemory::attach()
{

  if (_header == NULL) {
    // No shared memory header, needed!
    throw ShmNoHeaderException();
  }

  if ((_memptr != NULL) && (__shared_mem_id != -1)) {
    // a memptr has already been attached
    return;
  }

  // based on code by ipcs and Philipp Vorst from allemaniACs3D
  int              max_id;
  int              shm_id;
  struct shmid_ds  shm_segment;
  void            *shm_buf;
  void            *shm_ptr;

  // Find out maximal number of existing SHM segments
  struct shm_info shm_info;
  max_id = shmctl( 0, SHM_INFO, (struct shmid_ds *)&shm_info );

  if (max_id >= 0) {
    for ( int i = 0; (_memptr == NULL) && (i <= max_id); ++i ) {

      shm_id = shmctl( i, SHM_STAT, &shm_segment );
      if ( shm_id < 0 )  continue;

      shm_buf = shmat(shm_id, NULL, _is_read_only ? SHM_RDONLY : 0);
      if (shm_buf != (void *)-1) {
	// Attached

	_shm_magic_token = (char *)shm_buf;
	_shm_header = (SharedMemory_header_t *)((char *)shm_buf + MagicTokenSize);

	if ( strncmp(_shm_magic_token, _magic_token, MagicTokenSize) == 0 ) {

	  shm_ptr = (char *)shm_buf + MagicTokenSize
                                    + sizeof(SharedMemory_header_t);

	  if ( _header->matches( shm_ptr ) ) {
	    // matching memory segment found

	    _header->set( shm_ptr );
	    _data_size = _header->data_size();
	    _mem_size  = sizeof(SharedMemory_header_t) + MagicTokenSize
	                                               + _header->size() + _data_size;

	    if (_mem_size != (unsigned int) shm_segment.shm_segsz) {
	      throw ShmInconsistentSegmentSizeException(_mem_size,
							(unsigned int) shm_segment.shm_segsz);
	    }

	    _header->set( shm_ptr );
	    // header->printInfo();

	    __shared_mem_id   = shm_id;
	    __shared_mem      = shm_buf;
	    __shared_mem_upper_bound = (void *)((size_t)__shared_mem + _mem_size);
	    _shm_upper_bound   = (void *)((size_t)_shm_header->shm_addr + _mem_size);
	    _memptr            = (char *)shm_ptr + _header->size();
	    _shm_offset        = (size_t)__shared_mem - (size_t)_shm_header->shm_addr;

	    if ( _shm_header->semaphore != 0 ) {
	      // Houston, we've got a semaphore, open it!
	      add_semaphore();
	    }

	  } else {
	    // not the wanted memory segment
	    shmdt(shm_buf);
	  }
	} else {
	  // not our region of memory
	  shmdt(shm_buf);
	}
      } // else could not attach, ignore
    }
  }

  if ((_memptr == NULL) && ! _is_read_only && _should_create) {
    // try to create a new shared memory segment
    __created = true;
    key_t key = 1;

    _data_size = _header->data_size();
    _mem_size  = sizeof(SharedMemory_header_t) + MagicTokenSize + _header->size() + _data_size;
    while ((_memptr == NULL) && (key < INT_MAX)) {
    // no shm segment found, create one
      __shared_mem_id = shmget(key, _mem_size, IPC_CREAT | IPC_EXCL | 0666);
      if (__shared_mem_id != -1) {
	__shared_mem = shmat(__shared_mem_id, NULL, 0);
	if (__shared_mem != (void *)-1) {
	  memset(__shared_mem, 0, _mem_size);

	  _shm_magic_token = (char *)__shared_mem;
	  _shm_header = (SharedMemory_header_t *)((char *)__shared_mem + MagicTokenSize);
	  _shm_header->shm_addr = __shared_mem;

	  _memptr     = (char *)__shared_mem + MagicTokenSize
	                                     + sizeof(SharedMemory_header_t)
                                             + _header->size();
	  _shm_upper_bound = (void *)((size_t)__shared_mem + _mem_size);
	  _shm_offset      = 0;
	  __shared_mem_upper_bound = _shm_upper_bound;

	  strncpy(_shm_magic_token, _magic_token, MagicTokenSize);

	  _header->initialize( (char *)__shared_mem + MagicTokenSize
			                            + sizeof(SharedMemory_header_t));
	} else {
	  // It didn't work out, destroy shared mem and try again
	  shmctl(__shared_mem_id, IPC_RMID, NULL);
	  throw ShmCouldNotAttachException("Could not create shared memory segment");
	}
      } else {
	if (errno == EEXIST) {
	  // non-free key number, try next one
	  // note: we don't care about existing shared memory regions as we scanned
	  // them before already!
	  ++key;
	} else if (errno == EINVAL) {
	  throw ShmCouldNotAttachException("Could not attach, segment too small or too big");
	} else {
	  throw ShmCouldNotAttachException("Could not attach, shmget failed");
	}
      }
    }
    if (_memptr == NULL) {
      throw ShmCouldNotAttachException("Could not attach, memptr still NULL");
    }
  }

  /*
    printf("__shared_mem=0x%lX  _memptr=0x%lX  _mem_size=%lu  _data_size=%lu  m-s=%lu  ms-ds=%lu\n",
           (size_t)__shared_mem, (size_t)_memptr, _mem_size, _data_size,
           (size_t)_memptr - (size_t)__shared_mem, _mem_size - _data_size);
  */

}


/** Get the real pointer to the data based on an address.
 * If there is address-dependent data in the shared memory segment (like pointers
 * to the next element in a linked list) these are only valid for the process
 * that created the shared memory segment, they are not necessarily valid for
 * other processes.
 *
 * The function takes an address that has been stored in the
 * shared memory segment and transforms it into a valid local pointer.
 * Not that this does only work with pointers inside the shared memory segment.
 * You can only tranform addresses that point to somewhere inside the shared
 * memory segment!
 *
 * We could also have added local offsets, starting with 0 at the beginning
 * of the shared memory segment. We decided against this since our major our
 * main concern is that this works fast for the master, because this will be the
 * Fawkes main application, and for attached processes it may work slower and
 * we don't care.
 *
 * @param addr memory address read from the shared memory segment
 * @return pointer inside the shared memory segment
 * @exception ShmAddrOutOfBoundsException This exception is thrown if addr is not NULL,
 * smaller than the base addr and greater or equal to the base addr plus the memory size.
 * @see addr()
 */
void *
SharedMemory::ptr(void *addr)
{
  if ( _shm_offset == 0 )  return addr;
  if ( addr == NULL) return NULL;
  if ( (addr < _shm_header->shm_addr) ||
       (addr >= _shm_upper_bound) ) {
    throw ShmAddrOutOfBoundsException();
  }
  return (void *)((size_t)addr + _shm_offset);
}


/** Get an address from a real pointer.
 * If there is address-dependent data in the shared memory segment (like pointers
 * to the next element in a linked list) these are only valid for the process
 * that created the shared memory segment, they are not necessarily valid for
 * other processes.
 *
 * This method takes a pointer that points to data in the shared memory segment
 * that is valid in the local process and transform it to a pointer that is valid
 * inside the shared memory segment with respect to the base address used by the
 * creating process.
 *
 * @param ptr pointer to data inside the shared memory segment
 * @return  memory address valid for the creator of the shared memory segment
 * @exception ShmPtrOutOfBoundsException This exception is thrown if ptr is not NULL,
 * smaller than the local base ptr and greater or equal to the local base ptr plus
 * the memory size.
 * @see ptr()
 */
void *
SharedMemory::addr(void *ptr)
{
  if ( _shm_offset == 0 )  return ptr;
  if ( ptr == NULL) return NULL;
  if ( (ptr < __shared_mem) ||
       (ptr >= __shared_mem_upper_bound) ) {
    throw ShmPtrOutOfBoundsException();
  }
  return (void *)((size_t)ptr - _shm_offset);
}


/** Check for read-only mode
 * @return true, if the segment is opened in read-only mode, false otherwise
 */
bool
SharedMemory::is_read_only()
{
  return _is_read_only;
}


/** Determine if the shared memory segment has been created by this instance.
 * In some situations you want to know if the current instance has created the shared
 * memory segment or if it attached to an existing shared memory segment. This is
 * handy for example in master-slave constellations where one process is the master
 * over a given shared memory segment and other slaves may read but need special
 * means to alter the data.
 * This is a somewhat softer variant of exclusive access.
 * @return true, if this instance of SharedMemory created the segment, false
 * otherwise
 */
bool
SharedMemory::is_creator()
{
  return __created;
}

/** Get a pointer to the shared memory
 * This method returns a pointer to the data-segment of the shared memory
 * segment. It has the size stated as dataSize() from the header.
 * @return pointer to the data-segment
 * @see getDataSize()
 */
void *
SharedMemory::memptr()
{
  return _memptr;
}


/** Get the size of the data-segment.
 * Use this method to get the size of the data segment. Calls dataSize() of
 * the data-specific header internally.
 * @return size of the data-segment in bytes
 */
size_t
SharedMemory::data_size()
{
  return _data_size;
}


/** Copies data from the memptr to shared memory.
 * Use this method to copy data from the given external memptr to the
 * data segment of the shared memory.
 * @param memptr the memptr to copy from
 */
void
SharedMemory::set(void *memptr)
{
  memcpy(_memptr, memptr, _data_size);
}


/** Check if segment has been destroyed
 * This can be used if the segment has been destroyed. This means that no
 * other process can connect to the shared memory segment. As long as some
 * process is attached to the shared memory segment the segment will still
 * show up in the list
 * @return true, if this shared memory segment has been destroyed, false
 *         otherwise
 */
bool
SharedMemory::is_destroyed()
{
  return is_destroyed(__shared_mem_id);
}


/** Check if memory can be swapped out.
 * This method can be used to check if the memory can be swapped.
 * @return true, if the memory can be swapped, false otherwise
 */
bool
SharedMemory::is_swapable()
{
  return is_swapable(__shared_mem_id);
}


/** Check validity of shared memory segment.
 * Use this to check if the shared memory segmentis valid. That means that
 * this instance is attached to the shared memory and data can be read from
 * or written to the memptr.
 * @return true, if the shared memory segment is valid and can be utilized,
 *         false otherwise
 */
bool
SharedMemory::is_valid()
{
  return (_memptr != NULL);
}


/** Check if memory segment is protected.
 * This method can be used to determine if a semaphore has been associated to
 * this shared memory segment. Locking is not guaranteed, it depends on the
 * application. Use lock(), tryLock() and unlock() appropriately. You can do
 * this always, also if you start with unprotected memory. The operations are
 * just noops in that case. Protection can be enabled by calling addSemaphore().
 * If a memory segment was protected when it was opened it is automatically
 * opened in protected mode.
 * @return true, if semaphore is associated to memory, false otherwise
 */
bool
SharedMemory::is_protected()
{
  return (__semset != NULL);
}


/** Set deletion behaviour.
 * This has the same effect as the destroy_on_delete parameter given to the
 * constructor.
 * @param destroy set to true to destroy the shared memory segment on
 *        deletion
 */
void
SharedMemory::set_destroy_on_delete(bool destroy)
{
  _destroy_on_delete = destroy;
}


/** Add semaphore to shared memory segment.
 * This adds a semaphore to the system and puts its key in the shared memory
 * segment header. The semaphore can then be protected via the semaphore by
 * appropriate locking. If a semaphore has been assigned to the shared memory
 * segment already but after the segment was opened the semaphore is opened
 * and no new semaphore is created.
 */
void
SharedMemory::add_semaphore()
{
  if (__semset != NULL)  return;

  if ( _shm_header->semaphore != 0 ) {
    // a semaphore has been created but not been opened
    __semset = new SemaphoreSet( _shm_header->semaphore,
				 /* num sems    */ 1,
				 /* create      */ false,
				 /* dest on del */ false );
  } else {
    __semset = new SemaphoreSet( /* num sems    */ 1,
				 /* dest on del */ true );
    // one and only one may lock the memory
    __semset->unlock();
    _shm_header->semaphore = __semset->getKey();
  }
}


/** Set shared memory swapable.
 * Setting memory unswapable (in terms of Linux memory management: lock all
 * pages related to this memory segment) will only succeed for very small
 * portions of memory. A resource limit is implied (see getrlimit(2)). In
 * most cases the maximum amout of locked memory is about 32 KB.
 * @param swapable set to true, if memory should be allowed to be swaped out.
 */
void
SharedMemory::set_swapable(bool swapable)
{
  if (swapable) {
    shmctl(__shared_mem_id, SHM_UNLOCK, NULL);
  } else {
    shmctl(__shared_mem_id, SHM_LOCK, NULL);
  }
}


/** Lock shared memory segment.
 * If the shared memory segment is protected by an associated semaphore it can be
 * locked with this semaphore by calling this method.
 * @see isProtected()
 * @see unlock()
 * @see tryLock()
 */
void
SharedMemory::lock()
{
  if ( __semset == NULL ) {
    return;
  }
  __semset->lock();
}


/** Try to aquire lock on shared memory segment.
 * If the shared memory segment is protected by an associated semaphore it can be
 * locked. With tryLock() you can try to aquire the lock, but the method will not
 * block if it cannot get the lock but simply return false. This can be used to detect
 * if memory is locked:
 * @code
 * if (mem->tryLock()) {
 *   // was not locked
 *   mem->unlock();
 * } else {
 *   // is locked
 * }
 * @endcode
 * @see isProtected()
 * @see unlock()
 * @see lock()
 */
bool
SharedMemory::try_lock()
{
  if ( __semset == NULL )  return false;
  
  return __semset->tryLock();
}


/** Unlock memory.
 * If the shared memory segment is protected by an associated semaphore it can be
 * locked. With unlock() you lift the lock on the memory. Be aware that unlocking
 * a not-locked piece of memory will result in havoc and insanity! Have only exactly
 * guaranteed pairs of lock/successful tryLock() and unlock()!
 */
void
SharedMemory::unlock()
{
  if ( __semset == NULL )  return;
  __semset->unlock();
}


/* ==================================================================
 * STATICs
 */

/** Check if a segment has been destroyed.
 * Check for a shared memory segment of the given ID.
 * @param shm_id ID of the shared memory segment.
 * @return true, if the shared memory segment was destroyed, false otherwise.
 * @exception ShmDoesNotExistException No shared memory segment exists for
 *                                     the given ID.
 */
bool
SharedMemory::is_destroyed(int shm_id)
{
  struct shmid_ds  shm_segment;
  struct ipc_perm *perm = &shm_segment.shm_perm;

  if (shmctl(shm_id, IPC_STAT, &shm_segment ) == -1) {
    throw ShmDoesNotExistException();
    return false;
  } else {
    return (perm->mode & SHM_DEST);
  }
}


/** Check if memory can be swapped out.
 * This method can be used to check if the memory can be swapped.
 * @param shm_id ID of the shared memory segment.
 * @return true, if the memory can be swapped, false otherwise
 */
bool
SharedMemory::is_swapable(int shm_id)
{
  struct shmid_ds  shm_segment;
  struct ipc_perm *perm = &shm_segment.shm_perm;

  if (shmctl(shm_id, IPC_STAT, &shm_segment ) < 0) {
    return true;
  } else {
    return ! (perm->mode & SHM_LOCKED);
  }
}


/** Get number of attached processes.
 * @param shm_id ID of the shared memory segment.
 * @return number of attached processes
 */
unsigned int
SharedMemory::num_attached(int shm_id)
{
  struct shmid_ds  shm_segment;

  if (shmctl(shm_id, IPC_STAT, &shm_segment ) < 0) {
    return 0;
  } else {
    return shm_segment.shm_nattch;
  }
}


/** List shared memory segments of a given type.
 * This method lists all shared memory segments that match the given magic
 * token (first MagicTokenSize bytes, filled with zero) and the given
 * header. The lister is called to format the output.
 * @param magic_token Token to look for
 * @param header      header to identify interesting segments with matching
 *                    magic_token
 * @param lister      Lister used to format output
 */
void
SharedMemory::list(char *magic_token,
		   SharedMemoryHeader *header, SharedMemoryLister *lister)
{

  lister->printHeader();

  int              max_id;
  int              shm_id;
  struct shmid_ds  shm_segment;
  void            *shm_buf;

  char                  *shm_magic_token;
  SharedMemory_header_t *shm_header;

  unsigned int     num_segments = 0;

  // Find out maximal number of existing SHM segments
  struct shm_info shm_info;
  max_id = shmctl( 0, SHM_INFO, (struct shmid_ds *)&shm_info );
  if (max_id >= 0) {
    for ( int i = 0; i <= max_id; ++i ) {

      shm_id = shmctl( i, SHM_STAT, &shm_segment );
      if ( shm_id < 0 )  continue;

      shm_buf = shmat(shm_id, NULL, SHM_RDONLY);
      if (shm_buf != (void *)-1) {
	// Attached

	shm_magic_token = (char *)shm_buf;
	shm_header = (SharedMemory_header_t *)((char *)shm_buf + MagicTokenSize);

	if (strncmp(shm_magic_token, magic_token, MagicTokenSize) == 0) {
	  if ( header->matches( (char *)shm_buf + MagicTokenSize
                                                         + sizeof(SharedMemory_header_t)) ) {
	    header->set((char *)shm_buf + MagicTokenSize
			                         + sizeof(SharedMemory_header_t));
	    lister->printInfo(header, shm_id, shm_header->semaphore,
			      shm_segment.shm_segsz,
			      (char *)shm_buf + MagicTokenSize
                                                       + sizeof(SharedMemory_header_t)
                                                       + header->size());
	    ++num_segments;
	  }
	}
	shmdt(shm_buf);
      }
    }
    if ( num_segments == 0 ) {
      lister->printNoSegments();
    }
  }

  lister->printFooter();
}


/** Erase shared memory segments of a given type.
 * This method erases (destroys) all shared memory segments that match the
 * given magic token (first MagicTokenSize bytes, filled with zero) and the
 * given header. The lister is called to format the output. If a semaphore
 * has been assigned to this shared memory segment it is destroyed as well.
 * @param magic_token Token to look for
 * @param header      header to identify interesting segments with matching
 *                    magic_token
 * @param lister      Lister used to format output, maybe NULL (default)
 */
void
SharedMemory::erase(char *magic_token,
		    SharedMemoryHeader *header, SharedMemoryLister *lister)
{

  if (lister != NULL) lister->printHeader();

  int              max_id;
  int              shm_id;
  struct shmid_ds  shm_segment;
  void            *shm_buf;

  char                  *shm_magic_token;
  SharedMemory_header_t *shm_header;
  unsigned int     num_segments = 0;

  // Find out maximal number of existing SHM segments
  struct shm_info shm_info;
  max_id = shmctl( 0, SHM_INFO, (struct shmid_ds *)&shm_info );
  if (max_id >= 0) {
    for ( int i = 0; i <= max_id; ++i ) {

      shm_id = shmctl( i, SHM_STAT, &shm_segment );
      if ( shm_id < 0 )  continue;

      shm_buf = shmat(shm_id, NULL, SHM_RDONLY);
      if (shm_buf != (void *)-1) {
	// Attached

	shm_magic_token = (char *)shm_buf;
	shm_header = (SharedMemory_header_t *)((char *)shm_buf + MagicTokenSize);

	if (strncmp(shm_magic_token, magic_token, MagicTokenSize) == 0) {
	  if ( header->matches( (char *)shm_buf + MagicTokenSize
                                                + sizeof(SharedMemory_header_t)) ) {
	    header->set((char *)shm_buf + MagicTokenSize
			                + sizeof(SharedMemory_header_t));

	    if ( shm_header->semaphore != 0 ) {
	      // a semaphore has been assigned, destroy!
	      SemaphoreSet::destroy(shm_header->semaphore);
	    }
	    // Mark shared memory segment as destroyed
	    shmctl(shm_id, IPC_RMID, NULL);

	    if ( lister != NULL)
	      lister->printInfo(header, shm_id, shm_header->semaphore,
				shm_segment.shm_segsz,
				(char *)shm_buf + MagicTokenSize
				                + sizeof(SharedMemory_header_t)
				                + header->size());
	    ++num_segments;
	  }
	}
	shmdt(shm_buf);
      }
    }
    if ( num_segments == 0 ) {
      if (lister != NULL) lister->printNoSegments();
    }
  }

  if (lister != NULL) lister->printFooter();
}


/** Erase orphaned (attach count = 0) shared memory segments of a given type.
 * This method erases (destroys) all shared memory segments that match the
 * given magic token (first MagicTokenSize bytes, filled with zero) and the
 * given header and where no process is attached to. If a semaphore has been
 * assigned to this shared memory segment it is destroyed as well.
 * The lister is called to format the output.
 * @param magic_token Token to look for
 * @param header      header to identify interesting segments with matching
 *                    magic_token
 * @param lister      Lister used to format output, maybe NULL (default)
 */
void
SharedMemory::erase_orphaned(char *magic_token,
			     SharedMemoryHeader *header, SharedMemoryLister *lister)
{

  if (lister != NULL) lister->printHeader();

  int              max_id;
  int              shm_id;
  struct shmid_ds  shm_segment;
  void            *shm_buf;

  char                  *shm_magic_token;
  SharedMemory_header_t *shm_header;
  unsigned int     num_segments = 0;

  // Find out maximal number of existing SHM segments
  struct shm_info shm_info;
  shm_segment.shm_nattch = 0;
  max_id = shmctl( 0, SHM_INFO, (struct shmid_ds *)&shm_info );
  if (max_id >= 0) {
    for ( int i = 0; i <= max_id; ++i ) {

      shm_id = shmctl( i, SHM_STAT, &shm_segment );
      if ( shm_id < 0 )  continue;
      if ( shm_segment.shm_nattch > 0 ) continue;

      shm_buf = shmat(shm_id, NULL, SHM_RDONLY);
      if (shm_buf != (void *)-1) {
	// Attached

	shm_magic_token = (char *)shm_buf;
	shm_header = (SharedMemory_header_t *)((char *)shm_buf + MagicTokenSize);

	if (strncmp(shm_magic_token, magic_token, MagicTokenSize) == 0) {
	  if ( header->matches( (char *)shm_buf + MagicTokenSize
                                                + sizeof(SharedMemory_header_t)) ) {
	    header->set((char *)shm_buf + MagicTokenSize
			                + sizeof(SharedMemory_header_t));

	    if ( shm_header->semaphore != 0 ) {
	      // a semaphore has been assigned, destroy!
	      SemaphoreSet::destroy(shm_header->semaphore);
	    }
	    // Mark shared memory segment as destroyed
	    shmctl(shm_id, IPC_RMID, NULL);

	    if ( lister != NULL)
	      lister->printInfo(header, shm_id, shm_header->semaphore,
				shm_segment.shm_segsz,
				(char *)shm_buf + MagicTokenSize
				                + sizeof(SharedMemory_header_t)
				                + header->size());
	    ++num_segments;
	  }
	}
	shmdt(shm_buf);
      }
    }
    if ( num_segments == 0 ) {
      if (lister != NULL) lister->printNoOrphanedSegments();
    }
  }

  if (lister != NULL) lister->printFooter();
}


/** Check if a specific shared memory segment exists.
 * This method will search for a memory chunk that matches the given magic
 * token and header.
 * @param magic_token Token to look for
 * @param header      header to identify interesting segments with matching
 *                    magic_token
 * @return true, if a matching shared memory segment was found, else
 * otherwise
 */
bool
SharedMemory::exists(char *magic_token,
		     SharedMemoryHeader *header)
{
  int              max_id;
  int              shm_id;
  struct shmid_ds  shm_segment;
  void            *shm_buf;

  char                  *shm_magic_token;
  SharedMemory_header_t *shm_header;

  // Find out maximal number of existing SHM segments
  struct shm_info shm_info;
  bool found = false;
  max_id = shmctl( 0, SHM_INFO, (struct shmid_ds *)&shm_info );
  if (max_id >= 0) {
    for ( int i = 0; (! false) && (i <= max_id); ++i ) {

      shm_id = shmctl( i, SHM_STAT, &shm_segment );
      if ( shm_id < 0 )  continue;

      shm_buf = shmat(shm_id, NULL, SHM_RDONLY);
      if (shm_buf != (void *)-1) {
	// Attached

	shm_magic_token = (char *)shm_buf;
	shm_header = (SharedMemory_header_t *)((char *)shm_buf + MagicTokenSize);

	if (strncmp(shm_magic_token, magic_token, MagicTokenSize) == 0) {
	  if ( header->matches((char *)shm_buf + MagicTokenSize
			                       + sizeof(SharedMemory_header_t)) ) {
	    found = true;
	  }
	}
	shmdt(shm_buf);
      }
    }
  }

  return found;
}
