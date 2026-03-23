#define CHANNEL_1 1
#define CHANNEL_2 2
#define CHANNEL_3 3
#define CHANNEL_4 4
#define CHANNEL_5 5 
#define CHANNEL_6 6

void init_pins() {
  pinMode(CHANNEL_1, OUTPUT);
  pinMode(CHANNEL_2, OUTPUT);
  pinMode(CHANNEL_3, OUTPUT);
  pinMode(CHANNEL_4, OUTPUT);
  pinMode(CHANNEL_5, OUTPUT);
  pinMode(CHANNEL_6, OUTPUT);

  digitalWrite(CHANNEL_1, LOW);
  digitalWrite(CHANNEL_2, LOW);
  digitalWrite(CHANNEL_3, LOW);
  digitalWrite(CHANNEL_4, LOW);
  digitalWrite(CHANNEL_5, LOW);
  digitalWrite(CHANNEL_6, LOW);
}

void setup() {
  init_pins();
}

void loop() {

}