#include "maps_ITSPlanner.h"	// Includes the header of this component


MAPS_BEGIN_INPUTS_DEFINITION(MAPSITSPlanner)

  /// - it is encouraged that this input be run through the maps_PathPreprocessor 
  ///   block first. (or before DLCManager)
  /// - format:
  ///   [x1,y1,v1,x2,y2,v2,...,xn,yn,vn]
  ///   v could really be anything, but as of v0.1, only 0/-1 is supported.
  ///     -1: unspecified speed
  ///     0: stop sign
  /// the path that the vehicle will follow, including stop signs
  MAPS_INPUT("master_path",  MAPS::FilterFloat64, MAPS::FifoReader)

  /// The kinematic state of the vehicle (at the rear axle)
  /// [x,y,theta,vBx,aBx]
  /// x: x-position in HRI linear coordinates (m)
  /// y: y-position in HRI linear coordinates (m)
  /// theta: heading in ISO convention (rad)
  /// vBx: forward speed of the vehicle (body x axis) (m/s)
  /// aBx: forward acceleration of the vehicle (body x axis) (m/s^2)
  MAPS_INPUT("state",        MAPS::FilterFloat64, MAPS::LastOrNextReader)

  /// pedestrians on the roadway
  /// - this can come from either the hsun pedestrian detection block, or from the 
  ///   pedestrian detection simulator block in this same package.
  /// - The only reliable piece of information we have available is position, so that's
  ///   the only piece of information from the RealObject that we use.
  /// - will stop for these pedestrians at the appropriate juncture, unless they 
  ///   disappear first
  /// - an empty vector of pedestrians indicates that none are detected. It is
  ///   critical that an empty real object vector be published here when there
  ///   are no pedestrians.
  MAPS_INPUT("rstop_peds", MAPS::FilterRealObjects, MAPS::LastOrNextReader)

MAPS_END_INPUTS_DEFINITION


MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSITSPlanner)

  /// This is the 1D trajectory simulated at time step specified by "sim_time_step"
  /// it is compressed into a double vector.
  /// There are 5 fields: t, s, v, a, j. Each field has the same number of points
  /// The format is as follows:
  ///   t1, s1, v1, a1, j1, t2, s2, v2, a2, j2, ..., tn, sn, vn, an, jn, 
  MAPS_OUTPUT("ref_cmp",          MAPS::VectorFlag|MAPS::Float64, NULL, NULL, 1e6) // variable

  /// This is the set of 1D control points (jerk phases) for the whole profile (subpath)
  /// It follows the same format as "ref_cmp", but it only records points where the constant jer
  /// value changes.
  MAPS_OUTPUT("cp_cmp",           MAPS::VectorFlag|MAPS::Float64, NULL, NULL, 1e3) // variable

  /// The output meant to go to the Bosch controller wrapper
  /// it is compressed into a double vector.
  /// There are 7 fields: x (path frame), y (path frame), theta, kappa, vBx, aBx, jBx
  /// the format is as follows
  ///   x1, y1, theta1, kappa1, vBx1, aBx1, jBx1, x2, y2, theta2, kappa2, vBx2, aBx2, jBx2, ... , xn, yn, thetan, kappan, vBxn, aBxn, jBxn
  /// note that the heading internal to the planner/manager does not follow the ISO
  /// convention; it's all messed up in a couple places where it's calculated. So
  /// This RTMaps wrapper fixes it on output.
  MAPS_OUTPUT("slice_cmp",        MAPS::VectorFlag|MAPS::Float64, NULL, NULL, 0) // set once at runtime

  /// the 1D positions along the current subpath corresponding to the slice points
  MAPS_OUTPUT("slice_s",          MAPS::VectorFlag|MAPS::Float64, NULL, NULL, 0) // set once at runtime
                                                                                 // 
  
  /// the current subpath
  /// output every time the current subpath changes (including RSTOP and RSTOP replanning)
  /// it does not include speed points:
  ///     [x1,y1,x2,y2,...,xn,yn]
  MAPS_OUTPUT("path_plan",        MAPS::VectorFlag|MAPS::Float64, NULL, NULL, 1e6) // variable 

  /// upcoming curvature ahead of the current position, regardless of PSTOPs
  /// use "future_turn_window_distance" and "future_turn_window_points" to control
  /// this window.
  /// When at or near the end of the master path, the points used to calculate these
  /// values will be projected along the very last path link past the end of the master
  /// path, so the corresponding curvature values will be at or near zero.
  /// Correspondingly, when the current position is before the beginning of the master
  /// path, the window will begin at the first point of the master path.
  /// This functions in a vaguely similar way to the slice outputting, but with fewer
  /// caveates
  /// curvature is in 1/meters.
  /// points will be evenly spaced according to "future_turn_window_distance" / "future_turn_window_points"
  MAPS_OUTPUT("future_curvature", MAPS::VectorFlag|MAPS::Float64, NULL, NULL, 1e3)

  /// upcoming heading ahead of the current position, regardless of PSTOPs
  /// use "future_turn_window_distance" and "future_turn_window_points" to control
  /// this window.
  /// When at or near the end of the master path, the points used to calculate these
  /// values will be projected along the very last path link past the end of the master
  /// path, so the corresponding curvature values will be at or near zero.
  /// Correspondingly, when the current position is before the beginning of the master
  /// path, the window will begin at the first point of the master path.
  /// This functions in a vaguely similar way to the slice outputting, but with fewer
  /// caveates
  /// heading is in radians
  /// points will be evenly spaced according to "future_turn_window_distance" / "future_turn_window_points"
  /// !!! NOTE That the corrections done to the heading that comes out of the slices
  /// is not done here. It does NOT follow the ISO convention, whereas the slice 
  /// headings do. This is meant to be used to determine whether to engage turn signals.
  /// - The sign will be "wrong" and the heading will be off by pi/2
  MAPS_OUTPUT("future_heading",   MAPS::VectorFlag|MAPS::Float64, NULL, NULL, 1e3)

