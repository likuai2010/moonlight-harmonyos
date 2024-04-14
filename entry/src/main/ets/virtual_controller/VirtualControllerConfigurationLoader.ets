import { AnalogStick, AnalogStickListener } from './AnalogStick';
import { VirtualControllerElement } from './common';
import { ControllerPacket } from './ControllerPacket';
import { DigitalButton, DigitalButtonListener } from './DigitalButton';
import { DigitalPad, DIGITAL_PAD_DIRECTION_DOWN, DIGITAL_PAD_DIRECTION_LEFT, DIGITAL_PAD_DIRECTION_RIGHT, DIGITAL_PAD_DIRECTION_UP } from './DigitalPad';
import { VirtualController } from './VirtualController';
import dataPreferences from '@ohos.data.preferences';
import display from '@ohos.display';
import { StreamSettings } from '../uitls/StreamSetttings';


const TRIGGER_L_BASE_X: number = 1;
const TRIGGER_R_BASE_X: number = 92;
const TRIGGER_DISTANCE: number = 23;
const TRIGGER_BASE_Y: number = 31;
const TRIGGER_WIDTH: number = 12;
const TRIGGER_HEIGHT: number = 9;

// Face buttons are defined based on the Y button (button number 9)
const BUTTON_BASE_X: number = 106;
const BUTTON_BASE_Y: number = 1;
const BUTTON_SIZE: number = 10;

const DPAD_BASE_X: number = 4;
const DPAD_BASE_Y: number = 41;
const DPAD_SIZE: number = 30;

const ANALOG_L_BASE_X: number = 6;
const ANALOG_L_BASE_Y: number = 4;
const ANALOG_R_BASE_X: number = 98;
const ANALOG_R_BASE_Y: number = 42;
const ANALOG_SIZE: number = 26;

const L3_R3_BASE_Y: number = 60;

const START_X: number = 83;
const BACK_X: number = 34;
const START_BACK_Y: number = 64;
const START_BACK_WIDTH: number = 12;
const START_BACK_HEIGHT: number = 7;
const OSC_PREFERENCE: string = "OSC";


function screenScale(units: number, height: number) {
  return ((height / 72) * units);
}


function createDigitalButton(
    elementId: number,
    keyShort: number,
    keyLong: number,
    layer: number,
    text: string,
    icon: number,
    controller: VirtualController,
): DigitalButton {
  const button = new DigitalButton(controller, elementId, layer);
  button.text = text
  const listener: DigitalButtonListener = {
    onClick: () => {
      const inputContext = controller.inputContext;
      inputContext.inputMap |= keyShort;
      controller.sendControllerInputContext();
    },
    onLongClick: () => {
      const inputContext = controller.inputContext;
      inputContext.inputMap |= keyLong;
      controller.sendControllerInputContext();
    },
    onRelease: () => {
      const inputContext = controller.inputContext;
      inputContext.inputMap &= ~keyShort;
      inputContext.inputMap &= ~keyLong;
      controller.sendControllerInputContext();
    },
  }
  button.addDigitalButtonListener(listener);

  return button;
}

function createLeftTrigger(
    layer: number,
    text: string,
    icon: number,
    controller: VirtualController,
): DigitalButton {
  const button = new DigitalButton(controller, VirtualControllerElement.EID_LT, layer);
  button.text = text;
  let listener:DigitalButtonListener = {
    onClick: () => {
      const inputContext = controller.inputContext;
      inputContext.leftTrigger = 0xFF;
      controller.sendControllerInputContext();
    },
    onLongClick: () => {
    },
    onRelease: () => {
      const inputContext = controller.inputContext;
      inputContext.leftTrigger = 0x00;
      controller.sendControllerInputContext();
    }
  }
  button.addDigitalButtonListener(listener);
  return button;
}

