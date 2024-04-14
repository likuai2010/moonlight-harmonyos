
import router from '@ohos.router'
import viewModel from '../entryability/ComputerManagerViewModel'
import { ComputerDetails } from '../entryability/computers/ComputerDetails'
import { NvHttp } from '../entryability/http/NvHttp'
import { Icon, NavTitle } from './compoments/Title'
import { NvApp } from '../entryability/http/NvApp'
import image from '@ohos.multimedia.image'



@Component
struct AppView {
  computer!: ComputerDetails
  app!: NvApp
  @State image: PixelMap| undefined = undefined;
  aboutToAppear(){
    viewModel.readImageByDisk(this.app).then((res: Uint8Array | null)=>{
      let imageSource = image.createImageSource(res.buffer.slice(0));
      let options :image.DecodingOptions = {
        editable: false,                  // 是否可编辑
        desiredPixelFormat: 3,                   // 像素格式
      }
      if(imageSource)
        imageSource.createPixelMap(options).then((pixelMap:PixelMap) => {
          this.image = pixelMap
        })
    })
  }
  build(){
    Column(){
      Stack(){
        if (this.image){
          Image(this.image).height(150).width(100)
        } else {
          Text(this.app.appName).fontColor(Color.White)
        }
        if(this.computer.runningGameId == this.app.appId){
          Column(){
            Icon({icon: $r("app.media.play_arrow_FILL1_wght700_GRAD200_opsz48"), iconSize:48})
            Icon({icon: $r("app.media.stop_FILL1_wght700_GRAD200_opsz48"), iconSize: 48})
          }
        }
      }
    }.onClick(()=>{
        router.pushUrl({ url:"pages/GamePage", params: { app: this.app, computer: this.computer } })
    })
  }
}



@Entry
@Component
struct AppPage {
  computer!: ComputerDetails
  @State appList: Array<NvApp> = []
  aboutToAppear(){
    const params = router.getParams() as Record<string, string> ;
    viewModel.getComputerByUUid(params["uuid"] as string).then((d: ComputerDetails)=>{
      this.computer = d
      const appList = d.appList
      this.appList = appList.filter((d) => d.appId != null);
    })
  }
  build(){
    Column(){
      NavTitle({ title: (router.getParams() as Record<string, string>)["computerName"] as string }).width("100%")
      Grid() {
        ForEach(this.appList, (d:NvApp)=>{
          GridItem(){
            AppView({app: d, computer: this.computer})
          }
        },(item: NvApp) => JSON.stringify(item))
      }
      .rowsTemplate('1fr 1fr 1fr')
      .columnsTemplate('1fr 1fr 1fr')
    }.padding(10).height("100%").width("100%").backgroundColor($r("app.color.page_background"))
  }
}