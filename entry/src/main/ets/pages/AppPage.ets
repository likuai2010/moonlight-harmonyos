
import router from '@ohos.router'
import viewModel from '../entryability/computers/ComputerManagerViewModel'
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
    viewModel.getImages(this.computer, this.app).then((res)=>{
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
        Image(this.image).height(100).width(100)
        Text(this.app.appName).fontColor(Color.White)
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
      const appList = NvHttp.getAppListByReader(d.rawAppList)
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