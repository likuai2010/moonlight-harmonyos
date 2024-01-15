import Url from '@ohos.url'
import xml from '@ohos.xml';

import { PairingManager, PairState } from './PairingManager'
import { AddressTuple, ComputerDetails, ComputerState, makeTuple } from '../computers/ComputerDetails'
import util from '@ohos.util';
import { LimelightCertProvider } from '../crypto/LimelightCryptoProvider';
import { ConnectionContext } from '../nvstream/ConnectionContext';
import { bytesToHex, generateRandomBytes } from '../crypto/CryptoManager';
import { NvApp } from './NvApp';
import List from '@ohos.util.List';
import Stack from '@ohos.util.Stack';
import { CurlClient } from 'libentry.so';
import LimeLog from '../LimeLog';


export class NvHttp {
  private uniqueId: string;
  pm: PairingManager;
  private clientCert: LimelightCertProvider;

  private static readonly DEFAULT_HTTPS_PORT: number = 47984;
  public static readonly DEFAULT_HTTP_PORT: number = 47989;

  public static readonly SHORT_CONNECTION_TIMEOUT: number = 3;
  public static readonly LONG_CONNECTION_TIMEOUT: number = 5;
  public static readonly NO_READ_TIMEOUT: number = 9999;

  // Print URL and content to logcat on debug builds
  private static verbose: boolean = false; // Assuming DEBUG builds are for development

  private baseUrlHttp: Url.URL; // Make sure to import HttpUrl or adjust the type accordingly

  private httpsPort: number;
  private serverCert: any;


  constructor(address: AddressTuple,
              httpsPort: number,
              serverCert: any = null,
              cryptoProvider: LimelightCertProvider = null
  ) {
    this.uniqueId = '0123456789ABCDEF';
    this.httpsPort = httpsPort | 0;
    this.serverCert = serverCert;
    const addressString = address.address;
    try {
      if (addressString.indexOf(":") > -1 && addressString.indexOf(".") > -1) {
        this.baseUrlHttp = Url.URL.parseURL(`http://${addressString}`)
      } else {
        this.baseUrlHttp = Url.URL.parseURL(`http://${addressString}:${address.port}`)
      }
    } catch (e) {
      console.error("xxx => " + e)
    }
    this.clientCert = cryptoProvider
    this.pm = new PairingManager(this, cryptoProvider)
  }

  public async getServerCert(clientCert: Uint8Array): Promise<string> {
    const salt = await generateRandomBytes(16);
    const getCert = await this.executePairingCommand(
      "phrase=getservercert&salt=" + bytesToHex(salt) + "&clientcert=" + bytesToHex(clientCert),
      false);
    if (NvHttp.getXmlString(getCert, "paired", true) !== '1') {
      return PairState.FAILED;
    }
    return NvHttp.getXmlString(getCert, "plaincert", false);
    ;
  }


  static getXmlString(serverInfo: string, search: string, isRequired: boolean): string {
    // Implement your logic to extract XML data based on the key
    // You may use a library for XML parsing in TypeScript
    // Return the XML value or null if not found (and isRequired is false)
    let textEncoder = new util.TextEncoder();
    let arrbuffer = textEncoder.encodeInto(serverInfo);
    let that = new xml.XmlPullParser(arrbuffer.buffer, 'UTF-8');
    let v = '';
    that.parse({ supportDoctype: true, ignoreNameSpace: true, tokenValueCallbackFunction: (key, value) => {
      console.log(value.getText());
      let name = value.getName();
      let text = value.getText();
      if (search === v) {
        v = text
        return false;
      }
      if (name === search) {
        v = search
      }
      return true
    } })
    return v
  }