function createRightTrigger(
    layer: number,
    text: string,
    icon: number,
    controller: VirtualController,
): DigitalButton {
  const button = new DigitalButton(controller, VirtualControllerElement.EID_RT, layer);
  button.text = text
  const listener :DigitalButtonListener = {
    onClick() {
      const inputContext = controller.inputContext;
      inputContext.rightTrigger = 0xFF;
      controller.sendControllerInputContext();
    },
    onLongClick() {
    },
    onRelease() {
      const inputContext = controller.inputContext;
      inputContext.rightTrigger = 0x00;
      controller.sendControllerInputContext();
    }
  }
  button.addDigitalButtonListener(listener);
  return button;
}

export function createLeftStick(controller: VirtualController): AnalogStick {
  const as = new AnalogStick(controller, VirtualControllerElement.EID_LS);
  const listener: AnalogStickListener = {
    onMovement: (x: number, y: number) => {
      const inputContext = controller.inputContext;
      inputContext.leftStickX = Math.round(x * 0x7FFE);
      inputContext.leftStickY = Math.round(y * 0x7FFE);
      controller.sendControllerInputContext();
    },
    onClick: () => {
      // Implement onClick logic if needed
    },
    onDoubleClick: () => {
      const inputContext = controller.inputContext;
      inputContext.inputMap |= ControllerPacket.LS_CLK_FLAG;

      controller.sendControllerInputContext();
    },
    onRevoke: () => {
      const inputContext = controller.inputContext;
      inputContext.inputMap &= ~ControllerPacket.LS_CLK_FLAG;
      controller.sendControllerInputContext();
    },
  }
  as.addAnalogStickListener(listener);
  return as;
}

function createRightStick(controller: VirtualController): AnalogStick {
  const as = new AnalogStick(controller, VirtualControllerElement.EID_RS);
  const listener: AnalogStickListener = {
    onMovement: (x: number, y: number) => {
      const inputContext = controller.inputContext;
      inputContext.rightStickX = Math.round(x * 0x7FFE);
      inputContext.rightStickY = Math.round(y * 0x7FFE);

      controller.sendControllerInputContext();
    },
    onClick: () => {
      // Implement onClick logic if needed
    },
    onDoubleClick: () => {
      const inputContext = controller.inputContext;
      inputContext.inputMap |= ControllerPacket.RS_CLK_FLAG;

      controller.sendControllerInputContext();
    },
    onRevoke: () => {
      const inputContext = controller.inputContext;
      inputContext.inputMap &= ~ControllerPacket.RS_CLK_FLAG;

      controller.sendControllerInputContext();
    },
  }
  as.addAnalogStickListener(listener);
  return as;
}

function createDigitalPad(controller: VirtualController,): DigitalPad {
  const digitalPad = new DigitalPad(controller, VirtualControllerElement.EID_DPAD);
  digitalPad.addDigitalPadListener({
    onDirectionChange: (direction: number) => {
      const inputContext = controller.inputContext;

      if ((direction & DIGITAL_PAD_DIRECTION_LEFT) !== 0) {
        inputContext.inputMap |= ControllerPacket.LEFT_FLAG;
      } else {
        inputContext.inputMap &= ~ControllerPacket.LEFT_FLAG;
      }
      if ((direction & DIGITAL_PAD_DIRECTION_RIGHT) !== 0) {
        inputContext.inputMap |= ControllerPacket.RIGHT_FLAG;
      } else {
        inputContext.inputMap &= ~ControllerPacket.RIGHT_FLAG;
      }
      if ((direction & DIGITAL_PAD_DIRECTION_UP) !== 0) {
        inputContext.inputMap |= ControllerPacket.UP_FLAG;
      } else {
        inputContext.inputMap &= ~ControllerPacket.UP_FLAG;
      }
      if ((direction & DIGITAL_PAD_DIRECTION_DOWN) !== 0) {
        inputContext.inputMap |= ControllerPacket.DOWN_FLAG;
      } else {
        inputContext.inputMap &= ~ControllerPacket.DOWN_FLAG;
      }
      controller.sendControllerInputContext();
    },
  });

  return digitalPad;
}
interface ScreenWH{
  widthPixels:number
  heightPixels:number
}

