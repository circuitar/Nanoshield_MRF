% RSSI to dBm Linear Regression
%
% Linear regression to find parameters to convert from RSSI to dBm based on MRF24J40 datasheet.
% The resulting parameters should be used in method getSignalStrength().
%
% This is done using the normal equation based on the following tutorial:
% http://www.lauradhamilton.com/tutorial-linear-regression-with-octave

data = load('rssi.csv');
rssi = data(:,1);
dbm = data(:,2);

X = [ones(length(rssi), 1) rssi];
theta = (pinv(X'*X))*X'*dbm
y = rssi * theta(2) + theta(1);

printf('dBm = rssi * %f + %f\n', theta(2), theta(1));

plot(rssi, dbm, rssi, y);
xlabel('RSSI Value');
ylabel('Received Power (dBm)');
