export  interface NvConnectionListener {
  stageStarting(stage: string): void;
  stageComplete(stage: string): void;
  stageFailed(stage: string, portFlags: number, errorCode: number): void;

  connectionStarted(): void;
  connectionTerminated(errorCode: number): void;
  connectionStatusUpdate(connectionStatus: number): void;

  displayMessage(message: string): void;
  displayTransientMessage(message: string): void;

  rumble(controllerNumber: number, lowFreqMotor: number, highFreqMotor: number): void;
  rumbleTriggers(controllerNumber: number, leftTrigger: number, rightTrigger: number): void;

  setHdrMode(enabled: boolean, hdrMetadata: number[]): void;

  setMotionEventState(controllerNumber: number, motionType: number, reportRateHz: number): void;

  setControllerLED(controllerNumber: number, r: number, g: number, b: number): void;
}