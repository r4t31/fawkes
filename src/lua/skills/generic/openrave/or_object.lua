
----------------------------------------------------------------------------
--  or_object.lua - object handling in openrave
--
--  Created: Thu Mar 03 14:32:47 2011
--  Copyright  2011  Bahram Maleki-Fard
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

-- Initialize module
module(..., skillenv.module_init)

-- Crucial skill information
name               = "or_object"
fsm                = SkillHSM:new{name=name, start="INIT"}
depends_skills     = {}
depends_interfaces = {
   {v = "if_openrave", type = "OpenRAVEInterface", id="OpenRAVE"}
}

documentation      = [==[OpenRAVE objerct handling skill.

This skill provides object manipulation for objects in OpenRAVE.
It basically allows all the object manipulation posiibilities provided in
*plugins/openrave/environment.h" (i.e. messages from OpenRAVEInterface)
to be called from the skill level.


Arguments needed for every call:
 NAME: name of the object (string)

Possible call modes:

or_object{add=true,  name=NAME, path=PATH}
 Adds an object to the environment.
 NAME: name to be given to object (string)
 PATH: path to xml file defining the object (string)

or_object{delete=true,  name=NAME}
 Deletes object from environment

or_object{attach=true,  name=NAME}
 Attaches object to the currently active robot

or_object{release=true,  name=NAME}
 Releases object from the currently active robot

or_object{release_all=true}
 Releases all object from the currently active robot

or_object{move=true,  name=NAME, x=X, y=Y, z=Z}
 Moves object to a global position in the environment. Need to consider, that
 coordinates define the center-point of the object.
 X: x-coordinate (float)
 Y: y-coordinate (float)
 Z: z-coordinate (float)

or_object{rotate=true,  name=NAME, x=X, y=Y, z=Z}
 Rotates object around its center point. Rotation values are absolute, not relative
 to current rotation.
 X: 1st rotation,  around x-axis (float)
 Y: 2nd rotation,  around y-axis (float)
 Z: 3rd rotation,  around z-axis (float)

or_object{rename=true,  name=NAME, new_name=NEW_NAME}
 Renames an object.
 NEW_NAME: new name of object (string)
]==]

-- Constants
local DEFAULT_ORI = 0.0
local DEFAULT_MARGIN = 0.2

-- Initialize as skill module
skillenv.skill_module(...)

-- Check if message handling is final
function jc_msg_final(state)
   return state.fsm.vars.msgid == if_openrave:msgid() and
          if_openrave:is_final()
end

fsm:add_transitions {
   closure={p=p, if_openrave=if_openrave},
   {"INIT", "RELEASE_ALL", "vars.release_all", precond=true, desc="release all"}, -- put here, because do not need name for it
   {"INIT", "FAILED", "not vars.name", precond=true, desc="no object name given"},

   {"INIT", "ADD", "vars.add", precond=true, desc="add"},
   {"INIT", "DELETE", "vars.delete", precond=true, desc="delete"},
   {"INIT", "ATTACH", "vars.attach", precond=true, desc="attach"},
   {"INIT", "RELEASE", "vars.release", precond=true, desc="release"},
   {"INIT", "MOVE", "vars.move", precond=true, desc="move"},
   {"INIT", "ROTATE", "vars.rotate", precond=true, desc="rotate"},
   {"INIT", "RENAME", "vars.rename", precond=true, desc="rename"},

   {"ADD", "FAILED", "not (vars.path)", precond=true, desc="insufficient arguments"},
   {"MOVE", "FAILED", "not (vars.x and vars.y and vars.z)", precond=true, desc="insufficient arguments"},
   {"ROTATE", "FAILED", "not (vars.x and vars.y and vars.z)", precond=true, desc="insufficient arguments"},
   {"RENAME", "FAILED", "not (vars.new_name)", precond=true, desc="insufficient arguments"},

   {"ADD", "CHECK", jc_msg_final, desc="final"},
   {"DELETE", "CHECK", jc_msg_final, desc="final"},
   {"ATTACH", "CHECK", jc_msg_final, desc="final"},
   {"RELEASE", "CHECK", jc_msg_final, desc="final"},
   {"RELEASE_ALL", "CHECK", jc_msg_final, desc="final"},
   {"MOVE", "CHECK", jc_msg_final, desc="final"},
   {"ROTATE", "CHECK", jc_msg_final, desc="final"},
   {"RENAME", "CHECK", jc_msg_final, desc="final"},

   {"CHECK", "FINAL", "if_openrave:is_success()", desc="command succeeded"},
   {"CHECK", "FAILED", true, desc="command failed"}
}

function ADD:init()
   self.fsm.vars.msgid = if_openrave:msgq_enqueue_copy(if_openrave.AddObjectMessage:new( self.fsm.vars.name,
                                                                                         self.fsm.vars.path ))
end

function DELETE:init()
   self.fsm.vars.msgid = if_openrave:msgq_enqueue_copy(if_openrave.DeleteObjectMessage:new( self.fsm.vars.name ))
end

function ATTACH:init()
   self.fsm.vars.msgid = if_openrave:msgq_enqueue_copy(if_openrave.AttachObjectMessage:new( self.fsm.vars.name ))
end

function RELEASE:init()
   self.fsm.vars.msgid = if_openrave:msgq_enqueue_copy(if_openrave.ReleaseObjectMessage:new( self.fsm.vars.name ))
end

function RELEASE_ALL:init()
   self.fsm.vars.msgid = if_openrave:msgq_enqueue_copy(if_openrave.ReleaseAllObjectsMessage:new())
end

function MOVE:init()
   self.fsm.vars.msgid = if_openrave:msgq_enqueue_copy(if_openrave.MoveObjectMessage:new( self.fsm.vars.name,
                                                                                          self.fsm.vars.x,
                                                                                          self.fsm.vars.y,
                                                                                          self.fsm.vars.z ))
end

function ROTATE:init()
   self.fsm.vars.msgid = if_openrave:msgq_enqueue_copy(if_openrave.RotateObjectMessage:new( self.fsm.vars.name,
                                                                                            self.fsm.vars.x,
                                                                                            self.fsm.vars.y,
                                                                                            self.fsm.vars.z ))
end

function RENAME:init()
   self.fsm.vars.msgid = if_openrave:msgq_enqueue_copy(if_openrave.RenameObjectMessage:new( self.fsm.vars.name,
                                                                                            self.fsm.vars.new_name ))
end

