
import router from '@ohos.router'
import { Icon, MainTitle, NavTitle } from './compoments/Title'
import promptAction from '@ohos.promptAction'
import { showSubWindow } from '../entryability/EntryAbility'
import viewModel from '../entryability/computers/ComputerManagerViewModel'

@Component
struct PcView {
  viewModel = viewModel
  build(){
    Column(){
      Stack({alignContent:Alignment.Center}){
        Icon({icon: $r('app.media.desktop_windows'), iconSize: 120})
        Icon({icon: $r('app.media.baseline_lock'), iconSize: 48}).offset({y:-10})
      }
      Text("11").fontColor(Color.White)
    }
  }
}
@Entry
@Component
struct PcPage {
  scroller: Scroller = new Scroller();
  pcList: string[] = ["1","2","3", "4", "5","6","7","8","9","10","10","10"]

  @Builder
  pcMenu(){
    Menu(){
      MenuItem({content:"111"})
      MenuItem({content:"2222"})
    }
  }

  build(){
    Column(){
      MainTitle()
      Scroll(this.scroller) {
        Grid() {
          ForEach(this.pcList, (d)=>{
            GridItem(){
              PcView()
            }.onClick( () => {
              showSubWindow()
              // promptAction.showDialog({
              //   title: '连接错误',
              //   message: 'Message InfoMessage InfoMessage InfoMessage InfoMessage InfoMessage Info',
              //   buttons: [
              //     {
              //       text: '确定',
              //       color: '#000000',
              //     }
              //   ]
              // }, (err, data) => {
              //   if (err) {
              //     console.info('showDialog err: ' + err);
              //     return;
              //   }
              //   console.info('showDialog success callback, click button: ' + data.index);
              // });

              //router.pushUrl({url:"pages/AppPage"})
            }).bindContextMenu(this.pcMenu, ResponseType.LongPress)
          })
        }
        .rowsTemplate('1fr 1fr 1fr')
        .columnsTemplate('1fr 1fr 1fr')
      }.layoutWeight(1).scrollable(ScrollDirection.Vertical).edgeEffect(EdgeEffect.Spring)
    }.padding(20).height("100%").width("100%").backgroundColor($r("app.color.page_background"))
  }
}