import gameViewModel from 'ets/entryability/GameViewModel'
import { VideoStatus } from 'libentry.so';
import router from '@ohos.router';
import { Spinner } from './compoments/Loading'
import { StreamSettings, loadSettings } from '../uitls/StreamSetttings';
import { ShowVirtualController, VirtualController, VirtualControllerBox } from '../virtual_controller/VirtualController';

@Entry
@Component
struct GamePage {
  @State videoStatus: VideoStatus = null
  xComponentContext: any | undefined = undefined;
  dialogController: CustomDialogController =  new CustomDialogController({
    builder: Spinner({ title: $r('app.string.conn_establishing_title'), text: "" }),
    autoCancel: false,
    alignment: DialogAlignment.Center,
    customStyle: true
  })
  aboutToDisappear() {
    this.dialogController.close()
    gameViewModel.stop()
    delete this.dialogController, // 删除dialogController
    this.dialogController = undefined // 将dialogController置空
  }
  aboutToAppear(){
    this.dialogController.open()
    loadSettings(getContext(this)).then((s)=>{
      gameViewModel.init(router.getParams()["computer"], router.getParams()["app"], s)
      //gameViewModel.start(this.dialogController, getContext(this))
      gameViewModel.conn.onVideoStatus((s)=>{
        this.videoStatus = s;
      })
    })
  }
  build() {
    Stack({ alignContent: Alignment.TopStart }){
      XComponent({ id: 'xcomponentId1', type: 'surface', libraryname: 'entry' })
        .onLoad((context: any) => {
          this.xComponentContext = context
        })
        .onDestroy(() => {
          console.log("onDestroy");
        }).borderWidth(0).height("100%").width('100%')
      if(this.videoStatus){
        Column(){
          Text(`视频流:  ${this.videoStatus?.totalFps?.toFixed(2) || 0 } FPS`).fontColor(Color.White)
          Text("解码器: ").fontColor(Color.White)
          Text(`网络接收帧数: ${this.videoStatus?.receivedFps?.toFixed(2) || 0 } FPS`) .fontColor(Color.White)
          Text(`渲染帧数: ${this.videoStatus?.renderedFps?.toFixed(2) || 0 } FPS`).fontColor(Color.White)
          Text(`网络丢失帧:  ${this.videoStatus?.networkDroppedFrames?.toFixed(2) || 0 } %`).fontColor(Color.White)
          Text(`平均网络延迟: ${this.videoStatus?.receivedTime?.toFixed(2) || 0 } `).fontColor(Color.White)
          //Text("主机处理延迟: ")
          Text(`平均解码时间:  ${this.videoStatus?.decodeTime?.toFixed(2) || 0 }  ms`).fontColor(Color.White)
        }.backgroundColor(Color.Black).opacity(0.5)
      }
      if (gameViewModel.virtualController != null){
        ShowVirtualController(gameViewModel.virtualController)
      }
    }.backgroundColor(Color.Black)
    .width('100%')
    .height('100%')
  }
}
