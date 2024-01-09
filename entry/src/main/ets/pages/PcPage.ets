
import router from '@ohos.router'
import { Icon, MainTitle, NavTitle } from './compoments/Title'
@Entry
@Component
struct PcPage {
  pcList: string[] = ["1","2","3", "4", "5"]
  build(){
    Column(){
      MainTitle()
      Grid() {
        ForEach(this.pcList, (d)=>{
          GridItem(){
            Text(d)
          }.onClick(()=>{
            router.pushUrl({url:"pages/AppPage"})
          })
        })
      }
      .rowsTemplate('1fr 1fr 1fr')
      .columnsTemplate('1fr 1fr 1fr')
    }.padding(10).height("100%").width("100%").backgroundColor(Color.Gray)
  }
}