MAPS_END_OUTPUTS_DEFINITION


MAPS_BEGIN_PROPERTIES_DEFINITION(MAPSITSPlanner)

  /////////////// PathManager

  /// Whether the log messages will be printed to the RTMaps console. If this is
  /// done and RTMaps crashes, the logs are lost forever. setting this to false
  /// will redirect log messages to the terminal in which RTMaps was launched.
  /// It is recommended that the `html-record` tool included inthe its-planner repo
  /// be used to record log messages for viewing/searching in a web browser.
  MAPS_PROPERTY("log_to_rtmaps", false, false, false)

  /// Whether messages of level debug are logged at all, there will be considerable 
  /// amount of logs if this option is checked.
  MAPS_PROPERTY("log_debug", true, false, false)

  /// How close the vehicle must be to a PSTOP point when the vehicle stops to be
  /// considered to have honored the stop sign. If the vehicle comes to a stop
  /// outside this radius, it will more or less get stuck and need to be "bumped"
  /// outside of AD mode. A value of 1.0 meter is often too small.
  MAPS_PROPERTY("stop_pos_tol", 2.5, false, true)

  /// max speed, in meters/second, to be considered "stopped"
  MAPS_PROPERTY("stop_speed_tol", 0.1, false, true)

  /// max acceleration, in meters/second/second, to be considered "stopped"
  MAPS_PROPERTY("stop_accel_tol", 0.05, false, false)

  /// How long to stay stopped at a PSTOP point before continuing on to the next 
  /// subpath
  MAPS_PROPERTY("pstop_duration", 2.0, false, false)

  /// This is important. How much space to plan to leave between the front bumper and the
  /// closest pedestrian after RSTOPping
  MAPS_PROPERTY("rstop_buf_dist", 8.5, false, false)

  /// AFter an RSTOP is planned, how far the pedestrian must move toward or away from
  /// the vehicle to trigger replanning
  MAPS_PROPERTY("rstop_replan_pos_tol", 1.0, false, false)

  /// How long to wait after all pedestrians in range are gone before exiting a 
  /// planned RSTOP and resuming NORMAL mode
  MAPS_PROPERTY("rstop_resume_wait_time", 0.5, false, false)

  /// How much to add to "rstop_buf_dist" to check for pedestrians when going from
  /// either RSTOP or PSTOP back to NORMAL. This value keeps variation in the dynamic
  /// required stopping distance during braking from toggling back and forth from 
  /// RSTOP and NORMAL
  MAPS_PROPERTY("stop_resume_pos_buf", 4.0, false, false)

  /// the distance from the rear axle (origin of the vehicle frame and what GPS measurements
  /// correspond to) and the front bumper
  MAPS_PROPERTY("car_length", 4.5, false, false)

  /// How far ahead of the current position (rear axle) to sample path points
  /// for upcoming turn detection output
  MAPS_PROPERTY("future_turn_window_distance", 10.0, false, true)

  /// How many points to include in the upcoming curvature/heading window 
  MAPS_PROPERTY("future_turn_window_points", 10, false, true)

  ////////////// TrajectoryPlanner
  
  /// How many points to include in the slice. This value and that in the Bosch
  /// controller wrapper need to be the same, or you will have huge problems
  MAPS_PROPERTY("slice_length", 25, false, false)

  /// How far apart in future reference time the slice points need to be. This is
  /// a hard value for the Bosch controller.
  MAPS_PROPERTY("slice_time_step", 0.1, false, false)

  /// delta position threshold for equivalence
  MAPS_PROPERTY("planner_pos_tol", 1e-2, false, false)

  /// delta speed threshold for equivalence
  MAPS_PROPERTY("planner_speed_tol", 1e-2, false, false)

  /// delta acceleration threshold for equivalence
  MAPS_PROPERTY("planner_accel_tol", 1e-3, false, false)

  /// the peak stopping (negative) acceleration to try to get every time the car slows down,
  /// in g's. When this value is not feasible it will be modified within the limites
  /// specified in the corresponding "bound" fields
  MAPS_PROPERTY("accel_peak_lower_g", -0.25, false, false)

  /// the peak position acceleration to try to get to every time the car speed up,
  /// in g's. When this value is not feasible it will be modified within the limites
  /// specified in the corresponding "bound" fields
  MAPS_PROPERTY("accel_peak_upper_g", 0.2, false, false)  

  /// the stopping jerk (negative) to try to use every time the car speeds up, in 
  /// g/s. When this value is not feasible it will be modified within the limites
  /// specified in the corresponding "bound" fields
  MAPS_PROPERTY("jerk_peak_lower_g", -0.1, false, false)

  /// the positive jerk to try to use every time the car speeds up, in 
  /// g/s. When this value is not feasible it will be modified within the limites
  /// specified in the corresponding "bound" fields
  MAPS_PROPERTY("jerk_peak_upper_g", 0.05, false, false)

  /// the minimum acceptable plan speed. This should be just below zero, to accomodate
  /// numerical errors.
  MAPS_PROPERTY("speed_bound_lower", -1e-2, false, false)

  /// IMPORTANT. This is the speed limit for the whole course, in m/s
  MAPS_PROPERTY("speed_bound_upper", 25.0*0.44704, false, false)

  /// The lower boundary on planned acceleration, in g
  MAPS_PROPERTY("long_accel_bound_lower_g", -0.3, false, false)

  /// The upper boundary on planned acceleration, in g
  MAPS_PROPERTY("long_accel_bound_upper_g", 0.25, false, false)

  /// The upper boundary on planned acceleration, in g. This directly controls
  /// how fast the car goes through turns, using the curvature value of each.
  MAPS_PROPERTY("lat_accel_bound_g", 0.1, false, false)

  /// The lower boundary on planned jerk, in g/s
  MAPS_PROPERTY("long_jerk_bound_lower_g", -0.35, false, false)

  /// The upper boundary on planned jerk, in g/s
  MAPS_PROPERTY("long_jerk_bound_upper_g", 0.2, false, false)

  /// When creating a reverse lookup spline for t(s), the piecewise jerk must be
  /// integrated and simulated. This is the simulation time step. It controls the
  /// points output in "ref_cmp"
  MAPS_PROPERTY("sim_time_step", 0.1, false, false)
