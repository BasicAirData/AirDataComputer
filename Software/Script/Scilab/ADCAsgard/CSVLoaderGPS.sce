//ADC Asgard CSV Loader. JLJ . Basicairdata
//1 debug mode 
clc
GPS = csvRead("PIPPO01GPS.CSV") //GPS data

ngroups=20;//Number of data sets
fsample=0; //Averaged value of fsample

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
//Define GPS data columns
lat=3
lon=4
GPSaltitude=6
geoidaltitude=7
GPSspeed=8
bearing=9; //Deg
//Define tollerance band
//Define tollerance band
airdensityeband=0.5; //Air Density Percent of rho
viscosityeband=3;//Viscosity %
altitudeeband=0.5;//Altitude %
fsamplev=zeros(1,ngroups);
figGPS=scf(1);
//clf(1);
//GPS track
scatter3(GPS(:,lat),GPS(:,lon),GPS(:,GPSaltitude),"*")  

