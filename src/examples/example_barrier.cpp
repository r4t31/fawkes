
/***************************************************************************
 *  example_barrier.cpp - barrier example program
 *
 *  Generated: Thu Sep 15 14:48:11 2006
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
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02111-1307, USA.
 */

// Do not mention in API doc
/// @cond EXAMPLES

#include <core/threading/thread.h>
#include <core/threading/barrier.h>

#include <iostream>
#include <string>

using namespace std;

class ExampleBarrierThread : public Thread
{
 public:
  ExampleBarrierThread(string pp,
		       Barrier *barrier, unsigned int sleep_time)
  {
    this->pp         = pp;
    this->barrier    = barrier;
    this->sleep_time = sleep_time;
  }

  virtual void run()
  {
    forever {
      usleep( sleep_time );
      cout << pp << ": Waiting for barrier" << endl;
      barrier->wait();
      cout << pp << ": Barrier lifted" << endl;
    }
  }

 private:
  Barrier *barrier;
  unsigned int sleep_time;
  string pp;

};


int
main(int argc, char **argv)
{
  Barrier *b = new Barrier(3);

  ExampleBarrierThread *t1 = new ExampleBarrierThread("t1", b, 3424345);
  ExampleBarrierThread *t2 = new ExampleBarrierThread("t2", b, 326545);
  ExampleBarrierThread *t3 = new ExampleBarrierThread("t3", b, 6458642);

  t1->start();
  t2->start();
  t3->start();

  t1->join();
  t2->join();
  t3->join();

  delete t1;
  delete t2;
  delete t3;

  delete b;
}


/// @endcond
