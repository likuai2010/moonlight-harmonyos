import { AnalogStick, } from './AnalogStick';
import { DigitalButton } from './DigitalButton';
import mediaquery from '@ohos.mediaquery';

import { ElementLayoutParam, VirtualControllerButton, VirtualControllerElement } from './common';
import List from '@ohos.util.List';
import { DigitalPad } from './DigitalPad';
import { createLeftStick, VirtualControllerConfigurationLoader } from './VirtualControllerConfigurationLoader';
import Prompt from '@system.prompt';

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
  currentMode: ControllerMode = ControllerMode.Active
  inputContext: ControllerInputContext = new ControllerInputContext();
  elements: List<VirtualControllerElement> = new List()

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

    
  }
  setOpacity(opactiy: number) {

  }
}






@Component
export struct VirtualControllerTest {
  @State message: string = '11111 World'
  virtualController: VirtualController
  @ObjectLink inputContext: ControllerInputContext

  aboutToAppear() {
    mediaquery.matchMediaSync('(screen: landscape)')
    const loader = new VirtualControllerConfigurationLoader()
    this.virtualController.currentMode = ControllerMode.MoveButtons
    loader.createDefaultLayout(this.virtualController)
    VirtualControllerConfigurationLoader.loadFromPreferences(this.virtualController, getContext(this))
  }
  build() {
    Row() {
      Stack({ alignContent: Alignment.TopStart }) {
        ForEach(this.virtualController.elements.convertToArray(), (d: VirtualControllerElement) => {
          VirtualControllerButton({ element: d, layout: d.layout })
        }, (d:VirtualControllerElement) =>JSON.stringify(d))
        Button(){
          Text("setting")
        }.offset({y: 80}).onClick(()=>{
          this.virtualController.onSettingsClick(getContext(this))
        })
        Text(JSON.stringify(this.inputContext)).alignSelf(ItemAlign.Center).offset({x: 50,y:0})
      }.height('100%')
      .width('100%')
    }
    .height('100%')
  }
}


