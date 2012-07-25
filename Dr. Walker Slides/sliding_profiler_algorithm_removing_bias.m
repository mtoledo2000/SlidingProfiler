%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% File: sliding_profiler_algorithm_removing_bias.m
%
% Description: 
% This script calculates the profile and the bias in the
% profile. The bias is removed before the running average of the adjusted
% bias is calculated and before the running the analysis algorithm. 
% Plots of the profile, profile with removed bias, and running difference
% are plotted.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Algorithm parameters
threshold=150
cpf=224
fact=12000/cpf;         % Convert to mills proportional
biaslength = 12.5       % How many feet do we wait before calculate the bias and 
                        % start applieng it
raw_length = 8          % Running average window length (in feet) 
bump_check = 1*cpf      % How many counts per foot to wait before checking 
                        % after a bump was located 

add=1                    
volt_ind=2  % raw voltage column index in data
prof_ind=3  % profile column index in data 

%******************************************************************
%   Main - read data and plot profile from sliding profiler
%******************************************************************
fin='sliding_profiler-2142006_132257.txt' %'HallwayWithBump.txt'
fid=fopen(fin)

[a,cc]=fscanf(fid,'%f %f %f %f %f',[5,inf]);

cts= fix(cc/5)
%set up arrays
f_gyro=[1:cts]; % readings in angles
for i = 1:cts
    f_gyro(i) = (a(volt_ind,i)-0.008)/(1.414*5.012);
end

figure;
plot(a(prof_ind,:));
title('Profile - measured by Sliding Profiler in El Paso');
grid on;

%f-gyro = readings in angles
pp=zeros(1,cts);    % profile 
ngg=zeros(1,cts);   % profile with removed bias

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  initial set up
j=1;
for i=1:add
    j=i+add;
    pp(i)=f_gyro(j)*fact;
    ngg(i) = pp(i);
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Calculate profile
for i=add+1:cts-add
   j=i+add;
   pp(i)=pp(i-add) + f_gyro(j)*fact;
   ngg(i)=pp(i);
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Now remove the bias
bias_len = biaslength*cpf;
bias_sum=sum(pp(1:bias_len));

for i=bias_len+1:cts-1
    bias_sum = bias_sum - pp(i-bias_len)+pp(i+1);
    bias = bias_sum/bias_len;
    ngg(i) = pp(i)-bias;    
end


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Analysis algorithm
profile=ngg;
bump = zeros(1,length(profile));
diff = zeros(1,length(profile));
raw = zeros(1,length(profile));
avgsize = raw_length*cpf;
idx = 0;
for i=bias_len+avgsize/2+1:length(profile)-avgsize/2
    raw(i) = mean(profile(i-avgsize/2+1:i+avgsize/2));
    %raw(i) = mean(profile(i-avgsize+1:i));
    diff(i) = profile(i) - raw(i);

    if (idx>0)
        idx = idx - 1;
    else
        if (abs(diff(i))>threshold)
            bump(i) = 1;
            idx = bump_check;
        end
    end
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Now plot the profile, profile with removed bias, and running difference
profile = pp(cpf/2:end-cpf/2);
profile_no_bias = ngg(cpf/2:end-cpf/2);
diff = diff(cpf/2:end-cpf/2);
bump = bump(cpf/2:end-cpf/2);
xdist = [1:length(profile)]/cpf;

figure();
plot(xdist, profile);
title('Profile');
grid on;

figure();
plot(xdist, profile_no_bias);
title(strcat('Profile with bias removed (bias buffer length',num2str(biaslength),')'));
grid on;

pt=zeros(1,length(profile));    % threshold 150 line
pt1=zeros(1,length(profile));   % threshold 75 line
pt=pt + threshold;
pt1=pt1 + 75;
figure;
subplot(2,1,1);
plot(xdist, diff, xdist, pt, xdist, pt * -1, xdist, pt1, xdist, pt1 * -1);
legend('diff','150mil threshold','-150mil threshold','75mil threshold','-75mil threshold','Location','NorthWest');
title(strcat('Running difference - average window length -  ',num2str(raw_length),' feet'));
grid on;

subplot(2,1,2);
plot(xdist,bump);
axis([0 1000 -0.5 1.5]);
title(strcat('Bump threshold - ',num2str(threshold),' mills; Bump check - ',num2str(bump_check),' feet'));

fclose('all')
