//Zerodp.sce Calculate zerodpvalue that is the true sensor zero reading
//ADC Asgard CSV Loader. JLJ . Basicairdata
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
viscosityeband=2;//Viscosity %
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
//Deltap
subplot(2,2,1)
xtitle('Deltape Sensor, Frequency [Hz] vs FFT')
y=fft(Q(:,deltapcol)-mean(Q(:,deltapcol)));
plot(f',abs(y(1:n)))
//External Temperature
subplot(2,2,2)
xtitle('External Temp Sensor, Frequency [Hz] vs FFT')
y=fft(Q(:,exttempcol)-mean(Q(:,exttempcol)));
plot(f',abs(y(1:n)))
//Temperature from deltap sensor
subplot(2,2,3)
xtitle('Temp from Deltap Sensor ,Frequency [Hz] vs FFT')
y=fft(Q(:,tempdeltapcol)-mean(Q(:,tempdeltapcol)));
plot(f',abs(y(1:n)))
//Average of sample frequency
subplot(2,2,4)
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
//IAS [m/s]
subplot(3,2,1)
xtitle('Indicated Air Speed [m/s] vs Time [ms]')
plot(timetic,Q(:,IASmcol),2)
//TAS [m/s]
subplot(3,2,2)
xtitle('True Air Speed [m/s] vs Time [ms]')
plot(timetic,Q(:,TASmcol),2)
//External temperature [°K]
subplot(3,2,3)
xtitle('External Temperature [°K] vs Time [ms]')
plot(timetic,Q(:,ExtTempmcol),2)
//Temperature deltapsensor [°K]
subplot(3,2,4)
xtitle('Temperature from deltap pressure sensor [°K] vs Time [ms]')
plot(timetic,Q(:,tempdeltapmcol),2)
subplot(3,2,5)
xtitle('Deltap sensor counts vs Time [ms]')
plot(timetic,Q(:,deltapcol),2)

//Time plot compensated measurement
//8192 is the nominal zero for the sensor
//1 psi to 6894.75729
csensor=6894.75729/8191
zerodatasheet=8192;
zerodpvalue=zerodatasheet-mean(Q(:,deltapcol))
compdeltap=Q(:,deltapcol)+zerodpvalue
figtime=scf(3);
clf(3);
//Raw from sensor
subplot(3,2,1)
xtitle('Deltap sensor counts vs Time [ms]')
plot(timetic,Q(:,deltapcol),2)
//Compensated sensor
subplot(3,2,2)
xtitle('Conditioned deltap sensor counts vs Time [ms]')
plot(timetic,Q(:,deltapcol)+zerodpvalue,2)
//Non compensated IAS [m/s]
subplot(3,2,3)
xtitle('Non conditioned Indicated Air Speed [m/s] vs Time [ms]')
[r c]=size(Q);
for no=1:r
ISAnocond(no)=sign(Q(no,deltapcol)-zerodatasheet)*(2*(abs(Q(no,deltapcol)-zerodatasheet)*csensor/1.225)^0.5);
end
plot(timetic,ISAnocond,2)
//Compensated IAS [m/s]
//8192 is the nominal zero for the sensor
//1 psi to 6894.75729
[r c]=size(Q);
for no=1:r
IAScond(no)=sign(compdeltap(no)-zerodatasheet)*(2*(abs(compdeltap(no)-zerodatasheet)*csensor/1.225)^0.5);
end
subplot(3,2,4)
xtitle('Conditioned Indicated Air Speed [m/s] vs Time [ms]')
plot(timetic,IAScond,2)
//Oversampled output , with oversampled time
nosamples=10;
noover=1;
for nap=1:nosamples:(r-nosamples)
   somme=0;
   for iz=0:(nosamples-1)
   somme=somme+IAScond(nap+nosamples)
   end
   IASoversampled(noover)=somme/nosamples
   timeoversampled(noover)=timetic(nap)
   noover=noover+1
end
subplot(3,2,6)
xtitle('Oversampled Conditioned Indicated Air Speed [m/s] vs Time [ms]')
plot(timeoversampled,IASoversampled,2)



