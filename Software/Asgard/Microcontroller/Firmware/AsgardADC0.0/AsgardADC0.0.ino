//Work in progress 19-04-2017
//It does not compile. We're focused on the core ADC code.
//Sketch to be uploaded to ADC Asgard

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  double pstatic, dp, temperature, iTAS, ip1TAS, res

  // put your main code here, to run repeatedly:
  //An object for each ADC is created
  AirDC *ptrAirDC;
  ptrAirDC = &AirDataComputer;

  //Data acquisition phase, dp, pstatic, and temperature variables are updated
  //with fresh readings from the sensor

  //Update ADC internal status with measurements data
  AirDataComputer._p = pstatic;
  AirDataComputer._qc = dp;
  AirDataComputer._RH = 0; //No sensor for RH, we are selecting dry air but the library will handle moist air if required
  AirDataComputer._TAT = temperature;//Total Air Temperature
  //Computation
  //Init
  AirDataComputer._T = temperature;
  AirDataComputer.RhoAir(1);// Calculates the air density
  AirDataComputer.Viscosity(2);// Calculates the dynamic viscosity, Algorithm 2 (UOM Pas1e-6)
  AirDataComputer.CalibrationFactor(1); //Calibration factor set to 1
  AirDataComputer.IAS(1); //Calculates IAS method 1
  AirDataComputer._TAS = AirDataComputer._IAS;
  AirDataComputer.Mach(1); //Calculates Mach No
  AirDataComputer.OAT(1); //Outside Air Temperature

  //Wild iteration
  iof = 1;
  while ((res > 0.005) || (iof < 100)) {
    AirDataComputer.RhoAir(1);// Calculates the air density
    AirDataComputer.Viscosity(2);// Calculates the dynamic viscosity, Algorithm 2 (UOM Pas1e-6)
    AirDataComputer.CalibrationFactor(2); //Update calibration fator vat at TAS
    AirDataComputer.IAS(1); //IAS
    AirDataComputer.CAS(1); //CAS
    AirDataComputer.TAS(1); //True Air Speed
    AirDataComputer.Mach(1); //Update Mach No
    iTAS = AirDataComputer._TAS; //Store TAS value
    AirDataComputer.OAT(1); //Update outside Air Temperature
    AirDataComputer.RhoAir(1);// Calculates the air density
    AirDataComputer.Viscosity(2);// Calculates the dynamic viscosity, Algorithm 2 (UOM Pas1e-6)
    AirDataComputer.CalibrationFactor(2); //Update calibration fator vat at TAS
    AirDataComputer.TAS(1); //Update TAS
    AirDataComputer.Mach(1); //Update Mach No
    AirDataComputer.OAT(1); //Update outside Air Temperature
    ip1TAS = AirDataComputer._TAS;
    res = abs(ip1TAS - iTAS) / iTAS;
    iof++;
  }
  //Uncorrected ISA Altitude _h
  AirDataComputer.ISAAltitude(1);

}
