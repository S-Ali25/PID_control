data = readmatrix('arduino_log.csv');
setpoint_ard = data(:,1);
current_value_ard = data(:,2);
logsout = out.logsout;                        % the dataset of all logged signals
plantSignal = logsout.getElement(1);           % gets the first (only) logged signal
simout = plantSignal.Values.Data;              % the actual numeric values
tout = plantSignal.Values.Time;                % the corresponding time vector
dt_log = 0.1;  % your print throttle, in seconds
t_ard = (0:length(current_value_ard)-1) * dt_log;
t_ard_shifted = t_ard;  % adjust with + or - offset if needed to align the jump moment
plot(t_ard_shifted, current_value_ard, 'b', 'LineWidth', 1.5); hold on;
plot(tout, simout, 'r--', 'LineWidth', 1.5);
legend('Arduino (real)', 'Simulink (simulated)');
xlabel('Time (s)');
ylabel('Plant output');
title('Real hardware vs simulated PID response, Kp=15, Ki=10');
grid on;