  public static getAppListByReader(r: string): NvApp[] {
    const appList = new List<NvApp>();
    let textEncoder = new util.TextEncoder();
    let arraybuffer = textEncoder.encodeInto(r);
    let that = new xml.XmlPullParser(arraybuffer.buffer, 'UTF-8');
    let stack = new Stack<string>()
    that.parse({ supportDoctype: true, ignoreNameSpace: true, tokenValueCallbackFunction: (key, value) => {
      let name = value.getName()
      if (name !== '') {
        stack.push(name)
        if (name === "App") {
          appList.add(new NvApp())
          stack.pop();
        }
      }
      const iswhie = value.isWhitespace()
      const we = value.getDepth()
      let textValue = value.getText()
      if (textValue !== '' && !textValue.match("\n")) {
        const app = appList.getLast()
        if (stack.peek() === "AppTitle") {
          app.appName = textValue;
          stack.pop();
        } else if (stack.peek() === "ID") {
          app.appId = parseInt(textValue);
          stack.pop();
        } else if (stack.peek() === 'IsHdrSupported') {
          app.hdrSupported = textValue === '1';
          stack.pop();
        }
      }
      if (stack.length > 2 && stack.peek() === 'root')
        return false
      return true
    } })
    return appList.convertToArray();
  }


  static verifyResponseStatus(reader: string, tag: string): string {
    return ""
  }

  public getServerVersion(serverInfo: string): string {
    // appversion is present in all supported GFE versions
    return NvHttp.getXmlString(serverInfo, "appversion", true);
  }

  async fetchPairState(): Promise<PairState> {
    return this.getPairState(await this.getServerInfo(true));
  }

  private getPairState(serverInfo: string): PairState {
    // appversion is present in all supported GFE versions
    return NvHttp.getXmlString(serverInfo, "PairStatus", true) == "1" ?
    PairState.PAIRED : PairState.NOT_PAIRED;
  }

  getMaxLumaPixelsH264(serverInfo: string): number {
    // MaxLumaPixelsH264 wasn't present in old GFE versions
    const str = NvHttp.getXmlString(serverInfo, "MaxLumaPixelsH264", false);
    return str !== null ? parseInt(str, 10) : 0;
  }

  getMaxLumaPixelsHEVC(serverInfo: string): number {
    // MaxLumaPixelsHEVC wasn't present in old GFE versions
    const str = NvHttp.getXmlString(serverInfo, "MaxLumaPixelsHEVC", false);
    return str !== null ? parseInt(str, 10) : 0;
  }

  // Possible meaning of bits
  // Bit 0: H.264 Baseline
  // Bit 1: H.264 High
  // ----
  // Bit 8: HEVC Main
  // Bit 9: HEVC Main10
  // Bit 10: HEVC Main10 4:4:4
  // Bit 11: ???
  getServerCodecModeSupport(serverInfo: string): number {
    // ServerCodecModeSupport wasn't present in old GFE versions
    const str = NvHttp.getXmlString(serverInfo, "ServerCodecModeSupport", false);
    return str !== null ? parseInt(str, 10) : 0;
  }

  public getServerMajorVersion(serverInfo: string): number {
    return this.getServerAppVersionQuad(serverInfo)[0];
  }

  public getServerAppVersionQuad(serverInfo: string): number[] {
    const serverVersion = this.getServerVersion(serverInfo);
    if (serverVersion == null) {
      throw new Error("Missing server version field");
    }
    const serverVersionSplit = serverVersion.split(".");
    if (serverVersionSplit.length != 4) {
      throw new Error("Malformed server version field: " + serverVersion);
    }
    return serverVersionSplit.map((d) => {
      return Number.parseInt(d)
    })
  }

  getGpuType(serverInfo: string): string {
    // ServerCodecModeSupport wasn't present in old GFE versions
    return NvHttp.getXmlString(serverInfo, "gputype", false);
  }

  getGfeVersion(serverInfo: string): string {
    // ServerCodecModeSupport wasn't present in old GFE versions
    return NvHttp.getXmlString(serverInfo, "GfeVersion", false);
  }

