
import router from '@ohos.router'
import viewModel from '../entryability/ComputerManagerViewModel'
import { ComputerDetails } from '../entryability/computers/ComputerDetails'
import { NvHttp } from '../entryability/http/NvHttp'
import { Icon, NavTitle } from './compoments/Title'
import { NvApp } from '../entryability/http/NvApp'
import image from '@ohos.multimedia.image'



@Component
struct AppView {
  computer: ComputerDetails
  app: NvApp
  @State image: PixelMap = undefined;
  aboutToAppear(){
    viewModel.readImageByDisk(this.app).then((res)=>{
      let options = {
        alphaType: 0,                     // 透明度
        editable: false,                  // 是否可编辑
        pixelFormat: 3,                   // 像素格式
        scaleMode: 1,                     // 缩略值
        size: { height: 100, width: 100}
      }
      let imageSource = image.createImageSource(res.buffer);
      if(imageSource)
        imageSource.createPixelMap(options).then((pixelMap) => {
          this.image = pixelMap
        })
    })
  }
  build(){
    Column(){
      Stack(){
        if (this.image){
          Image(this.image).height(100).width(100)
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
    }
  }
}



@Entry
@Component
struct AppPage {
  computer: ComputerDetails
  @State appList: NvApp[] = []
  aboutToAppear(){
    const params = router.getParams();
    viewModel.getComputerByUUid(params["uuid"]).then((d)=>{
      this.computer = d
      const appList =  d.appList
      this.appList = appList.filter((d) => d.appId != null);
    })
  }
  build(){
    Column(){
      NavTitle({ title: router.getParams()["computerName"] }).width("100%")
      Grid() {
        ForEach(this.appList, (d:NvApp)=>{
          GridItem(){
            AppView({app: d, computer: this.computer})
          }
        },(item) => JSON.stringify(item))
      }
      .rowsTemplate('1fr 1fr 1fr')
      .columnsTemplate('1fr 1fr 1fr')
    }.padding(10).height("100%").width("100%").backgroundColor($r("app.color.page_background"))
  }
}