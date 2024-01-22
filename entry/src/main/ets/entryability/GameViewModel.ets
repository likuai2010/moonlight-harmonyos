import { NvConnectionListener } from './nvstream/ConnetionListener';
import { AddressTuple, ComputerDetails } from './computers/ComputerDetails';
import { NvApp } from './http/NvApp';
import { NvHttp } from './http/NvHttp';
import { NvConnection } from './nvstream/NvConnection';
import { StreamConfiguration } from './nvstream/StreamConfiguration';
import { Loading } from '../pages/compoments/Loading';
import { getResStringByContext } from '../uitls/ResString';
import emitter from '@ohos.events.emitter'
import { SpinnerEventId } from '../uitls/CommonEvents';
import LimeLog from './LimeLog';
import promptAction from '@ohos.promptAction';
import { StreamSettings } from '../uitls/StreamSetttings';
import { ControllerHandle } from '../virtual_controller/ControllerHandle';
import { VirtualController } from '../virtual_controller/VirtualController';


class GameViewModel implements NvConnectionListener {
  conn: NvConnection
  context: Context
  controllerHandle: ControllerHandle
  virtualController: VirtualController = new VirtualController()
  constructor() {}

  async init(
    computer: ComputerDetails,
    app: NvApp,
    settings: StreamSettings
  ): Promise<Boolean>{
    let host = computer.activeAddress.address
    let port = computer.activeAddress.port
    let httpsPort = computer.httpsPort || 0
    if (port == -1){
      port = NvHttp.DEFAULT_HTTP_PORT
    }
    const config = new StreamConfiguration()

    config.app = app
    config.build(settings)
    config.supportedVideoFormats = 257
    config.clientRefreshRateX100 = 12000

    this.conn = new NvConnection(new AddressTuple(host, port), httpsPort, "58908e6f06d0b57c", config)
    await this.conn.initKey()
    this.controllerHandle = new ControllerHandle(this.conn, settings)
    this.virtualController.controllerHandle = this.controllerHandle
    return true
  }

  setControllerLED(controllerNumber: number, r: number, g: number, b: number): void {
    console.error("setControllerLED")
  }

  setMotionEventState(controllerNumber: number, motionType: number, reportRateHz: number): void {
    console.error("setMotionEventState")
  }

  setHdrMode(enabled: boolean, hdrMetadata: number[]): void {
    LimeLog.warning("setHdrMode")
  }

  rumbleTriggers(controllerNumber: number, leftTrigger: number, rightTrigger: number): void {
    LimeLog.warning("rumbleTriggers")
  }

  rumble(controllerNumber: number, lowFreqMotor: number, highFreqMotor: number): void {
    LimeLog.warning("rumble")
  }

  displayTransientMessage(message: string): void {
    LimeLog.warning("displayTransientMessage")
  }

  displayMessage(message: string): void {
    LimeLog.warning("displayMessage")
    promptAction.showToast({message: message})
  }

  connectionStatusUpdate(connectionStatus: number): void {
    console.debug("connectionStatusUpdate")
  }

  connectionTerminated(errorCode: number): void {
    LimeLog.error("Connection terminated: " + errorCode);
    this.spinnerDialog.close()
  }

  connectionStarted(): void {
    console.debug("connectionStarted")
  }

  stageFailed(stage: string, portFlags: number, errorCode: number): void {
    LimeLog.error(stage + " failed: " + errorCode);
    this.spinnerDialog.close()
  }

  stageComplete(stage: string): void {
    console.error("stageComplete")
  }

  stageStarting(stage: string): void {
    getResStringByContext(this.context,$r("app.string.conn_starting") ).then((m)=>{
      this.sendMessageToSpinner(m + " " + stage)
    })
  }
  private spinnerDialog: CustomDialogController

  private sendMessageToSpinner(message: string | Resource){
    if (typeof message === "string"){
      emitter.emit({eventId: SpinnerEventId}, {data:{ message: message}})
    } else {
      getResStringByContext(this.context, message).then((m)=>{
        emitter.emit({eventId: SpinnerEventId}, {data:{ message: m}})
      })
    }
  }

  async start(dialog: CustomDialogController, context: Context) {
    this.spinnerDialog = dialog
    this.context = context
    this.sendMessageToSpinner($r("app.string.conn_establishing_msg"))
    this.initMoonBridge()
    await this.conn.start(this);
  }
  stop() {
    this.conn.api.stopConnection();
  }

  initMoonBridge(){

    this.conn.api.onClStage("BridgeClStageStarting", (stage:string)=>{
      this.stageStarting(stage)
    })
    this.conn.api.onClStageFailed("BridgeClStageFailed",(stage:string, code: number)=>{
      this.stageFailed(stage, 0,code)
    })
    this.conn.api.onClStage("BridgeClStageComplete",(stage:string)=>{
      this.stageComplete(stage)
    })
    this.conn.api.onClConnection("BridgeClConnectionStarted", (code:number)=>{
      this.connectionStarted()
    })
    this.conn.api.onClConnection("BridgeClConnectionStatusUpdate", (code:number)=>{
      this.connectionStatusUpdate(code)
    })
    this.conn.api.onClConnection("BridgeClConnectionTerminated", (code:number)=>{
      this.connectionTerminated(code)
    })
  }
}


const vieModel = new GameViewModel()

export default vieModel