  supports4K(serverInfo: string): boolean {
    // Only allow 4K on GFE 3.x. GfeVersion wasn't present on very old versions of GFE.
    const gfeVersionStr = NvHttp.getXmlString(serverInfo, "GfeVersion", false);
    return gfeVersionStr !== null && !gfeVersionStr.startsWith("2.");
  }

  getCurrentGame(serverInfo: string): number {
    // GFE 2.8 started keeping currentgame set to the last game played. As a result, it no longer
    // has the semantics that its name would indicate. To contain the effects of this change as much
    // as possible, we'll force the current game to zero if the server isn't in a streaming session.
    if (NvHttp.getXmlString(serverInfo, "state", true).endsWith("_SERVER_BUSY")) {
      return parseInt(NvHttp.getXmlString(serverInfo, "currentgame", true), 10);
    } else {
      return 0;
    }
  }
  /* NOTE: Only use this function if you know what you're doing.
    * It's totally valid to have two apps named the same thing,
    * or even nothing at all! Look apps up by ID if at all possible
    * using the above function */
  public async getAppByName(appName: string): Promise<NvApp> {
    const appList = await this.getAppList();
    for (const appFromList of appList) {
      if (appFromList.appName.toLowerCase() == appName.toLowerCase()) {
        return appFromList;
      }
    }
    return null;
  }

  getHttpsPort(serverInfo: string): number {
    try {
      return parseInt(NvHttp.getXmlString(serverInfo, "HttpsPort", true), 10);
    } catch (e) {
      console.error(e);
      return NvHttp.DEFAULT_HTTPS_PORT;
    }
  }

  getExternalPort(serverInfo: string): number {
    // This is an extension which is not present in GFE. It is present for Sunshine to be able
    // to support dynamic HTTP WAN ports without requiring the user to manually enter the port.
    try {
      return parseInt(NvHttp.getXmlString(serverInfo, "ExternalPort", true), 10);
    } catch (e) {
      // Expected on non-Sunshine servers
      return NvHttp.DEFAULT_HTTP_PORT
    }
  }

  async getComputerDetails(likelyOnline: boolean): Promise<ComputerDetails> {
    return this.getComputerDetailsByInfo(await this.getServerInfo(likelyOnline));
  }

  getComputerDetailsByInfo(serverInfo: string): ComputerDetails {
    const details = new ComputerDetails();
    details.name = NvHttp.getXmlString(serverInfo, "hostname", false) || "UNKNOWN";
    // UUID is mandatory to determine which machine is responding
    details.uuid = NvHttp.getXmlString(serverInfo, "uniqueid", true);
    details.httpsPort = this.getHttpsPort(serverInfo);
    details.macAddress = NvHttp.getXmlString(serverInfo, "mac", false);
    // FIXME: Do we want to use the current port?
    details.localAddress = makeTuple(NvHttp.getXmlString(serverInfo, "LocalIP", false), NvHttp.DEFAULT_HTTP_PORT);
    // This is missing on recent GFE versions, but it's present on Sunshine
    details.externalPort = this.getExternalPort(serverInfo);
    details.remoteAddress = makeTuple(NvHttp.getXmlString(serverInfo, "ExternalIP", false), details.externalPort);

    details.pairState = this.getPairState(serverInfo);
    details.runningGameId = this.getCurrentGame(serverInfo);
    // The MJOLNIR codename was used by GFE but never by any third-party server
    details.nvidiaServer = NvHttp.getXmlString(serverInfo, "state", true).includes("MJOLNIR");

    // We could reach it, so it's online
    details.state = ComputerState.ONLINE;

    return details;
  }



  async executePairingCommand(additionalArguments: String, enableReadTimeout: boolean): Promise<string> {
    return this.openHttpConnectionToString(
      this.baseUrlHttp, "pair", "devicename=roth&updateState=1&" + additionalArguments, enableReadTimeout ? NvHttp.LONG_CONNECTION_TIMEOUT : NvHttp.NO_READ_TIMEOUT,);
  }

