
  
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  Serial.println("Hello, world!");
  Serial.print("sizeof(int) = "); Serial.print(sizeof(int)); Serial.println("");
  Serial.print("sizeof(void*) = "); Serial.print(sizeof(void*)); Serial.println("");

  pinMode(PD3, INPUT);
  pinMode(A5, INPUT);
  pinMode(A0, INPUT);
}

uint8_t prev_pd3 = LOW;

void loop() {
  // put your main code here, to run repeatedly:
  uint8_t pd3 = digitalRead(PD3);
  if (pd3 && !prev_pd3) {
    //Serial.println("button activated");
    Serial.print("A5 = "); Serial.println(analogRead(A5));    
    Serial.print("A0 = "); Serial.println(analogRead(A0));
  }
  prev_pd3 = pd3;
}
