
/***************************************************************************
 *  connector.cpp - Fawkes to OpenRAVE Connector
 *
 *  Created: Thu Sep 16 14:50:34 2010
 *  Copyright  2010  Bahram Maleki-Fard, AllemaniACs RoboCup Team
 *
 ****************************************************************************/

/*  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  Read the full text in the LICENSE.GPL file in the doc directory.
 */

#include "connector.h"
#include "environment.h"
#include "robot.h"
#include "manipulator.h"

#include <openrave-core.h>
#include <utils/logging/logger.h>

namespace fawkes {
#if 0 /* just to make Emacs auto-indent happy */
}
#endif

/** Constructor */
OpenRAVEConnector::OpenRAVEConnector(fawkes::Logger* logger) :
  __logger( logger ),
  __env( 0 ),
  __robot( 0 ),
  __traj( 0 )
{
}

/** Destructor */
OpenRAVEConnector::~OpenRAVEConnector()
{
  __env->destroy();
}

/** Setup stuff .
 * MUST be called before starting to work with OR stuff*/
bool
OpenRAVEConnector::setup(const std::string& filenameRobot)
{
  try {
    OpenRAVE::RaveInitialize(true);
  } catch(const OpenRAVE::openrave_exception &e) {
    if(__logger)
      __logger->log_error("OpenRAVE Connector", "Could not initialize OpenRAVE. Ex:%s", e.what());
    throw;
  }

  __env   = new OpenRAVEEnvironment(__logger);
  __robot = new OpenRAVERobot(__logger);

  __env->create();
  __env->enableDebug(); // TODO: cfg
  if( !__robot->load(filenameRobot, __env) )    {return 0;}
  if( !__env->addRobot(__robot) )               {return 0;}
  if( !__robot->setReady() )                    {return 0;}

  __env->lock();

  return 1;
}

/** Start Viewer */
void
OpenRAVEConnector::startViewer() const
{
  __env->startViewer();
}


/** Set OpenRAVEManipulator object for robot */
void
OpenRAVEConnector::setManipulator(OpenRAVEManipulator* manip)
{
  __robot->setManipulator(manip);
}

/** Set target angles;
 * Angles from OR model. JUST FOR TESTING! */
void
OpenRAVEConnector::setTarget(std::vector<float>& angles)
{
  __robot->setTargetAngles(angles);
}

/** Set target, given transition, and rotation as quaternion.
 * @return true if solvable, false otherwise
 */
bool
OpenRAVEConnector::setTargetQuat(float& transX, float& transY, float& transZ, float& quatW, float& quatX, float& quatY, float& quatZ)
{
  return __robot->setTargetQuat(transX, transY, transZ, quatW, quatX, quatY, quatZ);
}

/** Set target, given transition, and rotation as axis-angle.
 * @return true if solvable, false otherwise
 */
bool
OpenRAVEConnector::setTargetAxisAngle(float& transX, float& transY, float& transZ, float& angle, float& axisX, float& axisY, float& axisZ)
{
  return __robot->setTargetQuat(transX, transY, transZ, angle, axisX, axisY, axisZ);
}

/** Run planner on previously set target.
 * @return false if some error occured during planning, true otherwise
 */
bool
OpenRAVEConnector::runPlanner()
{
  return __env->runPlanner(__robot);
}

/** Get trajectory from planned path.
 * Angles are converted to angles of real device.
 * @return trajectory vector
 */
std::vector< std::vector<float> >*
OpenRAVEConnector::getTrajectory() const
{
  return __robot->getTrajectoryDevice();
}

/** Get pointer to OpenRAVEEnvironment object.
 * @return pointer
 */
OpenRAVEEnvironment*
OpenRAVEConnector::getEnvironment() const
{
  return __env;
}

/** Get pointer to OpenRAVERobot object.
 * @return pointer
 */
OpenRAVERobot*
OpenRAVEConnector::getRobot() const
{
  return __robot;
}
//  boost::shared_ptr<Trajectory> traj(RaveCreateTrajectory(__env,__robot->GetActiveDOF()));

} // end of namespace fawkes