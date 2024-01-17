
import emitter from '@ohos.events.emitter'
import { SpinnerEventId } from '../../uitls/CommonEvents'

@CustomDialog
export struct Spinner {
  title: string | Resource
  @State text: string  = ""
  controller: CustomDialogController
  aboutToAppear(){
    let innerEvent = {
      eventId: SpinnerEventId,
    };
    emitter.on(innerEvent, (eventData)=>{
      this.text = eventData.data["message"]
    })
  }
  build() {
    Column(){
      Text(this.title).fontSize(18)
      Row(){
        LoadingProgress().height(100).width(80)
        Text(this.text).maxLines(2).height(100).textOverflow({overflow: TextOverflow.Ellipsis}).layoutWeight(1)
      }
    }.padding(20)
    .alignItems(HorizontalAlign.Start).backgroundColor(Color.White).borderRadius(10)
    .width("40%")
    .height(140)
  }
}

@CustomDialog
export struct Loading {
  title: string | Resource
  text: string | Resource = ""
  controller: CustomDialogController

  build() {
    Column(){
      Text(this.title).fontSize(18)
      Row(){
        LoadingProgress().height(100).width(80)
        Text(this.text).maxLines(2).height(100).textOverflow({overflow: TextOverflow.Ellipsis}).layoutWeight(1)
      }
    }.padding(20)
    .alignItems(HorizontalAlign.Start).backgroundColor(Color.White).borderRadius(10)
    .width("40%")
    .height(140)
  }
}

@CustomDialog
export struct Alert {
  title: string | Resource = "建立连接中"
  message: string | Resource
  controller: CustomDialogController
  build() {
    Column(){
      Text(this.title).fontSize(18)
      Blank().height(20)
      Text(this.message)
    }.padding(20)
    .alignItems(HorizontalAlign.Start).backgroundColor(Color.White).borderRadius(10)
    .width("40%")
  }
}
