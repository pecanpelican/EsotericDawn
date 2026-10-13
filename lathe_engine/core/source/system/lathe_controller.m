
#include "system/lathe_controller.h"
#import <GameController/GameController.h>
#include "system/lathe_logger.h"

bool controller_connected(void) {
  [[NSRunLoop currentRunLoop] //need to fix...
      runUntilDate:[NSDate dateWithTimeIntervalSinceNow:0.1]];
  return GCController.controllers.count > 0;
}

lathe_controller_state controller_get_state(void) {
  lathe_controller_state state = {0};

  GCController *controller = GCController.current;

  if (controller == nil){
    //How do you even get here
    LATHE_ERROR("Controller was not found?",stderr);
    return state;
  }

  GCExtendedGamepad *pad = controller.extendedGamepad;

  if (pad == nil){
    //Currently not supporting these odd controllers, should support most modern ones
    LATHE_ERROR("The Controller does not support the extended gamepad profile!",stderr);
    return state;
  }
    

  state.left_x = pad.leftThumbstick.xAxis.value;
  state.left_y = pad.leftThumbstick.yAxis.value;

  state.right_x = pad.rightThumbstick.xAxis.value;
  state.right_y = pad.rightThumbstick.yAxis.value;

  state.left_trigger = pad.leftTrigger.value;
  state.right_trigger = pad.rightTrigger.value;

  state.a_button = pad.buttonA.isPressed;
  state.b_button = pad.buttonB.isPressed;
  state.x_button = pad.buttonX.isPressed;
  state.y_button = pad.buttonY.isPressed;

  state.dpad_up = pad.dpad.up.isPressed;
  state.dpad_down = pad.dpad.down.isPressed;
  state.dpad_left = pad.dpad.left.isPressed;
  state.dpad_right = pad.dpad.right.isPressed;

  return state;
}

#ifdef LATHE_DEBUG_ENABLED
bool controller_print_buttons(void) {
  [[NSRunLoop currentRunLoop] runUntilDate:[NSDate dateWithTimeIntervalSinceNow:0.01]];//Gotta fix this...

  GCController *controller = GCController.current;

  if (controller == nil)
    return false;

  GCExtendedGamepad *pad = controller.extendedGamepad;

  if (pad == nil)
    return false;

  if (pad.buttonA.isPressed)
    LATHE_DEBUG("A pressed",stderr);
  if (pad.buttonB.isPressed)
    LATHE_DEBUG("B pressed",stderr);
  if (pad.buttonX.isPressed)
    LATHE_DEBUG("X pressed",stderr);
  if (pad.buttonY.isPressed)
    LATHE_DEBUG("Y pressed",stderr);

  if (pad.leftShoulder.isPressed)
    LATHE_DEBUG("L1 pressed",stderr);
  if (pad.rightShoulder.isPressed)
    LATHE_DEBUG("R1 pressed",stderr);

  if (pad.leftTrigger.isPressed)
    LATHE_DEBUG("L2 pressed",stderr);
  if (pad.rightTrigger.isPressed)
    LATHE_DEBUG("R2 pressed",stderr);

  if (pad.dpad.up.isPressed)
    LATHE_DEBUG("D-Pad Up pressed",stderr);
  if (pad.dpad.down.isPressed)
    LATHE_DEBUG("D-Pad Down pressed",stderr);
  if (pad.dpad.left.isPressed)
    LATHE_DEBUG("D-Pad Left pressed",stderr);
  if (pad.dpad.right.isPressed)
    LATHE_DEBUG("D-Pad Right pressed",stderr);
    
  if(pad.buttonMenu.isPressed){
      LATHE_DEBUG("Pause pressed",stderr);
      return false;//Done with test
  }
  return true;
}
#else
bool controller_print_buttons(void) {return false;}
#endif


