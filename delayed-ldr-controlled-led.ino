
  
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  Serial.println("Hello, world!");
  Serial.print("sizeof(int) = "); Serial.print(sizeof(int)); Serial.println("");
  Serial.print("sizeof(void*) = "); Serial.print(sizeof(void*)); Serial.println("");

  pinMode(PD3, INPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  delay(100);
}
