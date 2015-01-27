clear all;

data = load('log.txt');

%% data labels

dt = 1;
velocity = 2;
altitude = 3;
input = 4;

%% data margins

% okrojen� dat
from = 2000;
to = size(data, 1)-1000;

% od-do fitov�n�
plot_window = from:to;

% od-do identifikace
margin = 200;
iden_window = (from+margin):(to-margin); 

%% integrate dt to time vector

time_plot = integrate(data(plot_window, dt));
time_identifikace = integrate(data(iden_window, dt))+time_plot(margin);

%% position

% integruju rychlost
position = integrate(data(plot_window, velocity), data(plot_window, dt)); 

%% velocity

% fituji rychlost polynomem
velocity_pol = polyfit(time_plot', data(plot_window, velocity)', 40);
velocity_fit = polyval(velocity_pol, time_identifikace);

%% acceleration

% derive the velocity polynomial
acceleration_pol = polyder(velocity_pol);
acceleration_fit = polyval(acceleration_pol, time_identifikace);

%% input

% shift input of the offset
data(:, input) = data(:, input) - mean(data(plot_window(1:50), input));

%% identify transfare input -> acceleration
input_window = data(iden_window, input);
dt_window = data(iden_window, dt);

B = acceleration_fit(2:end);
A = [acceleration_fit(1:end-1).*dt_window(1:end-1) input_window(2:end).*dt_window(2:end)];

P = A\B;
P0 = P(1)
P1 = P(2)

%% Estimating all values from input

acceleration_est = firstOrder(data(plot_window, input), data(plot_window, dt), P);

velocity_est = integrate(acceleration_est, data(plot_window, dt));

position_est = integrate(velocity_est, data(plot_window, dt));

%% plot position

figure(1);
subplot(2, 2, 1);
hold off
plot(time_plot, position, 'b');
hold on
plot(time_plot, position_est, 'g');
title('Position');
legend('px4flow', 'estimated from input');
xlabel('time [s]');
ylabel('position [m]');

%% plot velocity

subplot(2, 2, 2);
hold off
plot(time_plot, data(plot_window, velocity), 'b');
hold on
plot(time_identifikace, velocity_fit, 'r');
plot(time_plot, velocity_est, 'g');
title('Velocity');
legend('px4flow', 'fitted polynome', 'estimated from input');
xlabel('time [s]');
ylabel('velocity [m/s]');

%% plot acceleration

subplot(2, 2, 3);
hold off
plot(time_identifikace, acceleration_fit, 'b');
hold on
plot(time_plot, acceleration_est, 'g');
title('Acceleration');
legend('estimate from px4flow', 'estimate from input');
xlabel('time [s]');
ylabel('acceleration [m/s^2]');

%% plot input

subplot(2, 2, 4);
hold off
plot(time_plot, data(plot_window, input), 'b');
hold on
title('Input');
legend('input');
xlabel('time [s]');
ylabel('input []');
