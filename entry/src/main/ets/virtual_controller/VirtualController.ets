import mediaquery from '@ohos.mediaquery';

import { ElementLayoutParam, VirtualControllerButton, VirtualControllerElement } from './common';
import List from '@ohos.util.List';
import { createLeftStick, VirtualControllerConfigurationLoader } from './VirtualControllerConfigurationLoader';
import Prompt from '@system.prompt';
import { ControllerHandle } from './ControllerHandle';
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
  controllerHandle: ControllerHandle
  currentMode: ControllerMode = ControllerMode.Active
  inputContext: ControllerInputContext = new ControllerInputContext();

  elements: List<VirtualControllerElement> = new List()

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
  sendControllerInputContext(){
    //Prompt.showToast({message: JSON.stringify(this.inputContext)})
    const inputContext = this.inputContext
    if(!this.controllerHandle)
      return null;
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
export struct VirtualControllerBox{
  virtualController: VirtualController
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


