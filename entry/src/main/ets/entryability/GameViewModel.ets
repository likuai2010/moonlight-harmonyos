import { NvConnectionListener } from './nvstream/ConnetionListener';
import { AddressTuple, ComputerDetails } from './computers/ComputerDetails';
import { NvApp } from './http/NvApp';
import { NvHttp } from './http/NvHttp';
import { NvConnection } from './nvstream/NvConnection';
import { StreamConfiguration } from './nvstream/StreamConfiguration';
import { BuildDialog } from '../uitls/Dialog';
import { Loading } from '../pages/compoments/Loading';
import { getResStringByContext } from '../uitls/ResString';

class GameViewModel implements NvConnectionListener {
  conn: NvConnection
  context: Context
  constructor() {}

  async init(
    computer: ComputerDetails,
    app: NvApp
  ): Promise<Boolean>{
    let host = computer.activeAddress.address
    let port = computer.activeAddress.port
    let httpsPort = computer.httpsPort || 0
    if (port == -1){
      port = NvHttp.DEFAULT_HTTP_PORT
    }
    const config = new StreamConfiguration()
    config.app = app
    config.supportedVideoFormats = 257
    config.colorRange = 0
    config.colorSpace = 1
    config.encryptionFlags = 1
    config.clientRefreshRateX100 = 12000
    config.playLocalAudio = false
    config.remote = 2
    config.sops = true
    config.enableAdaptiveResolution = false
    this.conn = new NvConnection(new AddressTuple(host, port), httpsPort, "58908e6f06d0b57c", config)
    this.conn.initKey()
    return true
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
    this.loadingDialog.close()
  }

  connectionStarted(): void {
    console.debug("connectionStarted")
    this.onDialogCallback("connectionStarted")
  }

  stageFailed(stage: string, portFlags: number, errorCode: number): void {
    console.error("stageFailed")
    this.loadingDialog.close()
  }

  stageComplete(stage: string): void {
    console.error("stageComplete")
  }

  stageStarting(stage: string): void {
    console.error("stageStarting")

  }
  private loadingDialog: CustomDialogController
  private onDialogCallback: ((string) => void) = (string)=>{}
  async onDialogMessage(callback: (string)=> void){
    this.onDialogCallback = callback
  }
  async start(dialog: CustomDialogController, context: Context) {
    dialog.open()
    this.onDialogCallback("xxx")
    this.loadingDialog = dialog
    await this.conn.start(this);
    this.onDialogCallback("22222")
  }
  stop() {
    this.conn.api.stopConnection();
  }
}


const vieModel = new GameViewModel()

export default vieModel