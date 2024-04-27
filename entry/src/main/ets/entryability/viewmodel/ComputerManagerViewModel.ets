import { AddressTuple, ComputerDetails, ComputerState } from '../computers/ComputerDetails';
import uri from '@ohos.uri';
import common from '@ohos.app.ability.common';
import { NvHttp } from '../http/NvHttp';
import { MoonBridge } from '../nvstream/MoonBridge';
import limelightCertProvider, { accessFile, readFile, writeFile } from '../crypto/LimelightCryptoProvider'
import dbManger from '../computers/ComputerDatabaseManager'
import LimeLog from '../LimeLog';
import { NvApp } from '../http/NvApp';
import { getResStringByContext } from '../../uitls/ResString';
import FaultLogger from '@ohos.faultLogger';

export class ResultInfo{
    success: boolean
    title: String
    message:string

  constructor(success: boolean, title:string, message:string) {
    this.title = title
    this.success = success
    this.message = message
  }
}


class ComputerManagerViewModel {
  private context!: common.UIAbilityContext
  initContext(context: common.UIAbilityContext){
    this.context = context
    dbManger.init(this.context)
  }
  async getComputerList(): Promise<ComputerDetails[]> {
    return await dbManger.getAllComputers()
  }
  async getComputerByUUid(uuid: string): Promise<ComputerDetails>{
    const detail = await dbManger.getComputerByUUID(uuid)
    try {
      await this.runPoll(detail, false);
      await this.downloadImageToDisk(detail, detail.appList)
    }catch (e){
      LimeLog.error(`${e.stack}`)
    }
    return detail;
  }
  async batchPollComputerList(list: ComputerDetails[]){
     for(let d of list){
       await this.runPoll(d, false)
       let newDetails = new ComputerDetails();
       newDetails.update(d);
       newDetails.isLoading = false;
       this.onDetailsLinster(newDetails);
     }
  }

  onDetailsLinster!: (news: ComputerDetails) => void
  onDetailsUpdate(callBack: (ComputerDetails:ComputerDetails) => void) {
    this.onDetailsLinster = callBack
  }

  async addComputerBlocking(fakeDetails: ComputerDetails): Promise<boolean> {
    let detail = await this.pollComputer(fakeDetails)
    if (detail != null) {
      fakeDetails.update(detail)
      await this.runPoll(fakeDetails, true);
    }
    if (fakeDetails.state == ComputerState.ONLINE) {
      return true;
    }
    else {
      return false;
    }
    return false
  }

  async runPoll(details: ComputerDetails, newPc: boolean): Promise<boolean> {
    if (details.state != ComputerState.ONLINE) {
      // PC未在线, 获取PC状态
      const d = await this.pollComputer(details)
      if (d) {
        details.update(d)
      }
      if (!d || d.state != ComputerState.ONLINE) {
        details.state = ComputerState.OFFLINE
      }
      await dbManger.addOrUpdateComputer(details)
    } else { // PC在线,持久化
      if(!newPc){
        const existingComputer = await dbManger.getComputerByUUID(details.uuid);
        // Check if it's in the database because it could have been
        // removed after this was issued
        if (existingComputer == null) {
          // It's gone
          return false;
        }
        existingComputer.update(details);
        await dbManger.addOrUpdateComputer(existingComputer)
      }
      else{
        // If the active address is a site-local address (RFC 1918),
        // then use STUN to populate the external address field if
        // it's not set already.
        if (details.remoteAddress == null) {
          // TODO
        }
        await dbManger.addOrUpdateComputer(details)
      }
    }
    return true
  }

  async getImages(details: ComputerDetails, app: NvApp): Promise<Uint8Array>{
    const http = new NvHttp(details.activeAddress, details.httpsPort, true, limelightCertProvider);
    return await http.getBoxArt(app)
  }

  async downloadImageToDisk(details: ComputerDetails, appList:NvApp[]){
    for (let app of appList){
      if (this.context){
        if(!accessFile(this.context.filesDir + "/" + app.appId + ".png")){
          const bytes = await this.getImages(details, app)
          writeFile(this.context.filesDir + "/" + app.appId + ".png", bytes)
        }
      }
    }
  }
  async readImageByDisk(app: NvApp): Promise<Uint8Array | null>{
    if (this.context){
      return readFile(this.context.filesDir + "/" + app.appId + ".png")
    }
    return null;
  }

