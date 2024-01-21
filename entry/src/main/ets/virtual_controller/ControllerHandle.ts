import { GenericControllerContext, InputDeviceContext } from './context/GenericControllerContext';

export class ControllerHandle {
  conn: any
  prefConfig: any
  currentControllers: number
  initialControllers: number
  defaultContext: InputDeviceContext
  stickDeadzone: number
  inputDeviceContexts: Map<number, InputDeviceContext> = new Map();

  constructor(prefConfig) {
    let deadzonePercentage = prefConfig.deadzonePercentage;
    // 1% is the lowest possible deadzone we support
    if (deadzonePercentage <= 0) {
      deadzonePercentage = 1;
    }

    this.stickDeadzone = deadzonePercentage / 100.0;
    this.defaultContext.leftStickXAxis = 0; //MotionEvent.AXIS_X;
    this.defaultContext.leftStickYAxis = 1; // MotionEvent.AXIS_Y;
    this.defaultContext.leftStickDeadzoneRadius = this.stickDeadzone;
    this.defaultContext.rightStickXAxis = 11 //MotionEvent.AXIS_Z;
    this.defaultContext.rightStickYAxis = 14 //MotionEvent.AXIS_RZ;
    this.defaultContext.rightStickDeadzoneRadius = this.stickDeadzone;
    this.defaultContext.leftTriggerAxis = 23 // MotionEvent.AXIS_BRAKE;
    this.defaultContext.rightTriggerAxis = 22 // MotionEvent.AXIS_GAS;
    this.defaultContext.hatXAxis = 15 //MotionEvent.AXIS_HAT_X;
    this.defaultContext.hatYAxis = 16 // MotionEvent.AXIS_HAT_Y;
    this.defaultContext.controllerNumber = 0;
    this.defaultContext.assignedControllerNumber = true;
    this.defaultContext.external = false;
  }