MAPS_END_PROPERTIES_DEFINITION


MAPS_BEGIN_ACTIONS_DEFINITION(MAPSITSPlanner)
MAPS_END_ACTIONS_DEFINITION


MAPS_COMPONENT_DEFINITION( MAPSITSPlanner,"ITSPlanner", "1.0", 128, MAPS::Threaded,MAPS::Threaded, -1, -1, -1, -1 )


void MAPSITSPlanner::Birth()
{
  manager = new its::PathManager();

  //Initialize the _inputs member array.
  m_inputs[0] = &Input("master_path");
  m_inputs[1] = &Input("state");
  m_inputs[2] = &Input("rstop_peds");

  // Logging for INFO, WARN, ERROR
  if (GetBoolProperty("log_to_rtmaps")) {
    manager->setLogInfoCallback( std::bind(&MAPSITSPlanner::logInfoCallback,  this, std::placeholders::_1, std::placeholders::_2));
    manager->setLogWarnCallback( std::bind(&MAPSITSPlanner::logWarnCallback,  this, std::placeholders::_1, std::placeholders::_2));
    manager->setLogErrorCallback(std::bind(&MAPSITSPlanner::logErrorCallback, this, std::placeholders::_1, std::placeholders::_2));
    manager->planner->setLogInfoCallback( std::bind(&MAPSITSPlanner::logInfoCallback,  this, std::placeholders::_1, std::placeholders::_2));
    manager->planner->setLogWarnCallback( std::bind(&MAPSITSPlanner::logWarnCallback,  this, std::placeholders::_1, std::placeholders::_2));
    manager->planner->setLogErrorCallback(std::bind(&MAPSITSPlanner::logErrorCallback, this, std::placeholders::_1, std::placeholders::_2));
  } // will default to terminal output otherwise
  // logging for DEBUG
  if (GetBoolProperty("log_debug")) {
    if (GetBoolProperty("log_to_rtmaps")) {
      manager->setLogDebugCallback(std::bind(&MAPSITSPlanner::logDebugCallback, this, std::placeholders::_1, std::placeholders::_2));
      manager->planner->setLogDebugCallback(std::bind(&MAPSITSPlanner::logDebugCallback, this, std::placeholders::_1, std::placeholders::_2));
    }
  } else { // send the debug messages nowhere
    manager->setLogDebugCallback(std::bind(its::defaultLogVoidCallback, std::placeholders::_1, std::placeholders::_2));
    manager->planner->setLogDebugCallback(std::bind(its::defaultLogVoidCallback, std::placeholders::_1, std::placeholders::_2));
  }

  manager->setStopPositionTolerance(GetFloatProperty("stop_pos_tol"));
  manager->setStopSpeedTolerance(GetFloatProperty("stop_speed_tol"));
  manager->setStopAccelerationTolerance(GetFloatProperty("stop_accel_tol"));
  manager->setPStopDuration(GetFloatProperty("pstop_duration"));
  manager->setRStopBufferDistance(GetFloatProperty("rstop_buf_dist"));
  manager->setReplanPositionTolerance(GetFloatProperty("rstop_replan_pos_tol"));
  manager->setResumeWaitTime(GetFloatProperty("rstop_resume_wait_time"));
  manager->setResumePositionBuffer(GetFloatProperty("stop_resume_pos_buf"));
  manager->setCarLength(GetFloatProperty("car_length"));
  
  manager->planner->setPositionTolerance(GetFloatProperty("planner_pos_tol"));
  manager->planner->setSpeedTolerance(GetFloatProperty("planner_speed_tol"));
  manager->planner->setAccelerationTolerance(GetFloatProperty("planner_accel_tol"));
  manager->planner->setSpeedBounds(GetFloatProperty("speed_bound_lower"), GetFloatProperty("speed_bound_upper"));
  manager->planner->setLongAccelBounds(GetFloatProperty("long_accel_bound_lower_g"), GetFloatProperty("long_accel_bound_upper_g"));
  manager->planner->setLatAccelBounds(GetFloatProperty("lat_accel_bound_g"));
  manager->planner->setJerkBounds(GetFloatProperty("long_jerk_bound_lower_g"), GetFloatProperty("long_jerk_bound_upper_g"));
  manager->planner->setLongAccelPeaks(GetFloatProperty("accel_peak_lower_g"), GetFloatProperty("accel_peak_upper_g"));
  manager->planner->setLongJerkPeaks(GetFloatProperty("jerk_peak_lower_g"), GetFloatProperty("jerk_peak_upper_g"));
  manager->planner->setSimTimeStep(GetFloatProperty("sim_time_step"));

  manager->planner->newPlanCallback = std::bind(&MAPSITSPlanner::outputPlan, this);

  slice_length = (uint) GetIntegerProperty("slice_length");
  slice_time_step = (double) GetFloatProperty("slice_time_step");

  // allocate the fixed size of the slices
  Output("slice_cmp").AllocOutputBuffer(slice_length * (its::Trajectory2D_N_FIELDS-1));
  Output("slice_s").AllocOutputBuffer(slice_length);
}


