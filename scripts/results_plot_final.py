#!/usr/bin/env python
import sys, os
import pyqtgraph as pg
from scipy.signal import filtfilt, savgol_filter
import numpy as np
import pyqtgraph.exporters


slice_cmp_filen = '/media/rgcofield/Handvaska/GoogleDrive/GAVLab/IVS-2016/Data/Export_ITS_20160419_142308_ITS_vehicle_planning_1__ITSPlanner_1_slice_cmp.csv'
slice_s_filen = '/media/rgcofield/Handvaska/GoogleDrive/GAVLab/IVS-2016/Data/Export_ITS_20160419_142308_ITS_vehicle_planning_1__ITSPlanner_1_slice_s.csv'
state_filen = 'Data/Export_ITS_20160419_142308_vehicle_state_2__Kinematic_state_vectorizer_outputFloat.csv'


this_dir = os.path.dirname(os.path.abspath(__file__))
data_dir = os.path.join(this_dir, 'Data')


def rtm_time_str_to_float(s):
  return float(s.split(':')[0])*60. + float(s.split(':')[1])

def smooth(y, box_pts):
  """moving average"""
  box = np.ones(box_pts)/box_pts
  y_smooth = np.convolve(y, box, mode='same')
  return y_smooth

# slice_cmp:
#  RTMTime, reftime, x, y, heading, curvature, speed, accel, jerk &&&& repeat
# slice_time = []
slice_v = []
slice_a = []
for line in open(slice_cmp_filen, 'r'):
  data = line.split(';')
  # slice_time.append(rtm_time_str_to_float(data[0]))
  slice_v.append(float(data[6]))
  slice_a.append(float(data[7]))

# slice_s
veh_s = []
for line in open(slice_s_filen, 'r'):
  data = line.split(';')
  veh_s.append(float(data[1]))

# veh state:
#  RTMTime, x, y, speed, accel, heading ????
# veh_time = []
veh_v = []
veh_a = []
for line in open(state_filen):
  data = line.split(';')
  if len(data) < 3:
    continue
  # veh_time.append(rtm_time_str_to_float(data[0]))
  veh_v.append(float(data[4]))
  veh_a.append(float(data[5 ]))

################

# veh_a = np.array(veh_a)

# veh_a *= 2.5

# # vehicle data is at ~25 hz

# # veh_a = filtfilt((0.01), (0.001, 1), veh_a)

# # veh_a = savgol_filter(veh_a, 25, 2  )

# veh_a = smooth(veh_a, 50)

###########

# entire data collection
#   begin at 0:10.671
#     slice_cmp: line 1
#     slice_s: line 1
#     state: line 16
#   end at 01:33.675306
#     slice_cmp: line 2076
#     slice_s: line 2076
#     state: line 2091

# new subpath at 
#   0:27 82 meters
#   01:12 203 meters
#   last subpath: 21 meters

# second subpath:
#   being at 0:27.132
#     slice_cmp: line 413
#     slice_s: line 413
#     state: line 427/428
#   end at 01:12.203
#     slice_cmp: line 1538
#     slice_s: line 1538
#     state: line 1554

slice_v1 = slice_v[:412]
slice_a1 = slice_a[:412]
veh_s1 = veh_s[:412]
veh_v1 = veh_v[16:428]
veh_a1 = veh_a[16:428]

slice_v2 = slice_v[413:1538]
slice_a2 = slice_a[413:1538]
veh_s2 = veh_s[413:1538]
veh_v2 = veh_v[428:1553]
veh_a2 = veh_a[428:1553]

slice_v3 = slice_v[1538:2076]
slice_a3 = slice_a[1538:2076]
veh_s3 = veh_s[1538:2076]
veh_v3 = veh_v[1553:]
veh_a3 = veh_a[1553:]


def quit(*args):
  pg.QtGui.QApplication.instance().quit()
  sys.exit()
import signal
signal.signal(signal.SIGINT, quit)

