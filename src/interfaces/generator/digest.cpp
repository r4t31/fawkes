 
/***************************************************************************
 *  digest.cpp - Interface config parser
 *
 *  Created: Thu Feb 28 15:51:20 2008
 *  Copyright  2006-2008  Tim Niemueller [www.niemueller.de]
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

#include <interfaces/generator/digest.h>
#include <interfaces/generator/exceptions.h>

#include <cstdio>
#include <openssl/evp.h>

#define FILE_STEP 1024


/** @class InterfaceDigest <interfaces/generator/digest.h>
 * Interface digest generator.
 * Creates the MD5 hash of the given config file.
 * @author Tim Niemueller
 */


/** Constructor
 * @param config_filename file name of config (interface template)
 */
InterfaceDigest::InterfaceDigest(std::string config_filename)
{
  digest = NULL;

  EVP_MD_CTX ctx;
  if ( ! EVP_DigestInit(&ctx, EVP_md5())) {
    throw Exception("Could not initialize digest context");
  }

  FILE *f = fopen(config_filename.c_str(), "r");
  void *buf = malloc(FILE_STEP);
  while ( ! feof(f) && ! ferror(f) ) {
    size_t rb;
    if ((rb = fread(buf, 1, FILE_STEP, f)) > 0) {
      if ( ! EVP_DigestUpdate(&ctx, buf, rb) ) {
	fclose(f);
	throw Exception("Failed to update digest");
      }
    }
  }
  if ( ferror(f) ) {
    fclose(f);
    throw Exception("Failure while reading the file");
  }
  fclose(f);

  digest_size=EVP_MD_CTX_size(&ctx);
  digest = new unsigned char[digest_size];

  if ( ! EVP_DigestFinal(&ctx, digest, NULL) ) {
    delete digest;
    digest = NULL;
    throw Exception("Could not finalize digest");
  }
}


/** Destructor. */
InterfaceDigest::~InterfaceDigest()
{
  delete digest;
}


/** Get hash.
 * @return hash
 */
const unsigned char *
InterfaceDigest::get_hash()
{
  return digest;
}


/** Get hash size.
 * @return hash size
 */
size_t
InterfaceDigest::get_hash_size()
{
  return digest_size;
}
