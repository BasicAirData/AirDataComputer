//ADC Asgard CSV Loader. JLJ . Basicairdata
//1 debug mode 
debug=1;
exec('rhoair.sci')
exec('viscosityair.sci')
exec('ISAaltitude.sci')
clc
//Q = csvRead("LG3-50HZ.CSV") //Field data  <-  Buono
Q = csvRead("LG57600.CSV") //Field data
[r c]=size(Q);
ngroups=20;//Number of data sets
fsample=0; //Averaged value of fsample
basetime=Q(1,17); //Starting mills
RH=0; //Relative humidity
//Define columns -> data correspondence
//Raw sensor data
deltapcol=3; //Deltap column is #3;
abscol=4; //Abspressure column is #4;
exttempcol=5; //External temp is #5;
tempdeltapcol=6; //Temp from deltap sensor #6;
tempabscol=7; //Temp from Abs pressure sensor #7;
//Measurements
absmcol=9;
IASmcol=13;
TASmcol=14;
ExtTempmcol=10;
tempdeltapmcol=11;
tempabsmcol=12;
altitude=15;
OAT=16;
rhoairmcol=22;
vair=23;
Re=24;
cfactor=25;
//Define tollerance band
airdensityeband=0.5; //Air Density Percent of rho
viscosityeband=0.5;//Viscosity %
altitudeeband=0.5;//Altitude %
fsamplev=zeros(1,ngroups);

//Calculate the average sample frequency from the samples
//Divide the data in ngroups
[pippo maxcount]=size(1:r/ngroups:r)
for ng=1:ngroups
    for i=1: maxcount
        fsamplev(ng)=fsamplev(ng) + 1000/((Q(3+ng*maxcount+i,17)-Q(2+ng*maxcount+i,17)))
    end
    fsamplev(ng)= fsamplev(ng)/ngroups;
