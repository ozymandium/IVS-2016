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

t_ = [t(1), t(end)];


f = figure;

p1 = subplot(2,2,1);
hold on
h = plot(tt,ss, 'linewidth', 6);
plot(t_, [ss(end), ss(end)], '--g', 'linewidth', 3)
text(0.05*t(end), 0.95*ss(end), 'L', 'Color', 'g', 'FontSize', 16, 'VerticalAlignment', 'top', 'FontWeight', 'bold')
grid on
whitebg('k')
uistack(h, 'top')
ylabel('Distance', 'FontSize', 16, 'FontWeight', 'bold')
xlabel('Time', 'FontSize', 16, 'FontWeight', 'bold')

p2 = subplot(2,2,2);
hold on
h = plot(tt,vv, 'w', 'linewidth', 6);
plot(t_, [vv(1), vv(1)], '--c', 'linewidth', 3)
text(0.95*t(end), 0.95*vv(1), 'v_i', 'Color', 'c', 'FontSize', 16, 'VerticalAlignment', 'top', 'HorizontalAlignment', 'right', 'FontWeight', 'bold')
grid on
whitebg('k')
uistack(h, 'top')
ylabel('Speed', 'FontSize', 16, 'FontWeight', 'bold')
xlabel('Time', 'FontSize', 16, 'FontWeight', 'bold')

p3 = subplot(2,2,3);
hold on
h = plot(tt,aa, 'w', 'linewidth', 6);
plot(t_, [min(aa), min(aa)], '--c', 'linewidth', 3)
text(0.05*t(end), 1.05*min(aa), 'a_p^-', 'Color', 'c', 'FontSize', 16, 'VerticalAlignment', 'top', 'HorizontalAlignment', 'left', 'FontWeight', 'bold')
plot(t_, 1.5*[min(aa), min(aa)], '--r', 'linewidth', 3)
text(0.95*t(end), 0.95*1.5*min(aa), 'a_m^-', 'Color', 'r', 'FontSize', 16, 'VerticalAlignment', 'bottom', 'HorizontalAlignment', 'right', 'FontWeight', 'bold')
plot([t(1) t(end)/2], [aa(1), aa(1)], '--c', 'linewidth', 3)
text(0.45*t(end), 0.05*min(aa), 'a_i', 'Color', 'c', 'FontSize', 16, 'VerticalAlignment', 'top', 'HorizontalAlignment', 'right', 'FontWeight', 'bold')
grid on
whitebg('k')
uistack(h, 'top')
xlabel('Time', 'FontSize', 16, 'FontWeight', 'bold')
ylabel('Accel', 'FontSize', 16, 'FontWeight', 'bold')

p4 = subplot(2,2,4);
hold on
h = plot(tt,jj, 'w', 'linewidth', 6);
plot(t_, [min(jj), min(jj)], '--c', 'linewidth', 3)
text(0.95*t(end), 0.95*min(jj), 'j_p^-', 'Color', 'c', 'VerticalAlignment', 'bottom', 'HorizontalAlignment', 'right', 'FontSize', 16, 'FontWeight', 'bold')
plot(t_, [max(jj), max(jj)], '--c', 'linewidth', 3)
text(0.05*t(end), 0.95*max(jj), '-j_p^-', 'Color', 'c', 'VerticalAlignment', 'top', 'HorizontalAlignment', 'left', 'FontSize', 16, 'FontWeight', 'bold')
plot(t_, 1.5*[min(jj), min(jj)], '--r', 'linewidth', 3)
text(0.95*t(end), 0.95*1.5*min(jj), 'j_m^-', 'Color', 'r', 'VerticalAlignment', 'bottom', 'HorizontalAlignment', 'right', 'FontSize', 16, 'FontWeight', 'bold')
plot(t_, 1.5*[max(jj), max(jj)], '--r', 'linewidth', 3)
text(0.05*t(end), 0.95*1.5*max(jj), '-j_m^-', 'Color', 'r', 'VerticalAlignment', 'top', 'HorizontalAlignment', 'left', 'FontSize', 16, 'FontWeight', 'bold')
grid on
whitebg('k')
uistack(h, 'top')
ylabel('Jerk', 'FontSize', 16, 'FontWeight', 'bold')
xlabel('Time', 'FontSize', 16, 'FontWeight', 'bold')

p1.Position(3) = 0.43;
p2.Position(3) = 0.43;
p3.Position(3) = 0.43;
p4.Position(3) = 0.43;
p1.Position(4) = 0.4;
p2.Position(4) = 0.4;
p3.Position(4) = 0.4;
p4.Position(4) = 0.4;
p1.Position(1) = 0.05;
p3.Position(1) = 0.05;
p2.Position(1) = 0.55;
p4.Position(1) = 0.55;

