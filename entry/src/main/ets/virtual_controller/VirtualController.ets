import mediaquery from '@ohos.mediaquery';

import { ElementLayoutParam, VirtualControllerElement } from './common';
import List from '@ohos.util.List';
import { createLeftStick, VirtualControllerConfigurationLoader } from './VirtualControllerConfigurationLoader';
import Prompt from '@system.prompt';
import { ControllerHandle } from './ControllerHandle';
import { ArrayList } from '@kit.ArkTS';
import { Icon } from '../pages/compoments/Title';

export enum ControllerMode {
  Active,
  MoveButtons,
  ResizeButtons
}
@Observed
export class ControllerInputContext {
  public inputMap: number = 0x0000;
  public leftTrigger: number = 0x00;
  public rightTrigger: number = 0x00;
  public rightStickX: number = 0x0000;
  public rightStickY: number = 0x0000;
  public leftStickX: number = 0x0000;
  public leftStickY: number = 0x0000;
}

export class VirtualController {
  controllerHandle!: ControllerHandle
  currentMode: ControllerMode = ControllerMode.Active
  inputContext: ControllerInputContext = new ControllerInputContext();

  elements: ArrayList<VirtualControllerElement> = new ArrayList()

  constructor() {
  }
  addElement(element: VirtualControllerElement, x: number, y: number, width: number, height: number) {
    element.setLayout(new ElementLayoutParam(x, y, width, height))
    this.elements.add(element)
  }
  onSettingsClick(context: Context){
    let message = ""
    if (this.currentMode == ControllerMode.Active){
      this.currentMode = ControllerMode.MoveButtons;
      message = "Entering configuration mode (Move buttons)";
    } else if (this.currentMode == ControllerMode.MoveButtons) {
      this.currentMode = ControllerMode.ResizeButtons;
      message = "Entering configuration mode (Resize buttons)";
    } else {
      this.currentMode = ControllerMode.Active;
      VirtualControllerConfigurationLoader.saveProfile(this, context);
      message = "Exiting configuration mode";
    }
    Prompt.showToast({message})
  }
  sendControllerInputContext(): void{
    //Prompt.showToast({message: JSON.stringify(this.inputContext)})
    const inputContext = this.inputContext
    if(!this.controllerHandle)
      return;
    this.controllerHandle.reportOscState(inputContext.inputMap,
      inputContext.leftStickX,
      inputContext.leftStickY,
      inputContext.rightStickX,
      inputContext.rightStickY,
      inputContext.leftTrigger,
      inputContext.rightTrigger)
  }
  setOpacity(opactiy: number) {

  }
}

@Builder
export function showVirtualController(virtualController: VirtualController, context: Context){
  ForEach(virtualController.elements.convertToArray(), (d: VirtualControllerElement) => {
    VirtualControllerButton({ element: d, layout: d.layout })
  }, (d: VirtualControllerElement ) =>d.elementId.toString())
  Button(){
    Icon({icon:$r("app.media.settings")})
  }.offset({y: 20}).onClick(()=>{
    virtualController.onSettingsClick(context)
  })
}


@Component
export struct VirtualControllerButton {

  //用来配置CanvasRenderingContext2D对象的参数，包括是否开启抗锯齿，true表明开启抗锯齿。
  private settings: RenderingContextSettings = new RenderingContextSettings(true)
  //用来创建CanvasRenderingContext2D对象，通过在canvas中调用CanvasRenderingContext2D对象来绘制。
  private context: CanvasRenderingContext2D = new CanvasRenderingContext2D(this.settings)
  element?: VirtualControllerElement
  layout?: ElementLayoutParam

  build() {
    Canvas(this.context)
      .height(this.layout?.height)
      .width(this.layout?.width)
      .translate({ x: this.layout?.x, y: this.layout?.y })
      .opacity(1)
      .onReady(() => {
        this.element?.onDraw(this.context)
      })
      .onTouch((e) => {
        // 单点
        this.element?.onTouchEvent(e)
        this.element?.onDraw(this.context)
      })
  }
}

@Component
export struct VirtualControllerBox{
  virtualController!: VirtualController
  @ObjectLink inputContext: ControllerInputContext
  aboutToAppear() {
    const loader = new VirtualControllerConfigurationLoader()
    loader.createDefaultLayout(this.virtualController)
    VirtualControllerConfigurationLoader.loadFromPreferences(this.virtualController, getContext(this))
  }
  build() {
    Stack({ alignContent: Alignment.TopStart }) {
      showVirtualController(this.virtualController, getContext(this))
      Text(JSON.stringify(this.inputContext)).alignSelf(ItemAlign.Center).offset({x: 50,y:0})
    }.height('100%')
    .width('100%')
  }
}


