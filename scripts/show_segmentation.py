#!/usr/bin/env python
import sys, os
from its_utils import *
import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec
from numpy import sqrt
from itertools import groupby


def group(iterable, predicate = lambda x: bool(x)):
  """groups an iterable by the result of running predicate on each member.
  groups are not necessarily distinct.
  @return idx the first index of each group
  @return val the value of each group
  """
  groups = [(0, predicate(iterable[0]))]
  for idx in range(1, len(iterable)):
    res = predicate(iterable[idx])
    if groups[-1][1] == res:
      continue
    else:
      groups.append((idx, res))
  return zip(*groups)


dat = []
with open(sys.argv[1], 'r') as path_csv:
  for line in path_csv:
    for v in line.split(','):
      dat.append(float(v))
ugly_path = zip([d - dat[0] for d in dat[0::3]],
                [d - dat[1] for d in dat[1::3]])

################################################################################

# # fig1 = plt.figure()
# # ax = fig1.add_subplot(1,1,1)
# # ax.set_title('Ugly path indices')
# # ax.hold(True)
# # for i in range(len(ugly_path)):
# #   ax.plot(ugly_path[i][0], ugly_path[i][1], 'b.')
# #   ax.annotate(str(i), xy=ugly_path[i])
# # ax.grid(True)

# path = pretty_path(ugly_path[15:80], 10, 3, 0.1, 250)
# s = convert_path_to_1D(path)
# _, k, _, _ = circular_curvature(path)


# fig = plt.figure(facecolor='black')
# ax = fig.add_subplot(1,1,1, axisbg='k')
# ax.hold(True)
# for i in range(len(path)):
#   if abs(k[i]) < 0.07:
#     ax.plot(path[i][0], path[i][1], 'b.', markersize=8)
# for i in range(len(path)):
#   if abs(k[i]) >= 0.07:
#     ax.plot(path[i][0],path[i][1], 'r.', markersize=8)
# 
# ax.grid(True, color='w')
# ax.yaxis.label.set_color('white')
# ax.xaxis.label.set_color('white')
# ax.tick_params(axis='x', colors='white')
# ax.tick_params(axis='y', colors='white')
# plt.savefig(os.path.join(os.path.dirname(os.path.realpath(__file__)), 'course_highlighted_turns.png'), dpi=500, facecolor='k', transparent=True)

# fig = plt.figure(facecolor='black')
# ax = fig.add_subplot(1,1,1, axisbg='k')
# ax.hold(True)
# ax.grid(True, color='w')
# ax.yaxis.label.set_color('white')
# ax.xaxis.label.set_color('white')
# ax.tick_params(axis='x', colors='white')
# ax.tick_params(axis='y', colors='white')
# for i in range(len(path)):
#   if abs(k[i]) >= 0.07:
#     ax.plot(s[i], 0, 'r.')
#   else:
#     ax.plot(s[i], 0, 'b.')

# # from ipdb import set_trace
# # set_trace()

# vmax_aBy = [sqrt(2.5/abs(kk)) for kk in k]
# idxs, is_curve = group(vmax_aBy, predicate = lambda v: v < 11.176)
# s_segment = [s[i] for i in idxs]
# v_segment = [11.176] * len(s_segment)
# for i in range(len(idxs)):
#   if is_curve[i]:
#     v_segment[i] = min(vmax_aBy[idxs[i]:idxs[i+1]])
# s_segment.append(s[-1])
# v_segment.append(v_segment[-1])

# fig = plt.figure(facecolor='black')
# ax = fig.add_subplot(1,1,1, axisbg='k')
# ax.hold(True)
# ax.plot([s[0], s[-1]], [11.176]*2, '-c', linewidth=7, label='Speed limit')
# ax.step(s_segment, v_segment, 'b.', label='$ceiling$', where='post', linewidth=4)
# ax.plot(s, vmax_aBy, '-r', label='$v_{B,x}(a_{B,y} , \kappa)$', linewidth=3)#, alpha=0.6)
# ax.grid(True, color='w')
# ax.yaxis.label.set_color('white')
# ax.xaxis.label.set_color('white')
# ax.tick_params(axis='x', colors='white')
# ax.tick_params(axis='y', colors='white')
# ax.set_ylabel('Speed Ceiling (m/s)')
# ax.set_xlabel('Path Length (m)')
# ax.set_ylim((0, 14))
# ax.legend(loc=0, fontsize=16)
# plt.savefig(os.path.join(os.path.dirname(os.path.realpath(__file__)), 'speed_ceiling.png'), dpi=500, facecolor='k')

################################################################################

path = pretty_path(ugly_path, 20, 3, 0.1, 250)
s = convert_path_to_1D(path)
_, k, _, _ = circular_curvature(path)

# fig1 = plt.figure()
# ax = fig1.add_subplot(1,1,1)
# ax.set_title('Ugly path indices')
# ax.hold(True)
# for i in range(len(ugly_path)):
#   ax.plot(ugly_path[i][0], ugly_path[i][1], 'b.')
#   ax.annotate(str(i), xy=ugly_path[i])
# ax.grid(True)

# # 25, 97
fig = plt.figure(facecolor='black')
ax = fig.add_subplot(1,1,1, axisbg='k')
ax.hold(True)
ax.grid(True, color='w')
ax.yaxis.label.set_color('white')
ax.xaxis.label.set_color('white')
ax.tick_params(axis='x', colors='white')
ax.tick_params(axis='y', colors='white')
ax.set_aspect('equal', 'datalim')
ax.set_xlabel('East (m)')
ax.set_ylabel('North (m)')
subpaths = [
  (ugly_path[0:26], 'w'),
  (ugly_path[25:98], 'w'), 
  (ugly_path[97:], 'w'),
]
for subpath, color in subpaths:
  ax.plot([p[0] for p in subpath], [p[1] for p in subpath], '-'+color+'.', linewidth=8)










plt.show()
