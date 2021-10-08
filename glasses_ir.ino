int NONE = -1;
int BACK = 1;
int FRONT = 2;
int UP = 3;
int DOWN = 4;

int cur_state = NONE;

// indicates whether a gesture has been detected
bool start_counting = false;

  
const byte movements_per_state = 3;
// record of movements detected over time_to_process_state loops
int movements[movements_per_state];         
byte movements_idx = 0;

// time/#loops (after first gesture) allowed before outputting detected gestures (max# = movements_per_state)
int time_count = 0;
const int time_to_process_state = 210*movements_per_state;

#include <Arduino_APDS9960.h>

void setup_ir() {
  #if DEBUG
  if(!Serial){
    Serial.begin(115200);
    // while (!Serial);
  }
  #endif

  if (!APDS.begin()) {
    #if DEBUG
    Serial.println("Error initializing APDS9960 sensor!");
    #endif
  }

  // for setGestureSensitivity(..) a value between 1 and 100 is required.
  // Higher values makes the gesture recognition more sensible but less accurate
  // (a wrong gesture may be detected). Lower values makes the gesture recognition
  // more accurate but less sensible (some gestures may be missed).
  // Default is 80
  //APDS.setGestureSensitivity(80);

  //Serial.println("Detecting gestures ...");
  #if DEBUG
  Serial.println(String(cur_state));
  #endif
  IR_command_given = 0;
}

void end_ir() {
  APDS.end();
}

void IR_gesture_check() {
  if (movements_idx < movements_per_state) {
      if (APDS.gestureAvailable()) {
        // a gesture was detected, read and print to serial monitor
        int gesture = APDS.readGesture();
        
        switch (gesture) {
          case GESTURE_UP:
            movements[movements_idx] = BACK; //"B";   //back
            movements_idx += 1;
            // Serial.println("gesture back");
            start_counting = true;
            break;
    
          case GESTURE_DOWN:
            movements[movements_idx] = FRONT;//"F";   //front
            movements_idx += 1;
            // Serial.println("gesture frnt");
            start_counting = true;
            break;
    
          case GESTURE_LEFT:
            movements[movements_idx] = UP;//"U";   //up
            movements_idx += 1;
            // Serial.println("gesture up");
            start_counting = true;
            break;
    
          case GESTURE_RIGHT:
            movements[movements_idx] = DOWN;//"D";   //down
            movements_idx += 1;
            // Serial.println("gesture down");
            start_counting = true;
            break;
    
          default:
            // ignore
            // Serial.println("no gesture");
            break;
        }
    }
  }
  // Up to (time_to_process_state-1) loops after we detect the first gesture, we print whatever movements (max amt: movements_per_state) we sensed => state/command indicated
  if (time_count == time_to_process_state || movements_idx == movements_per_state) {
    time_count = 0;
    start_counting = false;
    
    if (movements_idx == 0) {
      cur_state = NONE; //"No gesture";  
    } else {
      cur_state = movements[0];
      movements[0] = NONE;
      for (byte i = 1; i < movements_idx; i = i + 1) {
        cur_state *= 10;
        cur_state += movements[i];
        movements[i] = NONE;
        //cur_state.concat(" ");
        //cur_state.concat(movements[i]);
        //movements[i] = "";
      }
    }
    
    #if DEBUG
    Serial.println(cur_state);
    #endif
    IR_command = cur_state;
    IR_command_given = 1;
    
    cur_state = NONE; //"No gesture";
    movements_idx = 0;
  }
  if (start_counting) {
    time_count += 1;
  }
}
