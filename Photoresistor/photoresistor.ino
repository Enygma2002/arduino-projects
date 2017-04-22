/* Use a photoresistor (or photocell) to turn on an LED in the dark
   More info and circuit schematic: http://www.ardumotive.com/how-to-use-a-photoresistor-en.html
   Dev: Michalis Vasilakis // Date: 8/6/2015 // www.ardumotive.com */

// My Setup:
//   5V->Photoresistor->-+-->A7
//                       \->10K R->GND

//Constants
const int pResistor = A7; // Photoresistor at Arduino analog pin A7

//Variables
int value;          // Store value from photoresistor (0-1023)

void setup() {
  pinMode(pResistor, INPUT);// Set pResistor - A7 pin as an input (optional)
  Serial.begin(9600);
}

void loop() {
  value = analogRead(pResistor);

  Serial.print("Light: ");
  Serial.println(value);

  delay(500); //Small delay
}