end
fsample=mean(fsamplev)
//FFT Plot
N=r; //Number of ADC Samples
sample_rate=fsample;
f=sample_rate*(0:(N/2))/N; //associated frequency vector
n=size(f,'*')
fig=scf(1);
clf(1);
fig.figure_name='Fast Fourier Transform of ADC Samples';
//Abs pressure
subplot(3,2,1)
xtitle('Absolute Pressure Sensor, Frequency [Hz] vs FFT')
y=fft(Q(:,abscol)-mean(Q(:,abscol)));
plot(f',abs(y(1:n)))
//Deltap
subplot(3,2,2)
xtitle('Deltape Sensor, Frequency [Hz] vs FFT')
y=fft(Q(:,deltapcol)-mean(Q(:,deltapcol)));
plot(f',abs(y(1:n)))
//External Temperature
subplot(3,2,3)
xtitle('External Temp Sensor, Frequency [Hz] vs FFT')
y=fft(Q(:,exttempcol)-mean(Q(:,exttempcol)));
plot(f',abs(y(1:n)))
//Temperature from deltap sensor
subplot(3,2,4)
xtitle('Temp from Deltap Sensor ,Frequency [Hz] vs FFT')
y=fft(Q(:,tempdeltapcol)-mean(Q(:,tempdeltapcol)));
plot(f',abs(y(1:n)))
//Temperature from absolute pressure sensor
subplot(3,2,5)
xtitle('Temp from Abs Pressure Sensor ,Frequency [Hz] vs FFT')
y=fft(Q(:,tempabscol)-mean(Q(:,tempabscol)));
plot(f',abs(y(1:n)))

//Average of sample frequency
subplot(3,2,6)
xtitle('Average value of sample frequency vs #different data sets')
[r c]=size(fsamplev)
cla=1:c
plot(cla,fsamplev,'-')

//Time plot
//Need time
figtime=scf(2);
clf(2);
timetic=Q(:,17)-basetime
figtime.figure_name='Measurements and Calculations';
//Abs pressure [Pa]
subplot(4,3,1)
xtitle('Absolute Pressure Sensor [Pa] vs Time [ms]')
plot(timetic,Q(:,absmcol),2)
//IAS [m/s]
subplot(4,3,2)
xtitle('Indicated Air Speed [m/s] vs Time [ms]')
plot(timetic,Q(:,IASmcol),2)

//TAS [m/s]
subplot(4,3,3)
xtitle('True Air Speed [m/s] vs Time [ms]')
plot(timetic,Q(:,TASmcol),2)
//External temperature [°K]
subplot(4,3,3)
xtitle('External Temperature [°K] vs Time [ms]')
plot(timetic,Q(:,ExtTempmcol),2)
//Temperature deltapsensor [°K]
subplot(4,3,4)
xtitle('Temperature from deltap pressure sensor [°K] vs Time [ms]')
plot(timetic,Q(:,tempdeltapmcol),2)
//Temperature abspressure sensor [°K]
subplot(4,3,5)
xtitle('Temperature from absolute pressure sensor [°K] vs Time [ms]')
plot(timetic,Q(:,tempabsmcol),2)
//Altitude [m]
subplot(4,3,6)
xtitle('Altitude [m] vs Time [ms]')
plot(timetic,Q(:,altitude),2)
//Outside Air Temperature [°K]
subplot(4,3,7)
xtitle('Outside Air Temperature [°K] vs Time [ms]')
plot(timetic,Q(:,OAT),2)
//Air Density [kg/m^3]
subplot(4,3,8)
xtitle('Air Density [kg/m^3] vs Time [ms]')
plot(timetic,Q(:,rhoairmcol),2)
//Air Viscosity [mPas]
subplot(4,3,9)
xtitle('Air Viscosity [mPas] vs Time [ms]')
plot(timetic,Q(:,vair),2)
//Re
subplot(4,3,10)
xtitle('Reynolds vs Time [ms]')
plot(timetic,Q(:,Re),2)
//C factot
subplot(4,3,11)
xtitle('c factor vs Time [ms]')
plot(timetic,Q(:,cfactor),2)
//Compares ADC logged values with offline calculated ones
if debug==1 then
    //Calulates rhoair and add a one new colums to Q, contains (ADCrho-calculatedrho) 
    [r c]=size(Q);
    for ri=1:r
        Q(ri,c+1)=(Q(ri,rhoairmcol)-rhoair(Q(ri,ExtTempmcol), Q(ri,absmcol),0))/rhoair(Q(ri,ExtTempmcol), Q(ri,absmcol)*100)
    end
    //Plot calculated data
    figerrori=scf(3);
    clf(3);
    figerrori.figure_name='Absoulte differences betweeen logged ADC output and library calculated values';
    //Rhoair
    subplot(2,2,1)
    xtitle('Density of Air Difference % vs Time [ms]')
    plot(timetic,Q(:,c+1),2)
    if ((abs(min(Q(:,c+1)))<airdensityeband)&(abs(max(Q(:,c+1)))<airdensityeband)) then
        mprintf('\nADC calculated Air Density value is within the tollerance band of %f %% of density',airdensityeband)
    else
        mprintf('\nWarning: ADC calculated Air Density value is outside the tollerance band of %f %% of density',airdensityeband)
    end

    //Air viscosity check
    subplot(2,2,2)
    xtitle('Viscosity of Air Difference % vs Time [ms]')
    for ri=1:r
        Q(ri,c+2)=(Q(ri,vair)-viscosityair(Q(ri,exttempcol)))/viscosityair(Q(ri,exttempcol))*100
    end
    plot(timetic,Q(:,c+2),2)
    if ((abs(min(Q(:,c+2)))<viscosityeband)&(abs(max(Q(:,c+2)))<viscosityeband)) then
        mprintf('\nADC calculated Viscosity value is within the tollerance band of %f %% of viscosity',viscosityeband)
    else
        mprintf('\nWarning: ADC calculated Viscosity value is outside the tollerance band of %f %% of viscosity',viscosityeband)
    end
    //Altitude check
    subplot(2,2,3)
    xtitle('Altitude Difference % vs Time [ms]')
    for ri=1:r
        Q(ri,c+3)=(Q(ri,altitude)-ISAaltitude(Q(ri,absmcol)))/ISAaltitude(Q(ri,absmcol))*100
    end
    plot(timetic,Q(:,c+3),2)
    if ((abs(min(Q(:,c+3)))<altitudeeband)&(abs(max(Q(:,c+3)))<altitudeeband)) then
        mprintf('\nADC calculated altitude value is within the tollerance band of %f %% of viscosity',altitudeeband)
    else
        mprintf('\nWarning: ADC calculated altitude value is outside the tollerance band of %f %% of viscosity',altitudeeband)
    end
    
    
    
    
    
end
