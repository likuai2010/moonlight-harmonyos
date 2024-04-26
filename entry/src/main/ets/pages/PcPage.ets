import { Icon, MainTitle } from './compoments/Title'
import viewModel from '../entryability/ComputerManagerViewModel'
import { ComputerDetails, ComputerState } from '../entryability/computers/ComputerDetails'
import promptAction from '@ohos.promptAction'
import { PairState } from '../entryability/http/PairingManager'
import limelightCertProvider from '../entryability/crypto/LimelightCryptoProvider'
import { NvHttp } from '../entryability/http/NvHttp'
import router from '@ohos.router'
import { Alert, PrivacyConfirm } from './compoments/Loading'
async function getResString(com: any, r: Resource): Promise<string> {
  return await getContext(com).resourceManager.getStringValue(r)
}



@Component
struct PcView {
  @State detail: ComputerDetails = new ComputerDetails()
  onDelete!: (ComputerDetails:ComputerDetails) => void
  @Builder
  pcMenu() {
    Menu() {
      // if (this.detail.pairState == PairState.PAIRED) {
      //   // MenuItem({ content: "浏览游戏列表" }).onChange((e)=>{
      //   //   this.doAppList(this.detail, false, false)
      //   // })
      // } else {
      //   MenuItem({ content: "和电脑配对" }).onClick(()=>{
      //     this.doPair()
      //   })
      // }
      // MenuItem({ content: "测试网络连接" })
      MenuItem({ content: "删除电脑" }).onClick(() => {
        this.onDelete(this.detail)
      })
      MenuItem({ content: "查看详情" }).onClick((e) => {
        let alertDialog = new CustomDialogController({
          builder: Alert({
            title: "查看详情",
            message: this.detail.toString(),
          }),
          autoCancel: true,
          alignment: DialogAlignment.Center,
          customStyle: true
        })
        alertDialog.open()
      })
    }
  }
  click() {
    if (this.detail.state == ComputerState.ONLINE && this.detail.pairState != PairState.PAIRED) {
      this.doPair()
    } else if(this.detail.state == ComputerState.ONLINE) {
      this.doAppList(this.detail, false, false)
    } else {

    }
  }

  async doPair() {
    let message: string | null;
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
      const pinStr = "1234";
      const dialogText =  await getResString(this, $r('app.string.pair_pairing_msg')) + " " + pinStr + "\n\n" + await getResString(this, $r('app.string.pair_pairing_help'))
      let alertDialog = new CustomDialogController({
        builder: Alert({
          title: "配对中",
          message: dialogText,
        }),
        autoCancel: false,
        alignment: DialogAlignment.Center,
        customStyle: true
      })
      alertDialog.open()
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
        this.detail.serverCert = true
        viewModel.runPoll(this.detail, false)
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
      alertDialog.close()
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
  }

  build() {
    Column() {
      Stack({ alignContent: Alignment.Center }) {
        Icon({ icon: $r('app.media.desktop_windows'), iconSize: 120 })
        if (this.detail.isLoading) {
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



import dataPreferences from '@ohos.data.preferences';
@Entry
@Component
struct PcPage {
  scroller: Scroller = new Scroller();
  @State pcList: ComputerDetails[] = []
  heightValue?: number
  gridRowTemplate?: string

  privacyDialogController: CustomDialogController =  new CustomDialogController({
    builder: PrivacyConfirm({ onConfirm:() => {
      dataPreferences.getPreferences(getContext(this), "appSettings").then((pref: dataPreferences.Preferences)=>{
        pref.put("privacyConfirm", true)
        pref.flush()
      })
    }}),
    autoCancel: false,
    alignment: DialogAlignment.Center,
    customStyle: true
  })
  onPageShow(){
    dataPreferences.getPreferences(getContext(this), "appSettings").then((pref: dataPreferences.Preferences)=>{
      pref.get("privacyConfirm", false).then((r:boolean)=>{
        if(!r){
          this.privacyDialogController.open()
        }
      })
    })
    this.fetchDetails()
  }

  aboutToAppear() {
    viewModel.onDetailsUpdate((news: ComputerDetails) => {
      let indexes = this.pcList.findIndex((d) => d.uuid == news.uuid)
      if (indexes > -1) {
        this.pcList[indexes] = news
      }
      this.updateGrid()
    })
    this.updateGrid()
  }
  fetchDetails(){
    viewModel.getComputerList().then((list) => {
      for(let d of list){
        d.isLoading = true
      }
      this.pcList = list
      viewModel.batchPollComputerList(list)
    })
  }

  updateGrid() {
    let rows = Math.max(3, Math.round(this.pcList.length / 3))
    this.gridRowTemplate = '1fr '.repeat(rows);
    this.heightValue = rows * 192 - 8;
  }

  build() {
    Column() {
      MainTitle()
      Scroll(this.scroller) {
        Grid() {
          ForEach(this.pcList, (d: ComputerDetails) => {
            GridItem() {
              PcView({ detail: d, onDelete:(dev)=>{
                viewModel.delComputer(dev).then((result)=>{
                  if (result)
                    this.fetchDetails()
                  else
                    promptAction.showToast({message: "删除失败"})
                })
              }})
            }
          }, (item: ComputerDetails) => JSON.stringify(item))
        }.onKeyEvent((e)=>{
          console.log(e.keyCode+"");
        }).onMouse((e)=>{
          console.log(e.button+"");
        })
        .rowsTemplate(this.gridRowTemplate)
        .columnsTemplate('1fr 1fr 1fr')
        .height(this.heightValue)
      }.layoutWeight(1).scrollable(ScrollDirection.Vertical)
    }.padding(20).height("100%").width("100%").backgroundColor($r("app.color.page_background"))
  }
}