  async executePairingChallenge(): Promise<string> {
    return await this.openHttpConnectionToString(this.getHttpsUrl(true), "pair", "devicename=roth&updateState=1&phrase=pairchallenge", NvHttp.LONG_CONNECTION_TIMEOUT);
  }

  private getCompleteUrl(baseUrl: Url.URL, path: string, query: string): string {
    let url = Url.URL.parseURL(
      baseUrl +
      path +
      `?uniqueid=${this.uniqueId}&uuid=${util.generateRandomUUID()}&${query || ""}`
    )
    return url.href
  }

  public async quitApp(): Promise<boolean> {
    const xmlStr = await this.openHttpConnectionToString(this.getHttpsUrl(true), "cancel");
    if (NvHttp.getXmlString(xmlStr, "cancel", true) === '0') {
      return false;
    }
    // Newer GFE versions will just return success even if quitting fails
    // if we're not the original requestor.
    if (this.getCurrentGame(await this.getServerInfo(true)) != 0) {
      // Generate a synthetic GfeResponseException letting the caller know
      // that they can't kill someone else's stream.
      throw new Error("599");
    }
    return true;
  }

  async openHttpConnectionToUint8Array(baseUrl: Url.URL, path: string, query: string = null, timeout: number = 0): Promise<Uint8Array> {
    var httpClient = new CurlClient()
    try {
      let url = this.getCompleteUrl(baseUrl, path, query)
      let clientPath = null;
      let keyPath = null;
      if (baseUrl.protocol.startsWith('https')) {
        clientPath = this.clientCert.certPath
        keyPath = this.clientCert.keyPath
      }
      const response = await httpClient.get(url, timeout == 0 ? 9999 : 30, clientPath, keyPath)
      httpClient.close()
      return response
    } catch (e) {
      httpClient.close()
      LimeLog.error(`${e}`)
    }
    return null;
  }
  async openHttpConnectionToString(baseUrl: Url.URL, path: string, query: string = null, timeout: number = 0): Promise<string> {
    var httpClient = new CurlClient()
    try {
      let url = this.getCompleteUrl(baseUrl, path, query)
      let clientPath = null;
      let keyPath = null;
      if (baseUrl.protocol.startsWith('https')) {
        clientPath = this.clientCert.certPath
        keyPath = this.clientCert.keyPath
      }
      const response = await httpClient.get(url, timeout == 0 ? 360 : timeout, clientPath, keyPath)
      const td = util.TextDecoder.create('utf-8', { ignoreBOM : true })
      httpClient.close()
      return td.decodeWithStream(response)
    } catch (e) {
      httpClient.close()
      LimeLog.error(`${e}`)
    }
    return null;
  }

  public async unpair(): Promise<void> {
  }

