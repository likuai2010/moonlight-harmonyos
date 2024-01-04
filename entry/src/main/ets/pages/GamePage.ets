import hilog from '@ohos.hilog';
import { Game } from 'ets/entryability/Game'
import { MoonBridge } from '../entryability/nvstream/MoonBridge';
import { Loading } from './Loading'
const game = new Game()

@Entry
@Component
struct GamePage {
  @State message: string = 'Hello World'
  @State textValue: string = ''
  xComponentContext: any | undefined = undefined;
  dialogController: CustomDialogController = new CustomDialogController({
    builder: Loading({
      textValue: $textValue,
    }),
    cancel: ()=>{
    },
    autoCancel: false,
    alignment: DialogAlignment.Center,
    offset: { dx: 0, dy: 0 },
    gridCount: 2,
    customStyle: true
  })
  aboutToDisappear() {
    delete this.dialogController, // 删除dialogController
    this.dialogController = undefined // 将dialogController置空
  }
  aboutToAppear(){
    this.dialogController.open()
    game.surfaceChanged()
  }
  build() {
    Stack({ alignContent: Alignment.TopStart }){
      XComponent({ id: 'xcomponentId1', type: 'surface', libraryname: 'entry' })
        .onLoad((context: any) => {
          console.log("xxxxxx", context.add)
          this.xComponentContext = context
        })
        .onDestroy(() => {
          console.log("onDestroy");
        }).borderWidth(0).height("100%").width('100%')
      Column(){
        Text("视频流: ").fontColor(Color.White)
        Text("解码器: ").fontColor(Color.White)
        Text("网络接收帧数: ").fontColor(Color.White)
        Text("渲染帧数: ").fontColor(Color.White)
        Text("网络丢失帧: ").fontColor(Color.White)
        Text("平均网络延迟: ").fontColor(Color.White)
        //Text("主机处理延迟: ")
        Text("平均解码时间: ").fontColor(Color.White)
      }.backgroundColor(Color.Black).opacity(0.5)
      .alignItems(HorizontalAlign.Start)
    }.backgroundColor(Color.Black)
    .width('100%')
    .height('100%')
  }
}
