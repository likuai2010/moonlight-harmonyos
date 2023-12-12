export const add: (a: number, b: number) => number;

export const generate_x509_certificate: (a: string, b: string) => number;

export declare class CurlClient {
  close()
  get(url: string, timeout: Number, client: string, key: string): Promise<string>;
}

export declare class MoonBridgeNapi {
  startConnection(
    address: string, appVersion: string, gfeVersion: string,
    rtspSessionUrl: string, serverCodecModeSupport: number,
    width: number, height: number, fps: number,
    bitrate: number, packetSize: number, streamingRemotely: number,
    audioConfiguration: number, supportedVideoFormats: number,
    clientRefreshRateX100: number,
    encryptionFlags: number,
    riAesKey: Uint8Array, riAesIv: Uint8Array,
    videoCapabilities: number,
    colorSpace: number, colorRange: number
  ): number;

  static stopConnection(): void;

  static interruptConnection(): void;

  static sendMouseMove(deltaX: number, deltaY: number): void;

  static sendMousePosition(x: number, y: number, referenceWidth: number, referenceHeight: number): void;

  static sendMouseMoveAsMousePosition(deltaX: number, deltaY: number, referenceWidth: number, referenceHeight: number): void;

  static sendMouseButton(buttonEvent: number, mouseButton: number): void;

  static sendMultiControllerInput(
    controllerNumber: number,
    activeGamepadMask: number, buttonFlags: number,
    leftTrigger: number, rightTrigger: number,
    leftStickX: number, leftStickY: number,
    rightStickX: number, rightStickY: number
  ): void;

  static sendTouchEvent(
    eventType: number, pointerId: number, x: number, y: number, pressure: number,
    contactAreaMajor: number, contactAreaMinor: number, rotation: number
  ): number;

  static sendPenEvent(
    eventType: number, toolType: number, penButtons: number, x: number, y: number,
    pressure: number, contactAreaMajor: number, contactAreaMinor: number,
    rotation: number, tilt: number
  ): number;

  static sendControllerArrivalEvent(
    controllerNumber: number, activeGamepadMask: number, type: number, supportedButtonFlags: number, capabilities: number
  ): number;

  static sendControllerTouchEvent(
    controllerNumber: number, eventType: number, pointerId: number, x: number, y: number, pressure: number
  ): number;

  static sendControllerMotionEvent(controllerNumber: number, motionType: number, x: number, y: number, z: number): number;

  static sendControllerBatteryEvent(controllerNumber: number, batteryState: number, batteryPercentage: number): number;

  static sendKeyboardInput(keyMap: number, keyDirection: number, modifier: number, flags: number): void;

  static sendMouseHighResScroll(scrollAmount: number): void;

  static sendMouseHighResHScroll(scrollAmount: number): void;

  static sendUtf8Text(text: string): void;

  static getStageName(stage: number): string;

  static findExternalAddressIP4(stunHostName: string, stunPort: number): string;

  static getPendingAudioDuration(): number;

  static getPendingVideoFrames(): number;

  static testClientConnectivity(testServerHostName: string, referencePort: number, testFlags: number): number;

  static getPortFlagsFromStage(stage: number): number;

  static getPortFlagsFromTerminationErrorCode(errorCode: number): number;

  static stringifyPortFlags(portFlags: number, separator: string): string;

  static getEstimatedRttInfo(): bigint;

  static guessControllerType(vendorId: number, productId: number): number;

  static guessControllerHasPaddles(vendorId: number, productId: number): boolean;

  static guessControllerHasShareButton(vendorId: number, productId: number): boolean;

  static init(): void;
}