void MAPSITSPlanner::Core() 
{
  // Minimal stuff happening here, so that RTMaps is in control of as little as 
  // possible.
  int input_that_answered;
  MAPSIOElt * io_elt_in = StartReading(3, m_inputs, &input_that_answered);
  if (io_elt_in == NULL) return;

  // generically unpack the data
  MAPSTimestamp t = io_elt_in->Timestamp();
  MAPSInt32 sz = io_elt_in->VectorSize();
  std::vector<double> inp_double((int)sz);
  std::vector<MAPSRealObject> inp_real_object;
  
  switch(input_that_answered) {
    case 0: // master path
      for (int k = 0; k < sz; k++) {
        inp_double[k] = io_elt_in->Float64(k);
      }
      handleInputPath(inp_double, t);
      break;
    case 1: // state
      for (int k = 0; k < sz; k++) {
        inp_double[k] = io_elt_in->Float64(k);
      }
      handleInputState(inp_double, t);
      outputSlice();
      break;
    case 2: // pedestrian locations
      for (int k = 0; k < sz; k++) {
        inp_real_object.push_back(io_elt_in->RealObject(k));
      }
      handleInputPedestrians(inp_real_object, t);
      outputSlice();
      break;
    default:
      Error("Unknown input.");
      return;
  }
}