  async tryPollIp(details: ComputerDetails, address: AddressTuple): Promise<ComputerDetails | null> {
    // If the current address's port number matches the active address's port number, we can also assume
    // the HTTPS port will also match. This assumption is currently safe because Sunshine sets all ports
    // as offsets from the base HTTP port and doesn't allow custom HttpsPort responses for WAN vs LAN.
    const portMatchesActiveAddress = details.state == ComputerState.ONLINE &&
    details.activeAddress != null && address.port == details.activeAddress.port;
    const http = new NvHttp(address, portMatchesActiveAddress ? details.httpsPort : 0, details.serverCert, limelightCertProvider);
    // If this PC is currently online at this address, extend the timeouts to allow more time for the PC to respond.
    const isLikelyOnline = details.state == ComputerState.ONLINE && address === details.activeAddress;
    try {
      const newDetails = await http.getComputerDetails(isLikelyOnline);
      if (!newDetails)
        return null
      newDetails.rawAppList = await http.getAppListRaw()
      try {
        newDetails.appList = NvHttp.getAppListByReader(newDetails.rawAppList)
      }catch (e){
        LimeLog.error("get appList " + e.stack)
      }
      // Check if this is the PC we expected
      if (newDetails.uuid == null) {
        LimeLog.error("UUID is null ")
        return null;
      }
      // details.uuid can be null on initial PC add
      else if (details.uuid != null && details.uuid != '' && details.uuid != newDetails.uuid) {
        LimeLog.error("UUID is '' ")
        // We got the wrong PC!
        return null;
      }
      return newDetails;
    } catch (e) {
      LimeLog.error(`fetch ${address}detail failure: ${e}`)
      LimeLog.error(`${e.stack}`)
    }
    return null;
  }

  async pollComputer(detail: ComputerDetails): Promise<ComputerDetails | null> {
    if (detail.manualAddress) {
      const info = await this.tryPollIp(detail, detail.manualAddress)
      if (info) {
        info.activeAddress = detail.manualAddress
        return info;
      }
    }
    if (detail.localAddress) {
      const info = await this.tryPollIp(detail, detail.localAddress)
      if (info) {
        info.activeAddress = detail.localAddress
        return info;
      }
    }
    if (detail.remoteAddress) {
      const info = await this.tryPollIp(detail, detail.remoteAddress)
      if (info) {
        info.activeAddress = detail.remoteAddress
        return info;
      }
    }
    if (detail.ipv6Address) {
      const info = await this.tryPollIp(detail, detail.ipv6Address)
      if (info) {
        info.activeAddress = detail.ipv6Address
        return info;
      }
    }
    return null;
  }
  async addPc(ip: string): Promise<ResultInfo> {
    const details = new ComputerDetails();
    const url = this.parseRawUserInputToUri(ip);
    let wrongSiteLocal = false;
    let invalidInput = false;
    let success: boolean;
    if (url) {
      const host = url.host;
      let port = parseInt(url.port);
      // If a port was not specified, use the default
      if (port == -1) {
        port = NvHttp.DEFAULT_HTTP_PORT;
      }
      details.manualAddress = new AddressTuple(host, port);
      success = await this.addComputerBlocking(details);
      if (success && this.onDetailsLinster) {
        this.onDetailsLinster(details)
      }
    } else {
      success = false;
      invalidInput = true;
    }

    let portTestResult: number = 0
    // Keep the SpinnerDialog open while testing connectivity
    if (!success && !wrongSiteLocal && !invalidInput) {
      // Run the test before dismissing the spinner because it can take a few seconds.
      //portTestResult = MoonBridge.testClientConnectivity("android.conntest.moonlight-stream.org", 443, MoonBridge.ML_PORT_FLAG_TCP_47984 | MoonBridge.ML_PORT_FLAG_TCP_47989);
    } else {
      // Don't bother with the test if we succeeded or the IP address was bogus
      portTestResult = MoonBridge.ML_TEST_RESULT_INCONCLUSIVE;
    }
    let title = await getResStringByContext(this.context, $r('app.string.conn_error_title'))
    let message = ""
    if (invalidInput) {
      message = await getResStringByContext(this.context, $r('app.string.addpc_unknown_host'))
    }
    else if (wrongSiteLocal) {
      message = await this.context.resourceManager.getStringValue($r('app.string.addpc_wrong_sitelocal'))
    } else if (!success) {
      if (portTestResult != MoonBridge.ML_TEST_RESULT_INCONCLUSIVE && portTestResult != 0) {
        message = await this.context.resourceManager.getStringValue($r('app.string.nettest_text_blocked'))
      }
      else {
        message = await this.context.resourceManager.getStringValue($r('app.string.addpc_fail'))
      }
    }
    return new ResultInfo(success, title, message)
  }

  async delComputer(detail: ComputerDetails): Promise<boolean> {
    try {
      dbManger.deleteComputer(detail)
      // TODO 删除后立即查询数据库会导致无法删除, 这里延时100ms
      await this.sleep(100)
    } catch (e) {
      LimeLog.error("delComputer" + e.stack)
      return false
    }
    return true
  }
  sleep(ms: number):Promise<void> {
    return new Promise(resolve => setTimeout(resolve, ms));
  }
  parseRawUserInputToUri(rawUserInput: string) {
    try {
      const url = new uri.URI("moonlight://" + rawUserInput);
      if (url.host && url.host != '')
        return url;
    } catch (ignore) {
    }
    try {
      const url = new uri.URI("moonlight://[" + rawUserInput + "]");
      if (url.host && url.host != '')
        return url;
    } catch (ignore) {
    }
    return null;
  }
}

const vieModel = new ComputerManagerViewModel()

export default vieModel