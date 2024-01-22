
import { VirtualController, VirtualControllerBox } from '../virtual_controller/VirtualController'

@Entry
@Component
struct VirtualControllerSettings {
  virtualController = new VirtualController()
  aboutToAppear(){
  }
  build(){
    Stack({alignContent:Alignment.TopStart}){
      VirtualControllerBox({virtualController: this.virtualController, inputContext:this.virtualController.inputContext })
    }
  }
}