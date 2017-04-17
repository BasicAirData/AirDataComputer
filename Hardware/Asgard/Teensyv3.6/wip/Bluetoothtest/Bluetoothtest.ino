/* Source code from https://forum.arduino.cc/index.php?topic=288234.0 
 * Changes by Basic Air Data
 */
const byte numChars = 32;
char receivedChars[numChars]; // an array to store the received data

boolean newData = false;

void setup() {
 Serial1.begin(9600);
 Serial1.println("BlueTooth module correctly hooked");
}

void loop() {
 recvWithEndMarker();
 showNewData();
}

void recvWithEndMarker() {
 static byte ndx = 0;
 char endMarker = '\n';
 char rc;
 
 // if (Serial.available() > 0) {
           while (Serial1.available() > 0 && newData == false) {
 rc = Serial1.read();

 if (rc != endMarker) {
 receivedChars[ndx] = rc;
 ndx++;
 if (ndx >= numChars) {
 ndx = numChars - 1;
 }
 }
 else {
 receivedChars[ndx] = '\0'; // terminate the string
 ndx = 0;
 newData = true;
 }
 }
}

void showNewData() {
 if (newData == true) {
 Serial.print("Incoming characters ... ");
 Serial.println(receivedChars);
 newData = false;
 }
}
