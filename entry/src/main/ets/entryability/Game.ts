import { NvConnectionListener } from './nvstream/ConnetionListener';
import { AddressTuple } from './http/ComputerDetails';
import { NvApp } from './http/NvApp';
import { NvHttp } from './http/NvHttp';
import { NvConnection } from './nvstream/NvConnection';
import { StreamConfiguration } from './nvstream/StreamConfiguration';


export class Game implements NvConnectionListener {
  conn: NvConnection

  constructor(uniqueId: string = "58908e6f06d0b57c", httpsPort: number = 47984) {
    const config = new StreamConfiguration()
    config.app = new NvApp()
    config.app.appName = "Desktop"
    config.app.appId = 881448767
    config.app.hdrSupported = false
    config.supportedVideoFormats = 257
    config.colorRange = 0
    config.colorSpace = 1
    config.encryptionFlags = 1
    config.clientRefreshRateX100 = 12000
    config.playLocalAudio = false
    config.remote = 2
    config.sops = true
    config.enableAdaptiveResolution = false
    this.conn = new NvConnection(new AddressTuple("192.168.3.5", NvHttp.DEFAULT_HTTP_PORT), httpsPort, uniqueId, config)
  }

  setControllerLED(controllerNumber: number, r: number, g: number, b: number): void {
    console.error("setControllerLED")
  }

  setMotionEventState(controllerNumber: number, motionType: number, reportRateHz: number): void {
    console.error("setMotionEventState")
  }

  setHdrMode(enabled: boolean, hdrMetadata: number[]): void {
    console.error("setHdrMode")
  }

  rumbleTriggers(controllerNumber: number, leftTrigger: number, rightTrigger: number): void {
    console.error("rumbleTriggers")
  }

  rumble(controllerNumber: number, lowFreqMotor: number, highFreqMotor: number): void {
    console.debug("rumble")
  }

  displayTransientMessage(message: string): void {
    console.debug("displayTransientMessage")
  }

  displayMessage(message: string): void {
    console.debug("displayMessage")
  }

  connectionStatusUpdate(connectionStatus: number): void {
    console.debug("connectionStatusUpdate")
  }

  connectionTerminated(errorCode: number): void {
    console.debug("connectionTerminated")
  }

  connectionStarted(): void {
    console.debug("connectionStarted")
  }

  stageFailed(stage: string, portFlags: number, errorCode: number): void {
    console.error("stageFailed")
  }

  stageComplete(stage: string): void {
    console.error("stageComplete")
  }

  stageStarting(stage: string): void {
    console.error("stageStarting")
  }

  async surfaceChanged() {
    await this.conn.start(this);
  }
}