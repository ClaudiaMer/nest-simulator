/*
 *  simulation_manager.h
 *
 *  This file is part of NEST.
 *
 *  Copyright (C) 2004 The NEST Initiative
 *
 *  NEST is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  NEST is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with NEST.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef SIMULATION_MANAGER_H
#define SIMULATION_MANAGER_H

// C++ includes:
#include <vector>

// Includes from libnestutil:
#include "manager_interface.h"

// Includes from nestkernel:
#include "nest_time.h"
#include "nest_types.h"

// Includes from sli:
#include "dictdatum.h"

namespace nest
{
class Node;

class SimulationManager : public ManagerInterface
{
public:
  SimulationManager();

  virtual void initialize();
  virtual void finalize();

  virtual void set_status( const DictionaryDatum& );
  virtual void get_status( DictionaryDatum& );

  /**
   * Simulate for the given time .
   * This function performs the following steps
   * 1. set the new simulation time
   * 2. call prepare_simulation()
   * 3. call resume()
   * 4. call finalize_simulation()
   */
  void simulate( Time const& );

  /**
   * Terminate the simulation after the time-slice is finished.
   */
  void terminate();

  /**
   *
   */
  size_t get_prelim_interpolation_order() const;

  /**
   *
   */
  double_t get_prelim_tol() const;

  /**
   * Get the time at the beginning of the current time slice.
   */
  Time const& get_slice_origin() const;

  /**
   * Get the time at the beginning of the previous time slice.
   */
  Time const get_previous_slice_origin() const;

  /**
   * Precise time of simulation.
   * @note The precise time of the simulation is defined only
   *       while the simulation is not in progress.
   */
  Time const get_time() const;

  /**
   * Return true, if the SimulationManager has already been simulated for some time.
   * This does NOT indicate that simulate has been called (i.e. if Simulate
   * is called with 0 as argument, the flag is still set to false.)
   */
  bool has_been_simulated() const;

  /**
   * Reset the SimulationManager to the state at T = 0.
   */
  void reset_network();

  /**
   * Get slice number. Increased by one for each slice. Can be used
   * to choose alternating buffers.
   */
  size_t get_slice() const;

  //! Return current simulation time.
  // TODO: Precisely how defined? Rename!
  Time const& get_clock() const;

  //! Return start of current time slice, in steps.
  // TODO: rename / precisely how defined?
  delay get_from_step() const;

  //! Return end of current time slice, in steps.
  // TODO: rename / precisely how defined?
  delay get_to_step() const;

private:
  void resume_( const size_t );       //!< actually run simulation; TODO: review
  const size_t prepare_simulation_(); //! setup before simulation start
  void finalize_simulation_();        //! wrap-up after simulation end
  void update_();                     //! actually perform simulation
  bool prelim_update_( Node* );
  void advance_time_();   //!< Update time to next time step
  void print_progress_(); //!< TODO: Remove, replace by logging!

  bool simulating_;                   //!< true if simulation in progress
  Time clock_;                        //!< SimulationManager clock, updated once per slice
  delay slice_;                       //!< current update slice
  delay to_do_;                       //!< number of pending cycles.
  delay to_do_total_;                 //!< number of requested cycles in current simulation.
  delay from_step_;                   //!< update clock_+from_step<=T<clock_+to_step_
  delay to_step_;                     //!< update clock_+from_step<=T<clock_+to_step_
  timeval t_slice_begin_;             //!< Wall-clock time at the begin of a time slice
  timeval t_slice_end_;               //!< Wall-clock time at the end of time slice
  long t_real_;                       //!< Accumunated wall-clock time spent simulating (in us)
  bool terminate_;                    //!< Terminate on signal or error
  bool simulated_;                    //!< indicates whether the SimulationManager has already been
                                      //!< simulated for sometime
  bool print_time_;                   //!< Indicates whether time should be printed during
                                      //!< simulations (or not)
  long max_num_prelim_iterations_;    //!< maximal number of iterations used for preliminary update
  size_t prelim_interpolation_order_; //!< interpolation order for prelim iterations
  double_t prelim_tol_;               //!< Tolerance of prelim iterations
};

inline void
SimulationManager::terminate()
{
  terminate_ = true;
}

inline Time const&
SimulationManager::get_slice_origin() const
{
  return clock_;
}

inline Time const
SimulationManager::get_time() const
{
  assert( not simulating_ );
  return clock_ + Time::step( from_step_ );
}

inline bool
SimulationManager::has_been_simulated() const
{
  return simulated_;
}

inline size_t
SimulationManager::get_slice() const
{
  return slice_;
}

inline Time const&
SimulationManager::get_clock() const
{
  return clock_;
}

inline delay
SimulationManager::get_from_step() const
{
  return from_step_;
}

inline delay
SimulationManager::get_to_step() const
{
  return to_step_;
}

inline size_t
SimulationManager::get_prelim_interpolation_order() const
{
  return prelim_interpolation_order_;
}

inline double_t
SimulationManager::get_prelim_tol() const
{
  return prelim_tol_;
}
}


#endif /* SIMULATION_MANAGER_H */
