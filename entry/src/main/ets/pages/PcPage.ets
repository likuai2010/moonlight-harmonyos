import { Icon, MainTitle, NavTitle } from './compoments/Title'
import viewModel from '../entryability/ComputerManagerViewModel'
import { ComputerDetails, ComputerState } from '../entryability/computers/ComputerDetails'
import promptAction from '@ohos.promptAction'
import { PairState } from '../entryability/http/PairingManager'
import limelightCertProvider from '../entryability/crypto/LimelightCryptoProvider'
import { NvHttp } from '../entryability/http/NvHttp'
import router from '@ohos.router'

async function getResString(com: any, r: Resource): Promise<string> {
  return await getContext(com).resourceManager.getStringValue(r)
}
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
  @State isLoading: string = "false"
  onDelete: (ComputerDetails) => void
  @Builder
  pcMenu() {
    Menu() {
      if (this.detail.pairState == PairState.PAIRED) {
        MenuItem({ content: "浏览游戏列表" }).onClick(()=>{
          this.doAppList(this.detail, false, false)
        })
      } else {
        MenuItem({ content: "和电脑配对" }).onTouch(()=>{

        })
      }
      MenuItem({ content: "测试网络连接" })
      MenuItem({ content: "删除电脑" }).onClick(() => {
        this.onDelete(this.detail)
      })
      MenuItem({ content: "查看详情" }).onClick(() => {
        promptAction.showDialog({ title: "查看详情", message: this.detail.toString(), })
      })
    }
  }

  click() {
    if (this.detail.state == ComputerState.ONLINE && this.detail.pairState != PairState.PAIRED) {
      this.doPair()
    } else {
      this.doAppList(this.detail, false, false)
      //promptAction.showDialog({ title: "查看详情", message: this.detail.toString(),})
    }

  }

  async doPair() {
    let message;
    let success = false;
    const computer = this.detail
    const httpConn = new NvHttp(this.detail.activeAddress,
      this.detail.httpsPort, null,
      limelightCertProvider);
    const state = await httpConn.fetchPairState()
    if (state == PairState.PAIRED) {
      // Don't display any toast, but open the app list
      message = null;
      success = true;
    } else {
      const pinStr = "12345";
      promptAction.showDialog({
        title: "配对中",
        message: await getResString(this, $r('app.string.pair_pairing_msg')) + " " + pinStr + "\n\n" + await getResString(this, $r('app.string.pair_pairing_help'))
      })
      const pm = httpConn.pm
      const pairState = await pm.pair(await httpConn.getServerInfo(true), pinStr);
      if (pairState == PairState.PIN_WRONG) {
        message = await getResString(this, $r('app.string.pair_incorrect_pin'))
      }
      else if (pairState == PairState.FAILED) {
        if (computer.runningGameId != 0) {
          message = await getResString(this, $r('app.string.pair_pc_ingame'))
        }
        else {
          message = await getResString(this, $r('app.string.pair_fail'))
        }
      }
      else if (pairState == PairState.ALREADY_IN_PROGRESS) {
          message = await getResString(this, $r('app.string.pair_already_in_progress'))
      }
      else if (pairState == PairState.PAIRED) {
        // Just navigate to the app view without displaying a toast
        message = null;
        success = true;

        // Pin this certificate for later HTTPS use
        //viewModel.getComputer(computer.uuid).serverCert = pm.getPairedCert();

        // Invalidate reachability information after pairing to force
        // a refresh before reading pair state again
        //viewModel.invalidateStateForComputer(computer.uuid);
      }
      else {
        // Should be no other values
        message = null;
      }

      if (message)
        promptAction.showToast({ message: message })
      if (success){
        this.doAppList(computer, true, false);
      }
    }
  }
  doAppList(computer: ComputerDetails , newlyPaired:boolean,  showHiddenGames:boolean){
    router.pushUrl({url:"pages/AppPage", params: { uuid:  this.detail.uuid, computerName: this.detail.name, rawAppList: this.detail.rawAppList}})
  }
  aboutToAppear() {
    if (this.detail.state != ComputerState.ONLINE) {
      this.isLoading = "true"
      viewModel.runPoll(this.detail, false).then(() => {
        this.isLoading = "false"
      }).catch((e) => {
        promptAction.showToast({ message: e })
        this.isLoading = "false"
      })
    }
  }

  build() {
    Column() {
      Stack({ alignContent: Alignment.Center }) {
        Icon({ icon: $r('app.media.desktop_windows'), iconSize: 120 })
        if (this.isLoading == "true") {
          LoadingProgress().width(50).height(50).color(Color.White).offset({ y: -10 })
        } else {
          if (this.detail.state == ComputerState.ONLINE) {
            if (this.detail.pairState != PairState.PAIRED) {
              Icon({ icon: $r('app.media.baseline_lock'), iconSize: 48 }).offset({ y: -10 })
            }
          } else {
            Icon({ icon: $r('app.media.baseline_warning'), iconSize: 48 }).offset({ y: -10 })
          }
        }
      }
      Text(this.detail.name || "-").fontColor(Color.White)
    }.onClick(() => {
      this.click()
    }).bindContextMenu(this.pcMenu, ResponseType.LongPress)
  }
}

@Entry
@Component
struct PcPage {
  scroller: Scroller = new Scroller();
  @State pcList: ComputerDetails[] = []
  heightValue: number
  gridRowTemplate: string

  aboutToAppear() {
    viewModel.getComputerList().then((d) => {
      this.pcList = d
    })
    viewModel.onDetailsUpdate((news: ComputerDetails) => {
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

  updateGrid() {
    var rows = Math.max(3, Math.round(this.pcList.length / 3))
    this.gridRowTemplate = '1fr '.repeat(rows);
    this.heightValue = rows * 192 - 8;
  }

  build() {
    Column() {
      MainTitle()
      Scroll(this.scroller) {
        Grid() {
          ForEach(this.pcList, (d) => {
            GridItem() {
              PcView({ detail: d, onDelete:(d)=>{

              }})
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