  public async launchApp(context: ConnectionContext, verb: string, appId: number, enableHdr: boolean): Promise<boolean> {
    // Using an FPS value over 60 causes SOPS to default to 720p60,
    // so force it to 0 to ensure the correct resolution is set. We
    // used to use 60 here but that locked the frame rate to 60 FPS
    // on GFE 3.20.3.
    const fps = context.isNvidiaServerSoftware && context.streamConfig.launchRefreshRate > 60 ?
      0 : context.streamConfig.launchRefreshRate;

    let enableSops = context.streamConfig.sops;
    if (context.isNvidiaServerSoftware) {
      // Using an unsupported resolution (not 720p, 1080p, or 4K) causes
      // GFE to force SOPS to 720p60. This is fine for < 720p resolutions like
      // 360p or 480p, but it is not ideal for 1440p and other resolutions.
      // When we detect an unsupported resolution, disable SOPS unless it's under 720p.
      // FIXME: Detect support resolutions using the serverinfo response, not a hardcoded list
      if (context.negotiatedWidth * context.negotiatedHeight > 1280 * 720 &&
      context.negotiatedWidth * context.negotiatedHeight != 1920 * 1080 &&
      context.negotiatedWidth * context.negotiatedHeight != 3840 * 2160) {
        console.info("Disabling SOPS due to non-standard resolution: " + context.negotiatedWidth + "x" + context.negotiatedHeight);
        enableSops = false;
      }
    }
    const query = "appid=" + appId +
    "&mode=" + context.negotiatedWidth + "x" + context.negotiatedHeight + "x" + fps +
    "&additionalStates=1&sops=" + (enableSops ? 1 : 0) +
    "&rikey=" + bytesToHex(context.riKey) +
    "&rikeyid=" + context.riKeyId +
    (!enableHdr ? "" : "&hdrMode=1&clientHdrCapVersion=0&clientHdrCapSupportedFlagsInUint32=0&clientHdrCapMetaDataId=NV_STATIC_METADATA_TYPE_1&clientHdrCapDisplayData=0x0x0x0x0x0x0x0x0x0x0") +
    "&localAudioPlayMode=" + (context.streamConfig.playLocalAudio ? 1 : 0) +
    "&surroundAudioInfo=" + context.streamConfig.audioConfiguration +
    "&remoteControllersBitmap=" + context.streamConfig.attachedGamepadMask +
    "&gcmap=" + context.streamConfig.attachedGamepadMask +
    "&gcpersist=" + (context.streamConfig.persistGamepadsAfterDisconnect ? 1 : 0)
    const xmlStr = await this.openHttpConnectionToString(
      this.getHttpsUrl(true),
      verb,
      query, NvHttp.LONG_CONNECTION_TIMEOUT
    );
    if ((verb === "launch" && NvHttp.getXmlString(xmlStr, "gamesession", true) !== "0" ||
    (verb === "resume" && NvHttp.getXmlString(xmlStr, "resume", true) !== "0"))) {
      // sessionUrl0 will be missing for older GFE versions
      context.rtspSessionUrl = NvHttp.getXmlString(xmlStr, "sessionUrl0", false);
      return true;
    }
    else {
      return false;
    }
  }


  private getHttpsUrl(likelyOnline: boolean): Url.URL {
    if (this.httpsPort == 0) {
      // Fetch the HTTPS port if we don't have it already
      // this.httpsPort = getHttpsPort(openHttpConnectionToString(likelyOnline ? httpClientLongConnectTimeout : httpClientShortConnectTimeout,
      //   baseUrlHttp, "serverinfo"));
      this.httpsPort = 47984
    }
    return Url.URL.parseURL(`https://${this.baseUrlHttp.hostname}:${this.httpsPort}`)
  }

  public getAppListRaw(): Promise<string> {
    return this.openHttpConnectionToString(this.getHttpsUrl(true), "applist");
  }

  async getAppList(): Promise<NvApp[]> {
    return NvHttp.getAppListByReader(await this.getAppListRaw())
  }

  async getBoxArt(app: NvApp): Promise<Uint8Array> {
      return await this.openHttpConnectionToUint8Array(this.getHttpsUrl(true), "appasset", "appid=" + app.appId+ "&AssetType=2&AssetIdx=0",  NvHttp.LONG_CONNECTION_TIMEOUT);
  }

  async getServerInfo(likelyOnline: boolean): Promise<string> {
    if (this.serverCert != null) {
      let info = await this.openHttpConnectionToString(this.getHttpsUrl(likelyOnline), "serverinfo", null , NvHttp.SHORT_CONNECTION_TIMEOUT)
      if (!info) {
        info = await this.openHttpConnectionToString(this.baseUrlHttp, "serverinfo", null, NvHttp.SHORT_CONNECTION_TIMEOUT)
      }
      if(info)
        this.getServerVersion(info)
      return info
    }
    else {
      return await this.openHttpConnectionToString(this.baseUrlHttp, "serverinfo", null,  NvHttp.SHORT_CONNECTION_TIMEOUT)
    }
  }
}