export class VirtualControllerConfigurationLoader {
  createDefaultLayout(controller: VirtualController, config: StreamSettings | undefined = undefined) {
    const defaultDisplay =  display.getDefaultDisplaySync()

    const screen : ScreenWH = { widthPixels: defaultDisplay.width / defaultDisplay.densityPixels, heightPixels: defaultDisplay.height / defaultDisplay.densityPixels }
    // Displace controls on the right by this amount of pixels to account for different aspect ratios
    const rightDisplacement = screen.widthPixels - screen.heightPixels * 16 / 9;

    const height = screen.heightPixels;
    // NOTE: Some of these getPercent() expressions seem like they can be combined
    // into a single call. Due to floating point rounding, this isn't actually possible.

    if (!config || !config.only_l3r3) {
      controller.addElement(createDigitalPad(controller),
        screenScale(DPAD_BASE_X, height),
        screenScale(DPAD_BASE_Y, height),
        screenScale(DPAD_SIZE, height),
        screenScale(DPAD_SIZE, height)
      );

      controller.addElement(createDigitalButton(
        VirtualControllerElement.EID_A,
          !config?.flip_face_buttons ? ControllerPacket.A_FLAG : ControllerPacket.B_FLAG, 0, 1,
          !config?.flip_face_buttons ? "A" : "B", -1, controller),
        screenScale(BUTTON_BASE_X, height) + rightDisplacement,
        screenScale(BUTTON_BASE_Y + 2 * BUTTON_SIZE, height),
        screenScale(BUTTON_SIZE, height),
        screenScale(BUTTON_SIZE, height)
      );

      controller.addElement(createDigitalButton(
        VirtualControllerElement.EID_B,
        config?.flip_face_buttons ? ControllerPacket.A_FLAG : ControllerPacket.B_FLAG, 0, 1,
        config?.flip_face_buttons ? "A" : "B", -1, controller),
        screenScale(BUTTON_BASE_X + BUTTON_SIZE, height) + rightDisplacement,
        screenScale(BUTTON_BASE_Y + BUTTON_SIZE, height),
        screenScale(BUTTON_SIZE, height),
        screenScale(BUTTON_SIZE, height)
      );

      controller.addElement(createDigitalButton(
        VirtualControllerElement.EID_X,
          !config?.flip_face_buttons ? ControllerPacket.X_FLAG : ControllerPacket.Y_FLAG, 0, 1,
          !config?.flip_face_buttons ? "X" : "Y", -1, controller),
        screenScale(BUTTON_BASE_X - BUTTON_SIZE, height) + rightDisplacement,
        screenScale(BUTTON_BASE_Y + BUTTON_SIZE, height),
        screenScale(BUTTON_SIZE, height),
        screenScale(BUTTON_SIZE, height)
      );

      controller.addElement(createDigitalButton(
        VirtualControllerElement.EID_Y,
        config?.flip_face_buttons ? ControllerPacket.X_FLAG : ControllerPacket.Y_FLAG, 0, 1,
        config?.flip_face_buttons ? "X" : "Y", -1, controller),
        screenScale(BUTTON_BASE_X, height) + rightDisplacement,
        screenScale(BUTTON_BASE_Y, height),
        screenScale(BUTTON_SIZE, height),
        screenScale(BUTTON_SIZE, height)
      );

      controller.addElement(createLeftTrigger(1, "LT", -1, controller),
        screenScale(TRIGGER_L_BASE_X, height),
        screenScale(TRIGGER_BASE_Y, height),
        screenScale(TRIGGER_WIDTH, height),
        screenScale(TRIGGER_HEIGHT, height)
      );

      controller.addElement(createRightTrigger( 1, "RT", -1, controller),
        screenScale(TRIGGER_R_BASE_X + TRIGGER_DISTANCE, height) + rightDisplacement,
        screenScale(TRIGGER_BASE_Y, height),
        screenScale(TRIGGER_WIDTH, height),
        screenScale(TRIGGER_HEIGHT, height)
      );

      controller.addElement(createDigitalButton(
        VirtualControllerElement.EID_LB,
        ControllerPacket.LB_FLAG, 0, 1, "LB", -1, controller),
        screenScale(TRIGGER_L_BASE_X + TRIGGER_DISTANCE, height),
        screenScale(TRIGGER_BASE_Y, height),
        screenScale(TRIGGER_WIDTH, height),
        screenScale(TRIGGER_HEIGHT, height)
      );

      controller.addElement(createDigitalButton(
        VirtualControllerElement.EID_RB,
        ControllerPacket.RB_FLAG, 0, 1, "RB", -1, controller),
        screenScale(TRIGGER_R_BASE_X, height) + rightDisplacement,
        screenScale(TRIGGER_BASE_Y, height),
        screenScale(TRIGGER_WIDTH, height),
        screenScale(TRIGGER_HEIGHT, height)
      );
      controller.addElement(createRightStick(controller),
        screenScale(ANALOG_R_BASE_X, height) + rightDisplacement,
        screenScale(ANALOG_R_BASE_Y, height),
        screenScale(ANALOG_SIZE, height),
        screenScale(ANALOG_SIZE, height)
      );

      controller.addElement(createLeftStick(controller),
        screenScale(ANALOG_L_BASE_X, height),
        screenScale(ANALOG_L_BASE_Y, height),
        screenScale(ANALOG_SIZE, height),
        screenScale(ANALOG_SIZE, height)
      );
      //


      controller.addElement(createDigitalButton(
        VirtualControllerElement.EID_BACK,
        ControllerPacket.BACK_FLAG, 0, 2, "BACK", -1, controller),
        screenScale(BACK_X, height),
        screenScale(START_BACK_Y, height),
        screenScale(START_BACK_WIDTH, height),
        screenScale(START_BACK_HEIGHT, height)
      );

      controller.addElement(createDigitalButton(
        VirtualControllerElement.EID_START,
        ControllerPacket.PLAY_FLAG, 0, 3, "START", -1, controller),
        screenScale(START_X, height) + rightDisplacement,
        screenScale(START_BACK_Y, height),
        screenScale(START_BACK_WIDTH, height),
        screenScale(START_BACK_HEIGHT, height)
      );
    }
    else {
      controller.addElement(createDigitalButton(
        VirtualControllerElement.EID_LSB,
        ControllerPacket.LS_CLK_FLAG, 0, 1, "L3", -1, controller),
        screenScale(TRIGGER_L_BASE_X, height),
        screenScale(L3_R3_BASE_Y, height),
        screenScale(TRIGGER_WIDTH, height),
        screenScale(TRIGGER_HEIGHT, height)
      );

      controller.addElement(createDigitalButton(
        VirtualControllerElement.EID_RSB,
        ControllerPacket.RS_CLK_FLAG, 0, 1, "R3", -1, controller),
        screenScale(TRIGGER_R_BASE_X + TRIGGER_DISTANCE, height) + rightDisplacement,
        screenScale(L3_R3_BASE_Y, height),
        screenScale(TRIGGER_WIDTH, height),
        screenScale(TRIGGER_HEIGHT, height)
      );
    }
    controller.setOpacity(config?.osc_opacity ?? 1);
  }
  public static async saveProfile(controller: VirtualController, context: Context): Promise<void> {
    const preferences = await dataPreferences.getPreferences(context, OSC_PREFERENCE)
    const prefEditor = preferences; // Use localStorage or other suitable storage mechanism
    for(let i=0; i< controller.elements.length; i++){
      const element = controller.elements[i] as VirtualControllerElement;
      const prefKey = `${element.elementId}`;
      try {
        prefEditor.put(prefKey,  element.getConfiguration());
      } catch (e) {
        console.error(e);
      }
    }
    prefEditor.flush()
  }
  public static async loadFromPreferences(controller: VirtualController, context: Context): Promise<void> {
    const pref: dataPreferences.Preferences = await dataPreferences.getPreferences(context, OSC_PREFERENCE)
    for(let i=0; i< controller.elements.length; i++){
      const element = controller.elements[i] as VirtualControllerElement;
      const prefKey = `${element.elementId}`;
      const jsonConfig: string  = (await pref.get(prefKey, null)).toString() ;
      if (jsonConfig !== null) {
        try {
          element.loadConfiguration(jsonConfig);
        } catch (e) {
          console.error(e);
          pref.delete(prefKey);
        }
      }
    }
  }
}