void MAPSITSPlanner::Death()
{
  delete manager;
}


void MAPSITSPlanner::handleInputPath(std::vector<double> raw_data, MAPSTimestamp stamp)
{
  // std::cout << "handleInputPath\n";
  // check data size
  if (raw_data.size() % INPUT_MASTER_PATH_N_FIELDS != 0) {
    ReportWarning("Received path of the wrong size");
    return;
  }
  its::Path2D path;
  its::Point2D this_point;
  std::vector<double> speed;
  for (auto itdat = raw_data.begin(); itdat != raw_data.end(); itdat += INPUT_MASTER_PATH_N_FIELDS)
  {
    this_point << *(itdat), *(itdat+1);
    path.push_back(this_point);
    speed.push_back(*(itdat+2));
  }

  if (!manager->setPath(path, speed)) {
    ReportWarning("Problem setting path.");
    return;
  }

}


void MAPSITSPlanner::handleInputState(std::vector<double> raw_data, MAPSTimestamp stamp)
{
  if (raw_data.size() != 5) {
    ReportError("Received state of the wrong size");
    return;
  }
  state.rN << raw_data[0], raw_data[1];
  state.theta = raw_data[2];
  state.vBx = raw_data[3];
  state.aBx = raw_data[4];

  double stamp_sec = stamp * 1e-6;

  if (!manager->updateState(state, stamp_sec)) {
    ReportWarning("Problem updating state.");
    return;
  }

}


