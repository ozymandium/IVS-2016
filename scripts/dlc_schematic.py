#!/usr/bin/env python
# -*- coding: utf-8 -*-
import g1fitting
import its_utils
import sys
from numpy import ceil, rad2deg, pi
import numpy as np
import matplotlib.pyplot as plt

## settings

obs_dist = 10.
lat_offset = 3.
res = 0.1

color_pri = 'g'
color_alt = 'c'
color_sn = 'b'

## fixed points

x1 = 0.
y1 = 0.
x2 = obs_dist
y2 = lat_offset
x3 = 2.*obs_dist
y3 = 0.
Ls = np.mean((x2-x1,x3-x2))

nleg = ceil(Ls/res)
x00 = list(np.linspace(x1-Ls, x1, nleg))
y00 = [y1]*nleg
x44 = list(np.linspace(x3, x3+Ls, nleg))
y44 = [y3]*nleg

## sinusoidal thing I created

w1 = y2-y1
w2 = y3-y2
L1 = x2-x1
L2 = x3-x2
pth_sn1 = its_utils.lateral_transition_cosine(L1, w1, res)
pth_sn2 = its_utils.lateral_transition_cosine(L2, w2, res, x2, y2)
pth_sn = zip(x00, y00)[:-1] + pth_sn1[:-1] + pth_sn2[:-1] + zip(x44,y44)

## Examine curvature

[R_sn, k_sn, t_sn, dt_sn] = its_utils.circular_curvature(pth_sn)
s_sn = its_utils.convert_path_to_1D(pth_sn)

## Plot

# path
fig1 = plt.figure(facecolor='black')
ax1 = fig1.add_subplot(1,1,1, axisbg='k')
ax1.grid(True, color='w')
ax1.hold(True)
ax1.plot([p[0] for p in pth_sn], [0.]*len(pth_sn), color_pri, linewidth=8, label='Orig. Path')
ax1.plot([p[0] for p in pth_sn], [lat_offset]*len(pth_sn), color_alt, linewidth=8, label='Alt. Path')
ax1.plot([p[0] for p in pth_sn], [p[1] for p in pth_sn], color_sn, linewidth=8, label='DLC Path')
# # w arrow
# ax1.annotate(
#   '', xy=(0, lat_offset), xycoords='data',
#   xytext=(0, 0), textcoords='data',
#   arrowprops={
#     'arrowstyle': '<->',
#     'color': 'w',
#     'linewidth': 6,
#   }
# )
# ax1.text(.5, lat_offset/2., 'w',  bbox=dict(facecolor='white'))
# # L arrow
# ax1.annotate(
#   '', xy=(0, lat_offset+.25), xycoords='data',
#   xytext=(obs_dist, lat_offset+.25), textcoords='data',
#   arrowprops={
#     'arrowstyle': '<->',
#     'color': 'w',
#     'linewidth': 6,
#   }
# )
# ax1.text(obs_dist/2., lat_offset+.5, 'L',  bbox=dict(facecolor='white'))
ax1.yaxis.label.set_color('white')
ax1.xaxis.label.set_color('white')
ax1.tick_params(axis='x', colors='white')
ax1.tick_params(axis='y', colors='white')
ax1.set_title('Path')
ax1.set_xlabel('x (m)')
ax1.set_ylabel('y (m)')
ax1.set_xlim((-1., 21.))
ax1.set_ylim((-1., 4.))
ax1.legend(loc=0)

plt.show()

