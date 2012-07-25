
% Some parameters
cpf=153             
bias_length=12      % (feet)
raw_length=8        % running average window length (feet)
add=1

%  ind = bump cart for inclim.
ind=2
ind1=3

%   ******************************************************************
%   Main
%   ******************************************************************

% ********************************************************************
%fin='C:\012005\02\pcwb.1'
fin='pcwb.1'
fout='C:\012005\02\pcwb2.out'

fid=fopen(fin)
fout=fopen(fout,'w')

[a,cc]=fscanf(fid,' %f %f %f',[3,inf]);

cts= fix(cc/3)
%set up arrays
x=[1:cts];
gyro=[1:cts];
gyro1=[1:cts];
f_gyro=[1:cts]; % readings in angles
diff=[1:cts];
gg=[1:cts];

%
scal=20/2^16;

figure;
subplot(2,1,1);
plot(a(2,:));
title('channel 1');

subplot(2,1,2);
plot(a(3,:));
title('channel 2');

for i = 1:cts
    gyro(i)=(a(ind,i)*scal -10);
    gyro(i)=gyro(i)-0.004;

    gyro1(i)=(a(ind1,i)*scal -10);
    gyro1(i)=gyro1(i)-0.008;

    diff(i)= (gyro(i)+gyro1(i))/2;
    gyro(i)=-(gyro(i)-diff(i));

    %now correct for angle
    f_gyro(i) = (gyro(i)) /(1.414*5.002);
    %f_gyro(i) = (gyro1(i)) /(1.414*5.012);
    gyro(i) = asin(f_gyro(i)); %asin(f_gyro(i));
    %gyro(i) = asin((gyro(i))/(1.414*5.002));    
    %gyro1(i) = asin((gyro1(i)) /(1.414*5.012));
end

%f-gyro = readings in angles

%
%   convert to angles displacement
%   *****************************
fact=1/cpf; %Convert to feet proportional
pts=fix(cts/add)
j=1;
pp=zeros(1,cts);    % profile buffer
bb=zeros(1,cts);    % profile buffer with removed bias
raw=zeros(1,cts);
bias_length=bias_length*cpf;
raw_length=raw_length*cpf;

% Calculate profile
%  initial set up
for i=1:add
    j=i+add;
    pp(i)=sin(f_gyro(j))*fact;
    bb(i)=pp(i);
end

% Now process the rest.
for i=add+1:cts-add
   j=i+add;
   pp(i)=pp(i-add) + sin(f_gyro(j))*fact;
   bb(i)=pp(i);
end

for i=bias_length+raw_length:length(pp)-raw_length/2
    raw(i-raw_length/2)=pp(i-raw_length/2)-mean(pp(i-raw_length:i));
end

profile=pp(cpf/2:end-cpf/2);
run_avg=raw(cpf/2:end-cpf/2);
%profile=profile;
xdist=[1:length(profile)]/cpf;
%profile1=profile;
%profile1=detrend(profile);
profile=profile*12;
figure();
subplot(4,1,1);
plot(xdist,profile);
subplot(4,1,2);
plot(xdist,run_avg);
grid

% Remove the bias
for i=bias_length+1:length(pp)
    %bias_sum = sum(pp(i-bias_length+1:i));
    %bb(i)=pp(i)-bias_sum/bias_length;
    bb(i)=pp(i)-mean(pp(i-25:i));
end

for i=bias_length+raw_length:length(pp)-raw_length/2
    raw(i-raw_length/2)=bb(i-raw_length/2)-mean(bb(i-raw_length:i));
end

bprofile=bb(cpf/2:end-cpf/2);
run_avg=raw(cpf/2:end-cpf/2);
xdist=[1:length(bprofile)]/cpf;
bprofile=bprofile*12;
subplot(4,1,3);
plot(xdist,bprofile);
subplot(4,1,4);
plot(xdist,run_avg);
%axis([15 100 -0.05 0.05])
grid

