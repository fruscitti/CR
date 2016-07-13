

/*
 * IZQ: Echo violeta
 * IZQ: Trig gris
 * Der: Echo
 * Der: Trig
 */
const byte PB = 9;
const byte PG = 8;
const byte PR = 12;
const byte RI = 5;
const byte BI = 4;
const byte RD = 7;
const byte GD = 6;
const byte TD = 41;
const byte ED = 40;
const byte TI = 22;
const byte EI = 23;
const byte SPK = 24;

const int r_freq = 440;
const int g_freq = 494;
const int b_freq = 554;
const int w_freq = 100;

const int usLL = 1;
const int usUL = 3;

int sound_freqs[] = {r_freq, g_freq, b_freq, w_freq}; 

long read_distance(byte trigPin, byte echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);  // Reads the echoPin, returns the sound wave travel time in microseconds
  long duration = pulseIn(echoPin, HIGH);
  if (duration < 0) duration = 0;
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

void p_red() {
  digitalWrite(PR, LOW);
  digitalWrite(PG, HIGH);
  digitalWrite(PB, HIGH);  
}

void p_blue() {
  digitalWrite(PB, LOW);
  digitalWrite(PG, HIGH);
  digitalWrite(PR, HIGH);  
}

void p_green() {
  digitalWrite(PG, LOW);
  digitalWrite(PR, HIGH);
  digitalWrite(PB, HIGH);  
}

void p_off() {
  digitalWrite(PR, HIGH);
  digitalWrite(PG, HIGH);
  digitalWrite(PB, HIGH);  
}

void play_sound(byte color, int ms) {
  tone(SPK, sound_freqs[color], ms);
  delay(ms);
}

void player_led(byte color) {
  switch(color) {
    case RED:
      dl_red();
      dr_red();
      break;
    case GREEN:
      dl_off();
      dr_green();
      break;
    case BLUE:
      dl_blue();
      dr_off();
      break;
    case WRONG:
      dl_off();
      dr_off();
      break; 
  }
}

bool check_activity() {
  long li = read_distance(TI, EI);
  long ld = read_distance(TD, ED);

  if ((li >= usLL && li <= usUL) || (ld >= usLL && ld <= usUL))
    return true;
    
  return false;
}

bool isBlue(int li, int ld) {
  return (li >= usLL && li <= usUL && (ld < usLL || ld > usUL));
}

bool isRed(int li, int ld) {
  return (li >= usLL && li <= usUL && ld >= usLL && ld <= usUL);
}

bool isGreen(int li, int ld) {
  return (ld >= usLL && ld <= usUL && (li < usLL || li > usUL));
}

const int read_interval = 30; // 40 ms
const int read_times = 1000 / read_interval; // Durante 1 segundo
const int read_ok = read_times / 2; // Mitad Ok

byte read_samples[read_times];

byte read_color(byte color) {
  int read_index = 0;
  int rc = WRONG;
  int t = 0;

  while (t < read_times) {
    int ld = read_distance(TD, ED);
    int li = read_distance(TI, EI);

    /*
    Serial.print("Li: ");
    Serial.print(li);
    Serial.print(", Ld: ");
    Serial.println(ld);
    */
    
    byte lrc = WRONG;
    if (isRed(li, ld)) lrc = RED;
    else if (isBlue(li, ld)) lrc = BLUE;
    else if (isGreen(li, ld)) lrc = GREEN;

    read_samples[t] = lrc;
    player_led(lrc);
    
    if (t >= read_ok-1) {
      int ok_count = 0;
      for(int ii=t-read_ok+1; ii<=t; ii++) {
        if (read_samples[ii] == color) ok_count++; 
      }
      if (ok_count >= read_ok-1) {
        Serial.println("Llego!!!!!!!!!!!!!!!!!");
        return color;
      }
    }
    
    t++;
    delay(read_interval);
  }

  // No llego al color real, cuento cual gana y retorno ese
  Serial.println("No llego");
  int cc[4];
  for(int ii=0; ii<4; ii++)
    cc[ii] = 0;

  for(int ii=0; ii<t; ii++)
    cc[read_samples[ii]]++;

  if(cc[RED] > 0 && cc[RED] > cc[GREEN] && cc[RED] > cc[BLUE]) return RED;
  if(cc[GREEN] > 0 && cc[GREEN] > cc[RED] && cc[GREEN] > cc[BLUE]) return GREEN;
  if(cc[BLUE] > 0 && cc[BLUE] > cc[RED] && cc[BLUE] > cc[GREEN]) return BLUE;
  return WRONG;
}


void caleido(int tic) {
  switch(tic % 2) {
    case 0:
      dl_blue();
      dr_green();
      break;
    case 1:
      dl_red();
      dr_red();
  };

  switch(tic % 3) {
    case 0:
      p_red();
      break;
    case 1:
      p_blue();
      break;
    case 2:
      p_green();
      break;
  } 
}

void game_reset() {
  digitalWrite(PB, HIGH);
  digitalWrite(PG, HIGH);
  digitalWrite(PR, HIGH);
  digitalWrite(RI, HIGH);
  digitalWrite(BI, HIGH);
  digitalWrite(RD, HIGH);
  digitalWrite(GD, HIGH);
  digitalWrite(SPK, LOW);
}

void game_setup() {
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
}


/*
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
*/
