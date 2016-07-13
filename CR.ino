
/*
 * Dispatch
 */

extern const byte PB;
extern const byte PG;
extern const byte PR;
extern const byte RI;
extern const byte BI;
extern const byte RD;
extern const byte GD;
extern const byte TD;
extern const byte ED;
extern const byte TI;
extern const byte EI;
extern const byte SPK;


typedef enum B_STATES { ST_WAIT, ST_PLAY, ST_WIN, ST_LOSS } b_state;
#define RED 0
#define GREEN 1
#define BLUE 2
#define WRONG 3


byte puzzle[] = { GREEN, BLUE, RED }; //, RED, GREEN, BLUE };
#define puzzle_size (sizeof(puzzle)/sizeof(puzzle[0]))

typedef struct STATE_INFO state_info;

typedef void (*state_handler)  (state_info *);
typedef bool (*state_pre_check)(state_info *);
typedef void (*state_init)     (state_info *);

typedef struct DISPATCH_INFO {
  b_state state;
  state_init    init;
  state_handler handler;
  state_init    quit;
  state_pre_check * pre_checks;
} dispatch_info;

struct STATE_INFO {
  b_state prev_state;
  b_state curr_state;

  long millis;      // Comienzo del estado

  /* wait */
  int tic;

  /* play */
  byte step;
};

void transition_to(b_state, state_info * info);

/* Externs */
extern void play_sound(byte color, int ms);

/* Precondiciones a chequear */
bool check_active (state_info * info) {
}

/*
 * Wait state
 */
void st_wait_init(state_info * info) {
  info->tic = 0;
}

void st_wait_handle(state_info * info){
  //Serial.println("wait handler");
  caleido(info->tic);
  info->tic++;
  for(int ii=0; ii<20; ii++) {
    if (check_activity()) {
      transition_to(ST_PLAY, info);
      return;
    }
    delay(50);
  }
}

void st_wait_quit(state_info * info) {
}

void leds_off() {
  dl_off();
  dr_off();
  p_off();
}

const int sh = 300;
void play_h(byte color, bool ok) {
  player_led(color);
  play_sound(ok?color:WRONG, sh);
  //delay(sh);
  dl_off();
  dr_off();
}

const int sd = 400;
void play_p(byte color) {
  switch(color) {
    case RED:
      p_red();
      break;
    case GREEN:
      p_green();
      break;
    case BLUE:
      p_blue();
      break;
    case WRONG:
      p_off();
      break; 
  }
  play_sound(color, sd);
  p_off();
}


const int dph = 500;
void play_ph(byte color_h, byte color_p, bool ok) {
  player_led(color_h);
  switch(color_p) {
    case RED:
      p_red();
      break;
    case GREEN:
      p_green();
      break;
    case BLUE:
      p_blue();
      break;
    case WRONG:
      p_off();
      break; 
  }
  play_sound(ok?color_h:WRONG, dph);
  dl_off();
  dr_off();
  p_off();
}


/*
 * Play state
 */

 
const int d1 = 300;
void st_play_init(state_info * info) {
  info->step = 0;
  leds_off();
/*  
  p_red();
  play_sound(RED, d1);
  delay(d1);
  p_green();
  play_sound(GREEN, d1);
  delay(d1);
  p_blue();
  play_sound(BLUE, d1);
  delay(d1);
*/  
}

void st_play_handle(state_info * info){
  /*
   * Muestro en pirana el color que va y el sound
   * Leo el control
   * si está Ok avanzo un step
   * Si no, wrong y salgo.
   */
  play_p(puzzle[info->step]);

  byte c = read_color(puzzle[info->step]);

  if (c == puzzle[info->step]) {
    play_ph(c, c, true);
    if(++(info->step) == puzzle_size) {
      transition_to(ST_WIN, info);
      return;
    }
    delay(400);
  } else {
    play_ph(c, puzzle[info->step], false);
    info->step = 0;
    transition_to(ST_WAIT, info);
    return;
  }
}

void st_play_handle2(state_info * info){
  /*
   * Muestro en pirana el color que va y el sound
   * Leo el control
   * si está Ok avanzo un step
   * Si no, wrong y salgo.
   */
  for(int ii=0; ii<=info->step; ii++) {
    play_p(puzzle[ii]);
    delay(300);
  }

  for(int ii=0; ii<=info->step; ii++) {
    byte c = read_color(ii);
  
    if (c == puzzle[ii]) {
      play_ph(c, c, true);
      delay(400);
    } else {
      play_ph(c, puzzle[info->step], false);
      info->step = 0;
      transition_to(ST_WAIT, info);
      delay(1000);
      return;
    }
  }

  if(++(info->step) == puzzle_size) {
    transition_to(ST_WIN, info);
    return;
  }
}

void st_play_quit(state_info * info) {
}

void st_win_init(state_info * info) {
}

void st_win_handle(state_info * info){
  p_green();
  delay(200);
  p_off();
  delay(200);
  if (read_color(RED) == RED) {
    transition_to(ST_PLAY, info);
    return;
  }
}

void st_win_quit(state_info * info) {
}



dispatch_info dispatch_info_table[] = {
  { ST_WAIT, st_wait_init, st_wait_handle, st_wait_quit, (state_pre_check[]){NULL} },
  { ST_PLAY, st_play_init, st_play_handle2, st_play_quit, (state_pre_check[]){NULL} },
  { ST_WIN, st_win_init, st_win_handle, st_win_quit, (state_pre_check[]){NULL} }
};


void transition_to(b_state state, state_info * info) {
  if (dispatch_info_table[info->curr_state].quit)
    dispatch_info_table[info->curr_state].quit(info);

  info->prev_state = info->curr_state;
  info->curr_state = state;
  info->millis = millis();

  if (dispatch_info_table[info->curr_state].init)
    dispatch_info_table[info->curr_state].init(info);
}

state_info info;

void reset() {
  game_reset();
  info.curr_state = ST_WAIT;
  transition_to(ST_WAIT, &info);
}

extern const byte PB;
extern const byte PG;
extern const byte PR;

void setup() {
  Serial.begin(115200);
  Serial.println("Begin");
  game_setup();
  reset();
  pinMode(47, OUTPUT);
}

void loop2() {
  Serial.println("Read Beg");
  byte c = read_color(RED);
  Serial.print("Color: ");
  Serial.println(c);
  delay(1000);
  /*
  tone(47, 440, 400);
  delay(1000);
  tone(47, 494, 400);
  delay(1000);
  tone(47, 554, 400);
  delay(1000);
  tone(47, 300, 400);
  delay(1000);
  tone(47, 200, 400);
  delay(1000);
  tone(47, 100, 400);
  delay(1000);
  */  
}

void loop3() {
  p_red();
  delay(1000);
  return;
  //byte k = read_color(RED);
  //return;
  long d = read_distance(TI, EI);
  Serial.print("Izq: ");
  Serial.println(d);
  delay(500);
  d = read_distance(TD, ED);
  Serial.print("Der: ");
  Serial.println(d);
  delay(500);
  return;
  dl_blue();
  dr_green();
}

void loop() {
  for(int ii=0; dispatch_info_table[info.curr_state].pre_checks[ii]; ii++)
    if (dispatch_info_table[info.curr_state].pre_checks[ii](&info))
      return; //transision
  dispatch_info_table[info.curr_state].handler(&info);
}


