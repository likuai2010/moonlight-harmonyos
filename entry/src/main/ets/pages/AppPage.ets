
import { NavTitle } from './compoments/Title'
@Entry
@Component
struct AppPage {
  appList: string[] = ["1","2","3", "4", "5"]
  build(){
    Column(){
      NavTitle({ title: 'Hello, World!' }).width("100%")
      Grid() {
        ForEach(this.appList, (d)=>{
          GridItem(){
            Text(d)
          }
        })
      }
      .rowsTemplate('1fr 1fr 1fr')
      .columnsTemplate('1fr 1fr 1fr')
    }.padding(10).height("100%").width("100%").backgroundColor($r("app.color.page_background"))
  }
}