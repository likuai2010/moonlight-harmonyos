import { AddressTuple, ComputerDetails, ComputerState } from './ComputerDetails';
import uri from '@ohos.uri';
import { NvHttp } from '../http/NvHttp';
import { MoonBridge } from '../nvstream/MoonBridge';
import limelightCertProvider from '../crypto/LimelightCryptoProvider'

class ComputerManagerViewModel {
  async addComputerBlocking(fakeDetails: ComputerDetails): Promise<boolean> {
    let detail = await this.pollComputer(fakeDetails)
    this.runPoll(detail, true, 0);
    if (detail.state == ComputerState.ONLINE) {
      //addTuple(fakeDetails);
      return true;
    }
    else {
      return false;
    }
    return false
  }
  async runPoll(detail: ComputerDetails,  newPc:boolean,  offlineCount:number){

  }
  async pollComputer(detail: ComputerDetails): Promise<ComputerDetails> {
    if (detail.localAddress) {
      const info = await this.tryPollIp(detail, detail.localAddress)
      if (info)
        return info;
    }
    if (detail.manualAddress) {
      const info = await this.tryPollIp(detail, detail.manualAddress)
      if (info)
        return info;
    }
    if (detail.remoteAddress) {
      const info = await this.tryPollIp(detail, detail.remoteAddress)
      if (info)
        return info;
    }
    if (detail.ipv6Address) {
      const info = await this.tryPollIp(detail, detail.ipv6Address)
      return info;
    }
    return null;
  }

  async tryPollIp(details: ComputerDetails, address: AddressTuple): Promise<ComputerDetails> {
    // If the current address's port number matches the active address's port number, we can also assume
    // the HTTPS port will also match. This assumption is currently safe because Sunshine sets all ports
    // as offsets from the base HTTP port and doesn't allow custom HttpsPort responses for WAN vs LAN.
    const portMatchesActiveAddress = details.state == ComputerState.ONLINE &&
    details.activeAddress != null && address.port == details.activeAddress.port;
    const http = new NvHttp(address, portMatchesActiveAddress ? details.httpsPort : 0, null, limelightCertProvider);
    // If this PC is currently online at this address, extend the timeouts to allow more time for the PC to respond.
    const isLikelyOnline = details.state == ComputerState.ONLINE && address === details.activeAddress;
    try {
      const newDetails = await http.getComputerDetails(isLikelyOnline);
      // Check if this is the PC we expected
      if (newDetails.uuid == null) {
        return null;
      }
      // details.uuid can be null on initial PC add
      else if (details.uuid != null && details.uuid != newDetails.uuid) {
        // We got the wrong PC!
        return null;
      }
      return newDetails;
    } catch (e) {
    }
    return null;
  }

  async addPc(ip: string) {
    const details = new ComputerDetails();
    const url = this.parseRawUserInputToUri(ip);
    let wrongSiteLocal = false;
    let invalidInput = false;
    let success;
    if (url) {
      const host = url.host;
      let port = parseInt(url.port);
      // If a port was not specified, use the default
      if (port == -1) {
        port = NvHttp.DEFAULT_HTTP_PORT;
      }
      details.manualAddress = new AddressTuple(host, port);
      success = await this.addComputerBlocking(details);
    } else {
      success = false;
      invalidInput = true;
    }

    let portTestResult
    // Keep the SpinnerDialog open while testing connectivity
    if (!success && !wrongSiteLocal && !invalidInput) {
      // Run the test before dismissing the spinner because it can take a few seconds.
      //portTestResult = MoonBridge.testClientConnectivity("android.conntest.moonlight-stream.org", 443, MoonBridge.ML_PORT_FLAG_TCP_47984 | MoonBridge.ML_PORT_FLAG_TCP_47989);
    } else {
      // Don't bother with the test if we succeeded or the IP address was bogus
      portTestResult = MoonBridge.ML_TEST_RESULT_INCONCLUSIVE;
    }

  }

  parseRawUserInputToUri(rawUserInput) {
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