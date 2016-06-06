
/*
 * IZQ: Echo violeta
 * IZQ: Trig gris
 * Der: Echo
 * Der: Trig
 */

const byte PB = 41;
const byte PG = 40;
const byte PR = 42;
const byte RI = 45;
const byte BI = 44;
const byte RD = 43;
const byte GD = 38;
const byte TD = 33;
const byte ED = 32;
const byte TI = 31;
const byte EI = 30;
const byte SPK = 47;

int read_distance(byte trigPin, byte echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);  // Reads the echoPin, returns the sound wave travel time in microseconds
  long duration = pulseIn(echoPin, HIGH);
  return (int)(duration>>9);
  // Calculating the distance
  //return duration*0.034/2;
}

void dl_red() {
  digitalWrite(RI, LOW);
  digitalWrite(BI, HIGH);
}

void dl_blue() {
  digitalWrite(RI, HIGH);
  digitalWrite(BI, LOW);
}

void dl_off() {
  digitalWrite(RI, HIGH);
  digitalWrite(BI, HIGH);
}

void dr_red() {
  digitalWrite(RD, LOW);
  digitalWrite(GD, HIGH);
}

void dr_green() {
  digitalWrite(RD, HIGH);
  digitalWrite(GD, LOW);
}

void dr_off() {
  digitalWrite(RD, HIGH);
  digitalWrite(GD, HIGH);
}

bool check_combination(int imin, int imax, int dmin, int dmax, int n_times, int m_times) {
  int t = 0;
  int tok = 0;

  while (t++ < m_times) {
    long li = read_distance(TI, EI);
    long ld = read_distance(TD, ED);

    if (li >= imin && li <= imax && ld >= dmin && ld <= dmax) {
      tok++;
      if (tok >= n_times)
        return true;
    } else {
      tok = 0;
    }
    /* Actualizo color aca?? */
    if (li >= imin && li <= imax && ld >= dmin && ld <= dmax) {
      dl_red();
      dr_red();
    } else if (li >= imin && li <= imax) {
      dl_blue();
      dr_off();
    } else if (ld >= dmin && ld <= dmax) {
      dl_off();
      dr_green();
    } else {
      dl_off();
      dr_off();
    }

    delay(50);
  }
  return false;
}

long buckets[]            = { 9999999l, 40,30,20,10 };
#define STEPS (sizeof(buckets)/sizeof(buckets[0]))
byte read_distance_level(byte trigPin, byte echoPin) {
  long d = read_distance(trigPin, echoPin);
  // Serial.println(d);
  int l;
  for (l=STEPS-1; l >= 0; l--) {
    if (buckets[l] > d) break;
  }
  return l; 
}


void setup() {
  Serial.begin(9600);
  pinMode(PB, OUTPUT);
  pinMode(PG, OUTPUT);
  pinMode(PR, OUTPUT);
  pinMode(RI, OUTPUT);
  pinMode(BI, OUTPUT);
  pinMode(RD, OUTPUT);
  pinMode(GD, OUTPUT);
  pinMode(SPK, OUTPUT);
  pinMode(TD, OUTPUT);
  pinMode(TI, OUTPUT);
  pinMode(ED, INPUT);
  pinMode(EI, INPUT);
  digitalWrite(PB, HIGH);
  digitalWrite(PG, HIGH);
  digitalWrite(PR, HIGH);
  digitalWrite(RI, HIGH);
  digitalWrite(BI, HIGH);
  digitalWrite(RD, HIGH);
  digitalWrite(GD, HIGH);
  digitalWrite(SPK, LOW);
  
}

void loop() {
  bool r = check_combination(2, 4, 2, 4, 10, 20);
  //Serial.println(r ? "OK" : "NO");
  if (r) {
    //tone(SPK, 1500);    
  } else {
    //tone(SPK, 500);
  }

  //delay(500);
}
