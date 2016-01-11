%%
clear all; close all; clc

j = [1 0 -1 0 -1 0 1];
t = [0 1 2 3 4 5 6 7];
jpp = mkpp(t,j);
app = fnint(jpp, 0);
vpp = fnint(app, 0);
spp = fnint(vpp, 0);

tt = linspace(t(1), t(end), 500);
ss = fnval(spp, tt);
vv = fnval(vpp, tt);
aa = fnval(app, tt);
jj = fnval(jpp, tt);

f = figure
plot(tt,ss, 'linewidth', 6)
grid on
xlabel('t', 'FontSize', 16, 'FontWeight', 'bold')
ylabel('s(t)', 'FontSize', 16, 'FontWeight', 'bold')
whitebg('k')

f = figure
plot(ss,tt, 'linewidth', 6)
grid on
xlabel('s', 'FontSize', 16, 'FontWeight', 'bold')
ylabel('t(s)', 'FontSize', 16, 'FontWeight', 'bold')
whitebg('k')

