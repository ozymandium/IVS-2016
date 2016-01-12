clear all; close all; clc

j = [-1 0 1];
t = [0 1 2 3];
jpp = mkpp(t,j);
app = fnint(jpp, 0);
vpp = fnint(app, 2);
spp = fnint(vpp, 0);

tt = linspace(t(1), t(end), 500);
ss = fnval(spp, tt);
vv = fnval(vpp, tt);
aa = fnval(app, tt);
jj = fnval(jpp, tt);

f = figure
subplot(4,1,1)
plot(tt,ss, 'linewidth', 6)
grid on
whitebg('k')
ylabel('Pos', 'FontSize', 16, 'FontWeight', 'bold')
subplot(4,1,2)
plot(tt,vv, 'linewidth', 6)
grid on
whitebg('k')
ylabel('Speed', 'FontSize', 16, 'FontWeight', 'bold')
subplot(4,1,3)
plot(tt,aa, 'linewidth', 6)
grid on
ylabel('Accel', 'FontSize', 16, 'FontWeight', 'bold')
whitebg('k')
subplot(4,1,4)
plot(tt,jj, 'linewidth', 6)
grid on
whitebg('k')
ylabel('Jerk', 'FontSize', 16, 'FontWeight', 'bold')
xlabel('Time', 'FontSize', 16, 'FontWeight', 'bold')

f = figure
plot(ss,vv, 'linewidth', 6)
grid on
whitebg('k')
xlabel('Pos', 'FontSize', 16, 'FontWeight', 'bold')
ylabel('Speed', 'FontSize', 16, 'FontWeight', 'bold')