/***************************************************************************
 *  string_urlescape.h - Fawkes string URL escape utils
 *
 *  Created: Fri Oct 24 09:31:39 2008
 *  Copyright  2006-2007  Tim Niemueller [www.niemueller.de]
 *
 ****************************************************************************/

/*  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version. A runtime exception applies to
 *  this software (see LICENSE.GPL_WRE file mentioned below for details).
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  Read the full text in the LICENSE.GPL_WRE file in the doc directory.
 */

#ifndef _UTILS_MISC_STRING_URLESCAPE_H_
#define _UTILS_MISC_STRING_URLESCAPE_H_

namespace fawkes {

/** Transform hex to value.
 * @param c character
 * @return value of hex code as number
 */
int
unhex(char c)
{
	return (c >= '0' && c <= '9' ? c - '0' : c >= 'A' && c <= 'F' ? c - 'A' + 10 : c - 'a' + 10);
}

/** Remove URL hex escapes from s in place.
 * @param s string to manipulate
 */
void
hex_unescape(char *s)
{
	char *p;

	for (p = s; *s != '\0'; ++s) {
		if (*s == '%') {
			if (*++s != '\0') {
				*p = unhex(*s) << 4;
			}
			if (*++s != '\0') {
				*p++ += unhex(*s);
			}
		} else {
			*p++ = *s;
		}
	}

	*p = '\0';
}

} // end namespace fawkes

#endif
