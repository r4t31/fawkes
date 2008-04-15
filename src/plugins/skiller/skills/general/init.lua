
----------------------------------------------------------------------------
--  init.lua - skiller Lua initialization code
--
--  Created: Mon Mar 10 17:00:35 2008
--  Copyright  2008  Tim Niemueller [www.niemueller.de]
--
--  $Id$
--
----------------------------------------------------------------------------

--  This program is free software; you can redistribute it and/or modify
--  it under the terms of the GNU General Public License as published by
--  the Free Software Foundation; either version 2 of the License, or
--  (at your option) any later version.
--
--  This program is distributed in the hope that it will be useful,
--  but WITHOUT ANY WARRANTY; without even the implied warranty of
--  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
--  GNU Library General Public License for more details.
--
--  Read the full text in the LICENSE.GPL file in the doc directory.


-- SKILLDIR is set by skiller before loading init.lua
package.path  = package.path .. ";" .. SKILLDIR .. "/?.lua";
package.cpath = package.cpath .. ";" .. LIBDIR .. "/lua/?.so";

-- Include C/C++ compat packages
require("utils");
require("config");
require("interface");
require("interfaces");

-- Base package extensions
require("general.stringext");
