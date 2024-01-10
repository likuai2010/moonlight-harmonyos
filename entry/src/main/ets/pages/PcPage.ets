
import { Icon, MainTitle, NavTitle } from './compoments/Title'
import viewModel from '../entryability/computers/ComputerManagerViewModel'
import { ComputerDetails, ComputerState } from '../entryability/computers/ComputerDetails'
import promptAction from '@ohos.promptAction'
import { PairState } from '../entryability/http/PairingManager'


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

@Component
struct PcView {
  @State detail: ComputerDetails = new ComputerDetails()
  isLoading: Boolean = true
  @Builder
  pcMenu(){
    Menu(){
      MenuItem({content:"浏览游戏列表"})
      MenuItem({content:"测试网络连接"})
      MenuItem({content:"查看详情"}).onClick(()=>{
        promptAction.showDialog({ title:"查看详情",message: this.detail.toString(),})
      })
    }
  }
  aboutToAppear(){
    this.isLoading = true
  }
  build(){
    Column(){
      Stack({alignContent:Alignment.Center}){
        Icon({icon: $r('app.media.desktop_windows'), iconSize: 120})

        if (this.isLoading){
          LoadingProgress().width(50).height(50).color(Color.White).offset({y:-10})
        } else {
          if (this.detail.state == ComputerState.ONLINE){
              if (this.detail.pairState != PairState.PAIRED) {
                Icon({icon: $r('app.media.baseline_lock'), iconSize: 48}).offset({y:-10})
              }
          } else {
            Icon({icon: $r('app.media.baseline_warning'), iconSize: 48}).offset({y:-10})
          }
        }
      }
      Text(this.detail.name || "-").fontColor(Color.White)
    }.onClick( () => {

      if(this.detail.state == ComputerState.ONLINE && this.detail.pairState != PairState.PAIRED){
          promptAction.showDialog({ title:"查看详情",message: this.detail.toString(),})
      }
      //router.pushUrl({url:"pages/AppPage"})
    }).bindContextMenu(this.pcMenu, ResponseType.LongPress)
  }
}
@Entry
@Component
struct PcPage {
  scroller: Scroller = new Scroller();
  @State pcList: Array<ComputerDetails> = []
  heightValue: number
  gridRowTemplate: string
  aboutToAppear() {
    viewModel.getComputerList().then((d)=>{
      this.pcList = d
    })
    viewModel.onDetailsUpdate((news: ComputerDetails)=>{
       var indexes = this.pcList.findIndex((d) => d.uuid == news.uuid)
        if (indexes < 0) {
          this.pcList.push(news)
        } else {
          this.pcList[indexes] = news
        }
        this.updateGrid()
    })
    this.updateGrid()
  }
  updateGrid(){
    var rows = Math.max(3, Math.round(this.pcList.length / 3))
    this.gridRowTemplate = '1fr '.repeat(rows);
    this.heightValue = rows * 180 - 8;
  }



  build(){
    Column(){
      MainTitle()
      Scroll(this.scroller) {
        Grid() {
          ForEach(this.pcList, (d)=>{
            GridItem(){
              PcView({detail: d, isLoading:true})
            }
          }, (item) => JSON.stringify(item))
        }
        .rowsTemplate(this.gridRowTemplate)
        .columnsTemplate('1fr 1fr 1fr')
        .height(this.heightValue)
      }.layoutWeight(1).scrollable(ScrollDirection.Vertical).edgeEffect(EdgeEffect.Spring)
    }.padding(20).height("100%").width("100%").backgroundColor($r("app.color.page_background"))
  }
}