String cur_state = "No gesture";

// indicates whether a gesture has been detected
bool start_counting = false;

  
const byte movements_per_state = 3;
// record of movements detected over time_to_process_state loops
String movements[movements_per_state];         
byte movements_idx = 0;

// time/#loops (after first gesture) allowed before outputting detected gestures (max# = movements_per_state)
int time_count = 0;
const int time_to_process_state = 210*movements_per_state;

#include <Arduino_APDS9960.h>

void setup_ir() {
  if(!Serial){
    Serial.begin(115200);
    while (!Serial);
  }

  if (!APDS.begin()) {
    Serial.println("Error initializing APDS9960 sensor!");
  }

  // for setGestureSensitivity(..) a value between 1 and 100 is required.
  // Higher values makes the gesture recognition more sensible but less accurate
  // (a wrong gesture may be detected). Lower values makes the gesture recognition
  // more accurate but less sensible (some gestures may be missed).
  // Default is 80
  //APDS.setGestureSensitivity(80);

  //Serial.println("Detecting gestures ...");
  Serial.println(cur_state);
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
            movements[movements_idx] = "B";   //back
            movements_idx += 1;
            // Serial.println("gesture back");
            start_counting = true;
            break;
    
          case GESTURE_DOWN:
            movements[movements_idx] = "F";   //front
            movements_idx += 1;
            // Serial.println("gesture frnt");
            start_counting = true;
            break;
    
          case GESTURE_LEFT:
            movements[movements_idx] = "U";   //up
            movements_idx += 1;
            // Serial.println("gesture up");
            start_counting = true;
            break;
    
          case GESTURE_RIGHT:
            movements[movements_idx] = "D";   //down
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
      cur_state = "No gesture";  
    } else {
      cur_state = movements[0];
      movements[0] = "";
      for (byte i = 1; i < movements_idx; i = i + 1) {
        cur_state.concat(" ");
        cur_state.concat(movements[i]);
        movements[i] = "";
      }
    }
    
    Serial.println(cur_state);
    IR_command = cur_state;
    IR_command_given = 1;
    
    cur_state = "No gesture";
    movements_idx = 0;
  }
  if (start_counting) {
    time_count += 1;
  }
}