  private sendControllerInputPacket(originalContext: GenericControllerContext): void {
    const conn = this.conn
    //this.assignControllerNumberIfNeeded(originalContext);

    // Take the context's controller number and fuse all inputs with the same number
    const controllerNumber: number = originalContext.controllerNumber;
    let inputMap: number = 0;
    let leftTrigger: number = 0;
    let rightTrigger: number = 0;
    let leftStickX: number = 0;
    let leftStickY: number = 0;
    let rightStickX: number = 0;
    let rightStickY: number = 0;

    // In order to properly handle controllers that are split into multiple devices,
    // we must aggregate all controllers with the same controller number into a single
    // device before we send it.
    // for (let i = 0; i < this.inputDeviceContexts.length; i++) {
    //   const context: GenericControllerContext = this.inputDeviceContexts[i];
    //   if (
    //     context.assignedControllerNumber &&
    //     context.controllerNumber === controllerNumber &&
    //     context.mouseEmulationActive === originalContext.mouseEmulationActive
    //   ) {
    //     inputMap |= context.inputMap;
    //     leftTrigger |= this.maxByMagnitude(leftTrigger, context.leftTrigger);
    //     rightTrigger |= this.maxByMagnitude(rightTrigger, context.rightTrigger);
    //     leftStickX |= this.maxByMagnitude(leftStickX, context.leftStickX);
    //     leftStickY |= this.maxByMagnitude(leftStickY, context.leftStickY);
    //     rightStickX |= this.maxByMagnitude(rightStickX, context.rightStickX);
    //     rightStickY |= this.maxByMagnitude(rightStickY, context.rightStickY);
    //   }
    // }
    // for (let i = 0; i < this.usbDeviceContexts.length; i++) {
    //   const context: GenericControllerContext = this.usbDeviceContexts[i];
    //   if (
    //     context.assignedControllerNumber &&
    //     context.controllerNumber === controllerNumber &&
    //     context.mouseEmulationActive === originalContext.mouseEmulationActive
    //   ) {
    //     inputMap |= context.inputMap;
    //     leftTrigger |= this.maxByMagnitude(leftTrigger, context.leftTrigger);
    //     rightTrigger |= this.maxByMagnitude(rightTrigger, context.rightTrigger);
    //     leftStickX |= this.maxByMagnitude(leftStickX, context.leftStickX);
    //     leftStickY |= this.maxByMagnitude(leftStickY, context.leftStickY);
    //     rightStickX |= this.maxByMagnitude(rightStickX, context.rightStickX);
    //     rightStickY |= this.maxByMagnitude(rightStickY, context.rightStickY);
    //   }
    // }
    if (this.defaultContext.controllerNumber === controllerNumber) {
      inputMap |= this.defaultContext.inputMap;
      leftTrigger |= this.maxByMagnitude(leftTrigger, this.defaultContext.leftTrigger);
      rightTrigger |= this.maxByMagnitude(rightTrigger, this.defaultContext.rightTrigger);
      leftStickX |= this.maxByMagnitude(leftStickX, this.defaultContext.leftStickX);
      leftStickY |= this.maxByMagnitude(leftStickY, this.defaultContext.leftStickY);
      rightStickX |= this.maxByMagnitude(rightStickX, this.defaultContext.rightStickX);
      rightStickY |= this.maxByMagnitude(rightStickY, this.defaultContext.rightStickY);
    }

    if (originalContext.mouseEmulationActive) {
      // const changedMask: number = inputMap ^ originalContext.mouseEmulationLastInputMap;
      //
      // const aDown: boolean = (inputMap & ControllerPacket.A_FLAG) !== 0;
      // const bDown: boolean = (inputMap & ControllerPacket.B_FLAG) !== 0;
      //
      // originalContext.mouseEmulationLastInputMap = inputMap;
      //
      // if ((changedMask & ControllerPacket.A_FLAG) !== 0) {
      //   if (aDown) {
      //     conn.sendMouseButtonDown(MouseButtonPacket.BUTTON_LEFT);
      //   } else {
      //     conn.sendMouseButtonUp(MouseButtonPacket.BUTTON_LEFT);
      //   }
      // }
      // if ((changedMask & ControllerPacket.B_FLAG) !== 0) {
      //   if (bDown) {
      //     conn.sendMouseButtonDown(MouseButtonPacket.BUTTON_RIGHT);
      //   } else {
      //     conn.sendMouseButtonUp(MouseButtonPacket.BUTTON_RIGHT);
      //   }
      // }
      // if ((changedMask & ControllerPacket.UP_FLAG) !== 0) {
      //   if ((inputMap & ControllerPacket.UP_FLAG) !== 0) {
      //     conn.sendMouseScroll(1);
      //   }
      // }
      // if ((changedMask & ControllerPacket.DOWN_FLAG) !== 0) {
      //   if ((inputMap & ControllerPacket.DOWN_FLAG) !== 0) {
      //     conn.sendMouseScroll(-1);
      //   }
      // }
      // if ((changedMask & ControllerPacket.RIGHT_FLAG) !== 0) {
      //   if ((inputMap & ControllerPacket.RIGHT_FLAG) !== 0) {
      //     conn.sendMouseHScroll(1);
      //   }
      // }
      // if ((changedMask & ControllerPacket.LEFT_FLAG) !== 0) {
      //   if ((inputMap & ControllerPacket.LEFT_FLAG) !== 0) {
      //     conn.sendMouseHScroll(-1);
      //   }
      // }
      //
      // conn.sendControllerInput(
      //   controllerNumber,
      //   this.getActiveControllerMask(),
      //   0,
      //   0,
      //   0,
      //   0,
      //   0,
      //   0,
      //   0
      // );
    } else {
      conn.sendControllerInput(
        controllerNumber,
        this.getActiveControllerMask(),
        inputMap,
        leftTrigger,
        rightTrigger,
        leftStickX,
        leftStickY,
        rightStickX,
        rightStickY
      );
    }
  }

  getActiveControllerMask(): number {
    if (this.prefConfig.multiController) {
      return (this.currentControllers | this.initialControllers | (this.prefConfig.onscreenController ? 1 : 0));
    }
    else {
      // Only Player 1 is active with multi-controller disabled
      return 1;
    }
  }

  maxByMagnitude(a: number, b: number): number {
    const absA = Math.abs(a);
    const absB = Math.abs(b);
    if (absA > absB) {
      return a;
    }
    else {
      return b;
    }
  }
}