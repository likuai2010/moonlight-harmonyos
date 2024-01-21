import Prompt from '@system.prompt';
import { ControllerPacket } from '../ControllerPacket';
export class GenericControllerContext {
  public id: number;
  public external: boolean;

  public vendorId: number;
  public productId: number;

  public leftStickDeadzoneRadius: number;
  public rightStickDeadzoneRadius: number;
  public triggerDeadzone: number;

  public assignedControllerNumber: boolean;
  public reservedControllerNumber: boolean;
  public controllerNumber: number;

  public inputMap: number = 0;
  public leftTrigger: number = 0x00;
  public rightTrigger: number = 0x00;
  public rightStickX: number = 0x0000;
  public rightStickY: number = 0x0000;
  public leftStickX: number = 0x0000;
  public leftStickY: number = 0x0000;

  public mouseEmulationActive: boolean;
  public mouseEmulationLastInputMap: number;
  public readonly mouseEmulationReportPeriod: number = 50;

  public readonly mouseEmulationRunnable: () => void = () => {
    if (!this.mouseEmulationActive) {
      return;
    }

    // Send mouse movement events from analog sticks
    this.sendEmulatedMouseEvent(this.leftStickX, this.leftStickY);
    this.sendEmulatedMouseEvent(this.rightStickX, this.rightStickY);

    // Requeue the callback
  };

  public toggleMouseEmulation(): void {
    this.mouseEmulationActive = !this.mouseEmulationActive;
    Prompt.showToast({message: "Mouse emulation is: " + (this.mouseEmulationActive ? "ON" : "OFF") })

    if (this.mouseEmulationActive) {
    }
  }

  public destroy(): void {
    this.mouseEmulationActive = false;
  }

  public sendControllerArrival(): void {
    // Implement if needed
  }

  private sendEmulatedMouseEvent(x: number, y: number): void {
    // Implement the logic to send emulated mouse events
  }
}


export class InputDeviceContext extends GenericControllerContext {
  public name: string;
  public quadVibrators: boolean;
  public lowFreqMotor: number;
  public highFreqMotor: number;
  public leftTriggerMotor: number;
  public rightTriggerMotor: number;

  public accelReportRateHz: number;

  public hasRgbLed: boolean;

  public lastReportedBatteryStatus: number;
  public lastReportedBatteryCapacity: number;

  public leftStickXAxis: number = -1;
  public leftStickYAxis: number = -1;

  public rightStickXAxis: number = -1;
  public rightStickYAxis: number = -1;

  public leftTriggerAxis: number = -1;
  public rightTriggerAxis: number = -1;
  public triggersIdleNegative: boolean;
  public leftTriggerAxisUsed: boolean;
  public rightTriggerAxisUsed: boolean;

  public hatXAxis: number = -1;
  public hatYAxis: number = -1;
  public hatXAxisUsed: boolean;
  public hatYAxisUsed: boolean;


  public isNonStandardDualShock4: boolean;
  public usesLinuxGamepadStandardFaceButtons: boolean;
  public isNonStandardXboxBtController: boolean;
  public isServal: boolean;
  public backIsStart: boolean;
  public modeIsSelect: boolean;
  public searchIsMode: boolean;
  public ignoreBack: boolean;
  public hasJoystickAxes: boolean;
  public pendingExit: boolean;
  public isDualShockStandaloneTouchpad: boolean;

  public emulatingButtonFlags: number = 0;
  public hasSelect: boolean;
  public hasMode: boolean;
  public hasPaddles: boolean;
  public hasShare: boolean;
  public needsClickpadEmulation: boolean;

  public lastLbUpTime: number = 0;
  public lastRbUpTime: number = 0;

  public startDownTime: number = 0;

  public readonly batteryStateUpdateRunnable: () => void = () => {
    //this.sendControllerBatteryPacket(this);

  };

  public readonly enableSensorRunnable: () => void = () => {
    // Turn back on any sensors that should be reporting but are currently unregistered
    // if (this.accelReportRateHz != 0 && this.accelListener == null) {
    //   this.handleSetMotionEventState(this.controllerNumber, MoonBridge.LI_MOTION_TYPE_ACCEL, this.accelReportRateHz);
    // }
    // if (this.gyroReportRateHz != 0 && this.gyroListener == null) {
    //   this.handleSetMotionEventState(this.controllerNumber, MoonBridge.LI_MOTION_TYPE_GYRO, this.gyroReportRateHz);
    // }
  };

  public destroy(): void {
    super.destroy();

  }

  public sendControllerArrival(): void {
    // Below KitKat we can't get enough information to report controller details accurately

  }

  public migrateContext(oldContext: InputDeviceContext): void {
    this.accelReportRateHz = oldContext.accelReportRateHz;

    // Don't release the controller number, because we will carry it over if it is present.
    // We also want to make sure the change is invisible to the host PC to avoid an add/remove
    // cycle for the gamepad which may break some games.
    oldContext.destroy();

    // Copy over existing controller number state
    this.assignedControllerNumber = oldContext.assignedControllerNumber;
    this.reservedControllerNumber = oldContext.reservedControllerNumber;
    this.controllerNumber = oldContext.controllerNumber;


    // Copy state initialized in reportControllerArrival()
    this.needsClickpadEmulation = oldContext.needsClickpadEmulation;

    // Re-enable sensors on the new context
    this.enableSensors();

  }

  public disableSensors(): void {
    // Stop any pending enablement
  }

  public enableSensors(): void {
    // We allow 1 second for the input device to settle before re-enabling sensors.
    // Pointer capture can cause the input device to change, which can cause
    // InputDeviceSensorManager to crash due to missing null checks on the InputDevice.
  }
}
