import gameViewModel from 'ets/entryability/GameViewModel'
import { VideoStatus } from 'libentry.so';
import router from '@ohos.router';
import { Spinner } from './compoments/Loading'
import { loadSettings, StreamSettings } from '../uitls/StreamSetttings';
import { VirtualController  } from '../virtual_controller/VirtualController';
import {  VirtualControllerElement } from '../virtual_controller/common';
import { VirtualControllerConfigurationLoader } from '../virtual_controller/VirtualControllerConfigurationLoader';
import { VirtualControllerButton } from '../virtual_controller/common';
import { Icon } from './compoments/Title';
import { ComputerDetails } from '../entryability/computers/ComputerDetails';
import { NvApp } from '../entryability/http/NvApp';

@Entry
@Component
struct GamePage {
  @State videoStatus: VideoStatus | null = null
  xComponentContext: object | undefined = undefined;
  @State virtualController: VirtualController | null = null
  @State settings: StreamSettings | null = null

  dialogController?: CustomDialogController =  new CustomDialogController({
    builder: Spinner({ title: $r('app.string.conn_establishing_title'), text: "" }),
    autoCancel: false,
    alignment: DialogAlignment.Center,
    customStyle: true
  })
  aboutToDisappear() {
    this.dialogController?.close()
    gameViewModel.stop()
    this.dialogController = undefined // 将dialogController置空
  }
  async onInit(){
    this.dialogController?.open()
    const settings = await loadSettings(getContext(this))
    this.settings = settings
    let param = router.getParams() as Record<string, ComputerDetails | NvApp>;
    await gameViewModel.init(param["computer"] as ComputerDetails, param["app"]  as NvApp, settings)
    await gameViewModel.start(this.dialogController!, getContext(this))
    if (settings.touchscreen_trackpad){
      const loader = new VirtualControllerConfigurationLoader()
      loader.createDefaultLayout(gameViewModel.virtualController, settings)
      VirtualControllerConfigurationLoader.loadFromPreferences(gameViewModel.virtualController, getContext(this))
      this.virtualController = gameViewModel.virtualController
    }
    gameViewModel.conn.onVideoStatus((s: VideoStatus)=>{
      this.videoStatus = s;
    })
  }

  aboutToAppear(){
    this.onInit()

  }
  build() {
    Stack({ alignContent: Alignment.TopStart }){
      XComponent({ id: 'xcomponentId1', type: 'surface', libraryname: 'entry' })
        .onLoad((context: object) => {
          this.xComponentContext = context
        })
        .onDestroy(() => {
          console.log("onDestroy");
        }).borderWidth(0).height("100%").width('100%')
      if(this.videoStatus){
        Column(){
          Text(`视频流:  ${this.settings?.resolution_list} ${this.videoStatus?.totalFps?.toFixed(2) || 0 } FPS`).fontColor(Color.White)
          Text("解码器: ").fontColor(Color.White)
          Text(`网络接收帧数: ${this.videoStatus?.receivedFps?.toFixed(2) || 0 } FPS`) .fontColor(Color.White)
          Text(`渲染帧数: ${this.videoStatus?.renderedFps?.toFixed(2) || 0 } FPS`).fontColor(Color.White)
          Text(`网络丢失帧:  ${this.videoStatus?.networkDroppedRate?.toFixed(2) || 0 } %`).fontColor(Color.White)
          Text(`平均网络延迟: ${this.videoStatus?.receivedTime?.toFixed(2) || 0 } `).fontColor(Color.White)
          //Text("主机处理延迟: ")
          Text(`平均解码时间:  ${this.videoStatus?.decodeTime?.toFixed(2) || 0 }  ms`).fontColor(Color.White)
        }.backgroundColor(Color.Black).opacity(0.5)
      }
     if (this.virtualController != null){
       ForEach(this.virtualController?.elements.convertToArray(), (d: VirtualControllerElement) => {
         VirtualControllerButton({ element: d, layout: d.layout })
       }, (d: VirtualControllerElement) => d.elementId.toString())
       Button(){
         Icon({icon:$r("app.media.settings")})
       }.offset({y: 20}).onClick(()=>{
         this.virtualController?.onSettingsClick(getContext(this))
       })
     }
    }.backgroundColor(Color.Black)
    .width('100%')
    .height('100%')
  }
}