void MAPSITSPlanner::handleInputPedestrians(std::vector<MAPSRealObject> raw_data, MAPSTimestamp stamp)
{
  its::Path2D ped_locs; // not actually a path
  its::Point2D ped_loc;
  // uint id;
  for (auto obj : raw_data) {
    // id = raw_data[k];
    ped_loc << obj.x, obj.y;
    ped_locs.push_back(ped_loc);
  }    

  // LOG(its::defaultLogWarnCallback, "Pedestrian vector to process: " << ped_locs);
  if (!manager->updateObstacles(ped_locs)) {
    ReportError("Problem updating obstacles");
    return;
  }
}


void MAPSITSPlanner::outputSlice()
{
  MAPSTimestamp stamp = MAPS::CurrentTime();

  // get slice from planner
  its::Trajectory2D trj;

  //std::cout<<"state, x: "<<state.rN[0] << " y:  " << state.rN[1] << " theta: " <<state.theta<< " v: "<<state.vBx << " a: "<<state.aBx <<std::endl;
      
  if (!manager->planner->getTrajectorySlice(state, slice_length, slice_time_step, trj)) {
      //ReportInfo("data %3.2f ", slice_length);



    ReportError("Slice failed");
    return;
  }

  // write slice to output
  std::vector<double> out(trj.size()*(its::Trajectory2D_N_FIELDS-1));
  uint kout = 0;
  for (uint k = 0; k < trj.size(); k++) {
    out[kout+0]   = trj.t[k];
    out[kout+1] = trj.rN[k][0]; // x
    out[kout+2] = trj.rN[k][1]; // y
    out[kout+3] = (trj.theta[k]+(M_PI/2.0)) * 180.0/M_PI;
    if (std::isnan(trj.k[k])) {
      LOG(logErrorCallback, "NaN in Slice!!. Setting to zero");
      out[kout+4] = 0.0;
    } else if (std::isinf(trj.k[k])) {
      LOG(logErrorCallback, "Inf in Slice!!. Setting to zero.");
      out[kout+4] = 0.0;
    } else {
      out[kout+4] = trj.k[k];
    }
    out[kout+5] = trj.vBx[k];
    out[kout+6] = trj.aBx[k];
    out[kout+7] = trj.jBx[k];
    kout += (its::Trajectory2D_N_FIELDS-1);
  }
  MAPSIOElt * slice_io_out = StartWriting(Output("slice_cmp"));
  for (uint k = 0; k < out.size(); k++) {
    slice_io_out->Float64(k) = out[k];
  }
  slice_io_out->Timestamp() = stamp;
  // ReportInfo("MAPSTrajectoryPlanner sending slice");
  StopWriting(slice_io_out);
  // ReportInfo("MAPSTrajectoryPlanner done sending slice");

  // write s to a separate output
  MAPSIOElt * s_io_out = StartWriting(Output("slice_s"));
  for (uint k = 0; k < trj.s.size(); k++) {
    s_io_out->Float64(k) = trj.s[k];
  }
  s_io_out->Timestamp() = stamp;
  // ReportInfo("MAPSTrajectoryPlanner sending slice s");
  StopWriting(s_io_out);
  // ReportInfo("MAPSTrajectoryPlanner done sending slice s");

  // // output the future turn window infos
  std::vector<double> future_curvature, future_heading;
  int sz = GetIntegerProperty("future_turn_window_points");
  manager->getUpcomingCurvatureHeading(GetFloatProperty("future_turn_window_distance"), sz, future_curvature, future_heading);
  MAPSIOElt * curvature_io_out = StartWriting(Output("future_curvature"));
  MAPSIOElt * heading_io_out = StartWriting(Output("future_heading"));
  for (uint k = 0; k < sz; k++) {
    curvature_io_out->Float64(k) = future_curvature[k];
    heading_io_out->Float64(k) = future_heading[k];
  }
  curvature_io_out->Timestamp() = stamp;
  heading_io_out->Timestamp() = stamp;
  curvature_io_out->VectorSize() = sz;
  heading_io_out->VectorSize() = sz;
  StopWriting(curvature_io_out);
  StopWriting(heading_io_out);
}


