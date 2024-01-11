
import router from '@ohos.router'
import viewModel from '../entryability/computers/ComputerManagerViewModel'
import { ComputerDetails } from '../entryability/computers/ComputerDetails'
import { NvHttp } from '../entryability/http/NvHttp'
import { Icon, NavTitle } from './compoments/Title'
import { NvApp } from '../entryability/http/NvApp'



@Component
struct AppView {
  computer: ComputerDetails
  app: NvApp
  aboutToAppear(){
    viewModel.getImages(this.computer, this.app).then((e)=>{
      console.log(e);
    })
  }
  build(){
    Column(){
      Icon({icon: $r('app.media.desktop_windows')})
      Text(this.app.appName).fontColor(Color.White)
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