%%
% clear all; close all; clc
% 
% j = [1 0 -1 0 -1 0 1];
% t = [0 1 2 3 4 5 6 7];
% jpp = mkpp(t,j);
% app = fnint(jpp, 0);
% vpp = fnint(app, 0);
% spp = fnint(vpp, 0);
% 
% tt = linspace(t(1), t(end), 500);
% ss = fnval(spp, tt);
% vv = fnval(vpp, tt);
% aa = fnval(app, tt);
% jj = fnval(jpp, tt);
% 
% f = figure
% subplot(4,1,1)
% plot(tt,ss, 'linewidth', 6)
% grid on
% whitebg('k')
% ylabel('Pos', 'FontSize', 16, 'FontWeight', 'bold')
% title('7 Phase Profile')
% subplot(4,1,2)
% plot(tt,vv, 'linewidth', 6)
% grid on
% whitebg('k')
% ylabel('Speed', 'FontSize', 16, 'FontWeight', 'bold')
% subplot(4,1,3)
% plot(tt,aa, 'linewidth', 6)
% grid on
% ylabel('Accel', 'FontSize', 16, 'FontWeight', 'bold')
% whitebg('k')
% subplot(4,1,4)
% plot(tt,jj, 'linewidth', 6)
% grid on
% whitebg('k')
% ylabel('Jerk', 'FontSize', 16, 'FontWeight', 'bold')
% xlabel('Time', 'FontSize', 16, 'FontWeight', 'bold')

%%
clear all; close all; clc

j = [1 0 -1 0 -1 0 1];
t = [0 .5 1.5 2.5 4 5 6.9+[0 1]];
jpp = mkpp(t,j);
app = fnint(jpp, .5);
vpp = fnint(app, 1);
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
plot(t_, [15 15], '--c', 'linewidth', 3)
text(1, 14, 'L', 'Color', 'c', 'FontSize', 16, 'VerticalAlignment', 'top')
grid on
whitebg('k')
uistack(h, 'top')
ylabel('Distance', 'FontSize', 16, 'FontWeight', 'bold')
xlabel('Time', 'FontSize', 16, 'FontWeight', 'bold')


p2 = subplot(2,2,2);
hold on
h = plot(tt,vv, 'linewidth', 6);
plot([0 2], [1 1], '--c', 'linewidth', 3)
text(2,1, 'v_i','Color', 'c', 'FontSize', 16, 'VerticalAlignment', 'top', 'FontWeight', 'bold')
plot([6 8], [0 0], '--c', 'linewidth', 3)
text(6,0, 'v_f ','Color', 'c', 'FontSize', 16, 'VerticalAlignment', 'bottom', 'HorizontalAlignment', 'right', 'FontWeight', 'bold')
plot(t_, [2.9 2.9], '--c', 'linewidth', 3)
text(8, 2.5, 'v_m ', 'Color', 'c', 'FontSize', 16, 'FontWeight', 'bold', 'HorizontalAlignment', 'right')
uistack(h, 'top')
grid on
whitebg('k')
ylabel('Speed', 'FontSize', 16, 'FontWeight', 'bold')
xlabel('Time', 'FontSize', 16, 'FontWeight', 'bold')


p3 = subplot(2,2,3);
hold on
h = plot(tt,aa, 'linewidth', 6);
plot([0 3], [1 1], '--c', 'linewidth', 3)
text(3, 1, 'a_p^+', 'Color', 'c', 'FontSize', 16, 'VerticalAlignment', 'top', 'FontWeight', 'bold')
plot([0 4], [1.5 1.5], '--r', 'linewidth', 3)
text(4, 1.5, 'a_m^+', 'Color', 'r', 'FontSize', 16, 'VerticalAlignment', 'top', 'FontWeight', 'bold')
plot([2.5 8], [-1.5, -1.5], '--r', 'linewidth', 3)
text(2.5, -1.5, 'a_m^- ', 'Color', 'r', 'FontSize', 16, 'VerticalAlignment', 'bottom', 'FontWeight', 'bold', 'HorizontalAlignment', 'right')
plot([3.5 8], [-1 -1], '--c', 'linewidth', 3)
text(3.5, -1, 'a_p^-', 'Color', 'c', 'FontSize', 16, 'VerticalAlignment', 'bottom', 'FontWeight', 'bold', 'HorizontalAlignment', 'right')
grid on
ylabel('Accel', 'FontSize', 16, 'FontWeight', 'bold')
uistack(h, 'top')
whitebg('k')
xlabel('Time', 'FontSize', 16, 'FontWeight', 'bold')


p4 = subplot(2,2,4);
hold on
h = plot(tt,jj, 'linewidth', 6);
plot([0 2.5], [1 1], '--c', 'linewidth', 3)
text(2.5, 1, 'a_p^+', 'Color', 'c', 'FontSize', 16, 'VerticalAlignment', 'top', 'FontWeight', 'bold')
plot([0 2.5], [1.5 1.5], '--r', 'linewidth', 3)
text(2.5, 1.5, 'a_m^+', 'Color', 'r', 'FontSize', 16, 'VerticalAlignment', 'top', 'FontWeight', 'bold')
plot([0 2.5], [-1 -1], '--c', 'linewidth', 3)
text(2.5, -1, ' -a_p^+', 'Color', 'c', 'FontSize', 16, 'VerticalAlignment', 'bottom', 'FontWeight', 'bold')
plot([0 2.5], [-1.5 -1.5], '--r', 'linewidth', 3)
text(2.5, -1.5, '-a_m^+', 'Color', 'r', 'FontSize', 16, 'VerticalAlignment', 'bottom', 'FontWeight', 'bold')
plot([4 8], [-1.5, -1.5], '--r', 'linewidth', 3)
text(4, -1.5, 'a_m^- ', 'Color', 'r', 'FontSize', 16, 'VerticalAlignment', 'bottom', 'FontWeight', 'bold', 'HorizontalAlignment', 'right')
plot([4 8], [-1 -1], '--c', 'linewidth', 3)
text(4, -1, 'a_p^- ', 'Color', 'c', 'FontSize', 16, 'VerticalAlignment', 'bottom', 'FontWeight', 'bold', 'HorizontalAlignment', 'right')
plot([4 8], [1.5, 1.5], '--r', 'linewidth', 3)
text(4, 1.5, '-a_m^- ', 'Color', 'r', 'FontSize', 16, 'VerticalAlignment', 'top', 'FontWeight', 'bold', 'HorizontalAlignment', 'right')
plot([4 8], [1 1], '--c', 'linewidth', 3)
text(4, 1, '-a_p^- ', 'Color', 'c', 'FontSize', 16, 'VerticalAlignment', 'top', 'FontWeight', 'bold', 'HorizontalAlignment', 'right')
text(0.1, -0.5, '\Delta t_1')
text(0.9, -0.5, '\Delta t_2')
text(1.9, 0.5, '\Delta t_3')
text(3, 0.5, '\Delta t_4')
text(4.3, 0.5, '\Delta t_5')
text(5.9, -0.5, '\Delta t_6')
text(7.3, -0.5, '\Delta t_7')
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