void MAPSITSPlanner::outputPlan()
{
  // LOG(logDebugCallback, "in outputPlan callback.");
  MAPSTimestamp stamp = MAPS::CurrentTime();

  // output the resultant path
  its::Path2D path_plan = manager->planner->getPath();
  MAPSIOElt * path_plan_io_out = StartWriting(Output("path_plan"));
  path_plan_io_out->Timestamp() = stamp;
  uint k = 0;
  for (uint kp = 0; kp != path_plan.size(); kp++) {
    path_plan_io_out->Float64(k) =   path_plan[kp][0];
    path_plan_io_out->Float64(k+1) = path_plan[kp][1];
    k += 2;
  }
  path_plan_io_out->VectorSize() = path_plan.size()*2;
  StopWriting(path_plan_io_out);

  // send the control points out as well
  its::Trajectory1D cp = manager->planner->getControlPoints();
  cp.j.push_back(0); // make the lengths even
  // // dynamically allocate memory for this output
  // Output("cp_cmp").AllocOutputBuffer(cp.size()*its::Trajectory1D_N_FIELDS);
  MAPSIOElt * cp_io_out = StartWriting(Output("cp_cmp"));
  cp_io_out->Timestamp() = stamp;
  int kio = 0;
  for (uint k=0; k < cp.size(); k++) {
    cp_io_out->Float64(kio+0) = cp.t[k];
    cp_io_out->Float64(kio+1) = cp.x[k];
    cp_io_out->Float64(kio+2) = cp.v[k];
    cp_io_out->Float64(kio+3) = cp.a[k];
    cp_io_out->Float64(kio+4) = cp.j[k];
    kio += 5;
  }
  // ReportInfo("setting vector size");
  cp_io_out->VectorSize() = cp.size()*its::Trajectory1D_N_FIELDS;
  StopWriting(cp_io_out);

  // oss << "CP sent of size: " << cp.size();
  // ReportInfo(oss.str().c_str());
  // oss.str("");

  // output the simulation points
  its::Trajectory1D sim = manager->planner->getSimulationPoints();
  // // dynamically allocate memory for this output
  MAPSIOElt * sim_io_out = StartWriting(Output("ref_cmp"));
  sim_io_out->Timestamp() = stamp;
  sim_io_out->VectorSize() = sim.size()*its::Trajectory1D_N_FIELDS;
  for (uint k=0, kio=0; k < sim.size(); k++, kio+=5) {
    sim_io_out->Float64(kio+0) = sim.t[k];
    sim_io_out->Float64(kio+1) = sim.x[k];
    sim_io_out->Float64(kio+2) = sim.v[k];
    sim_io_out->Float64(kio+3) = sim.a[k];
    sim_io_out->Float64(kio+4) = sim.j[k];
  }
  sim_io_out->VectorSize() = sim.size()*its::Trajectory1D_N_FIELDS;
  StopWriting(sim_io_out);

  // oss << "Sim sent of length: " << sim.size();
  // ReportInfo(oss.str().c_str());
  // oss.str("");

  LOG(logDebugCallback, "outputPlan callback finished successfully.");
}


void MAPSITSPlanner::logDebugCallback(
  const std::string & msg,
  const std::string & info
) {
  ReportInfo(std::string(std::string("[DEBUG] ") + info + std::string(": ") + msg).c_str());
}


void MAPSITSPlanner::logInfoCallback( 
  const std::string & msg,
  const std::string & info
) {
  ReportInfo(msg.c_str());
}


void MAPSITSPlanner::logWarnCallback( 
  const std::string & msg,
  const std::string & info
) {
  ReportWarning(std::string(info + std::string(": ") + info).c_str());
}


void MAPSITSPlanner::logErrorCallback(
  const std::string & msg,
  const std::string & info
) {
  ReportError(std::string(info + std::string(": ") + info).c_str());
}
