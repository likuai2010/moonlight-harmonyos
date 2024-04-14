
import emitter from '@ohos.events.emitter'
import { SpinnerEventId } from '../../uitls/CommonEvents'
import router from '@ohos.router'
import common from '@ohos.app.ability.common';
import UIAbility from '@ohos.app.ability.UIAbility';

@CustomDialog
export struct Spinner {
  title?: string | Resource
  @State text: string = ""
  controller: CustomDialogController
  aboutToAppear(){
    emitter.on({
      eventId: SpinnerEventId,
    }, (eventData)=>{
      if (eventData.data)
        this.text = eventData.data["message"]
    })
  }
  build() {
    Column(){
      Text(this.title).fontSize(18)
      Row(){
        LoadingProgress().height(100).width(80)
        if (this.text == ""){
          Text($r("app.string.conn_establishing_msg")).maxLines(2).height(100).textOverflow({overflow: TextOverflow.Ellipsis}).layoutWeight(1)
        } else {
          Text(this.text).maxLines(2).height(100).textOverflow({overflow: TextOverflow.Ellipsis}).layoutWeight(1)
        }
      }
    }.padding(20)
    .alignItems(HorizontalAlign.Start).backgroundColor(Color.White).borderRadius(10)
    .width("40%")
    .height(140)
  }
}

@CustomDialog
export struct Loading {
  title?: string | Resource
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
  message?: string | Resource
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

@CustomDialog
export struct PrivacyConfirm {
  title: string | Resource = "隐私政策"
  message: string | Resource
  onConfirm: ()=> void
  controller: CustomDialogController
  build() {
    Column(){
      Text(this.title).fontSize(18).textAlign(TextAlign.Center).width("100%")
      Blank().height(10)
      Text("欢迎使用本软件!")
      Blank().height(5)
      Text("在您使用本软件前,请您认真阅读并了解相应的隐私政策,以了解我们的服务内容和对您相关个人信息的处理规则。我们将严格的按照隐私政策为您提供服务,保护您的个人信息。")
      Blank().height(10)
      Text("《隐私政策》").fontColor(0x328dff).onClick(() => {
        router.pushUrl({url:"pages/PrivacyPage"})
      })
      Flex({ justifyContent: FlexAlign.SpaceAround }) {
        Button('不同意并退出')
          .onClick(() => {
            this.controller.close()
            var ui = getContext(this) as common.UIAbilityContext
            ui.terminateSelf()
          }).backgroundColor(0xffffff).fontColor(Color.Red)
        Button('同意')
          .onClick(() => {
            if (this.controller != undefined) {
              this.controller.close()
              AppStorage.Set("privacyConfirm", true)
            }
          }).backgroundColor(0xffffff).fontColor(Color.Black)
      }.margin({ top: 10 })
    }.padding({ left: 20, right: 20, top:10, bottom: 10 }).backgroundColor(Color.White)
    .alignItems(HorizontalAlign.Start).borderRadius(10)
    .width("50%")
  }
}