# win1 = pg.GraphicsWindow()
# fig_1_1 = win1.addPlot(row=0, col=0)
# fig_1_1.addLegend()
# fig_1_1.plot(veh_s1, slice_v1, name='plan', pen='b')
# fig_1_1.plot(veh_s1, veh_v1, name='actual', pen='r')
# fig_1_1.showGrid(x=True, y=True, alpha=0.5)
# fig_1_1.setLabels(left='Speed (m/s)', top='Subpath1')
# fig_1_2 = win1.addPlot(row=1, col=0)
# fig_1_2.addLegend()
# fig_1_2.plot(veh_s1, slice_a1, name='plan', pen='b')
# fig_1_2.plot(veh_s1, veh_a1, name='actual', pen='r')
# fig_1_2.showGrid(x=True, y=True, alpha=0.5)
# fig_1_2.setLabels(bottom='Path Distance (m)', left='Long. Accel. (m/s^2)')

labelStyle = {'color': '#FFF', 'font-size': '14pt', 'bold': True, 'italic': False}
legendLabelStyle = {'color': '#FFF', 'size': '12pt', 'bold': True, 'italic': False}

win2 = pg.GraphicsWindow()
fig_2_1 = win2.addPlot(row=0, col=0)
fig_2_1.addLegend()
fig_2_1.plot(veh_s2, slice_v2, name='plan', pen=pg.mkPen('c', width=6))
fig_2_1.plot(veh_s2, veh_v2, name='actual', pen=pg.mkPen('r', width=6))
fig_2_1.showGrid(x=True, y=True, alpha=0.5)
# fig_2_1.setLabels(left='Speed (m/s)', top='Subpath2')
fig_2_2 = win2.addPlot(row=1, col=0)
fig_2_2.addLegend()
fig_2_2.plot(veh_s2, slice_a2, name='plan', pen=pg.mkPen('c', width=6))
fig_2_2.plot(veh_s2, veh_a2, name='actual', pen=pg.mkPen('r', width=6))
fig_2_2.showGrid(x=True, y=True, alpha=0.5)
# fig_2_2.setLabels(bottom='Path Distance (m)', left='Long. Accel. (m/s^2)')

# a = fig2
fig_2_1.getAxis('left').setLabel('Speed (m/s)', **labelStyle)
fig_2_2.getAxis('left').setLabel('Accel (m/s^2)', **labelStyle)
fig_2_2.getAxis('bottom').setLabel('Path Distance (m)', **labelStyle)

for p1 in fig_2_1, fig_2_2:
  for item in p1.legend.items:
    for single_item in item:
      if isinstance(single_item, pg.graphicsItems.LabelItem.LabelItem):
          single_item.setText(single_item.text, **legendLabelStyle)


# win3 = pg.GraphicsWindow()
# fig_3_1 = win3.addPlot(row=0, col=0)
# fig_3_1.addLegend()
# fig_3_1.plot(veh_s3, slice_v3, name='plan', pen='b')
# fig_3_1.plot(veh_s3, veh_v3, name='actual', pen='r')
# fig_3_1.showGrid(x=True, y=True, alpha=0.5)
# fig_3_1.setLabels(left='Speed (m/s)', top='Subpath3')
# fig_3_2 = win3.addPlot(row=1, col=0)
# fig_3_2.addLegend()
# fig_3_2.plot(veh_s3, slice_a3, name='plan', pen='b')
# fig_3_2.plot(veh_s3, veh_a3, name='actual', pen='r')
# fig_3_2.showGrid(x=True, y=True, alpha=0.5)
# fig_3_2.setLabels(bottom='Path Distance (m)', left='Long. Accel. (m/s^2)')


# for win in range(1,4):
#   ex = pg.exporters.ImageExporter(locals()['win'+str(win)].scene())
#   ex.parameters()['width'] = 2000
#   ex.export('speed_accel_subpath'+str(win)+'.png')

pg.QtGui.QApplication.instance().exec_()