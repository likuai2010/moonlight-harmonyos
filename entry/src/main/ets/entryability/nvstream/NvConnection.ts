import { MoonBridgeNapi, VideoStatus } from 'libentry.so';
import { ConnectionContext } from './ConnectionContext';
import { AddressTuple, ComputerDetails } from '../computers/ComputerDetails';
import { NvHttp } from '../http/NvHttp';
import { generateRiKey, generateRiKeyId } from '../crypto/CryptoManager';
import {  MoonBridge } from './MoonBridge';
import { StreamConfiguration } from './StreamConfiguration'
import { NvConnectionListener } from './ConnetionListener';
import buffer from '@ohos.buffer';
import { LimelightCertProvider } from '../crypto/LimelightCryptoProvider';
import hilog from '@ohos.hilog';
import LimeLog from '../LimeLog';

export class NvConnection {
  api: MoonBridgeNapi = new MoonBridgeNapi()
  context: ConnectionContext
  uniqueId: string

  constructor(host: AddressTuple, httpsPort: number, uniqueId: string, config: StreamConfiguration) {
    this.uniqueId = uniqueId
    this.context = new ConnectionContext();
    this.context.serverAddress = host;
    this.context.httpsPort = httpsPort;
    this.context.streamConfig = config;

    this.context.videoCapabilities = 16777221
  }

  public async initKey(){
    this.context.riKey = await generateRiKey()
    this.context.riKeyId = await generateRiKeyId()
  }

  public async start(connectionListener: NvConnectionListener) {
    const context = this.context;
    this.context.connListener = connectionListener;
    const appName = this.context.streamConfig.app.appName;
    this.context.connListener.stageStarting(appName);
    try {

      if (!await this.startApp()) {
        this.context.connListener.stageFailed(appName, 0, 0);
        return;
      }
      this.context.connListener.stageComplete(appName);
    } catch (e) {
      this.context.connListener.displayMessage(e.message);
      this.context.connListener.stageFailed(appName, 0, -1);
      return;
    }
    let rikeyId:Uint8Array = null;
    try {
      const dd = buffer.alloc(16)
      dd.writeUInt32BE(context.riKeyId)
      rikeyId = new Uint8Array(dd.buffer)
    } catch (e) {
      this.context.connListener.displayMessage(e.message);
      this.context.connListener.stageFailed(appName, 0, 0);
      return;
    }
    // Moonlight-core is not thread-safe with respect to connection start and stop, so
    // we must not invoke that functionality in parallel.
    //MoonBridge.api.setupBridge(videoDecoderRenderer, audioRenderer, connectionListener);
    LimeLog.info("startConnection")
    try {
      const ret = this.api.startConnection(
        context.serverAddress.toAddress(),
        context.serverAppVersion,
        context.serverGfeVersion,
        context.rtspSessionUrl,
        context.serverCodecModeSupport,
        context.negotiatedWidth,
        context.negotiatedHeight,
        context.streamConfig.refreshRate,
        context.streamConfig.bitrate,
        context.negotiatedPacketSize,
        context.negotiatedRemoteStreaming,
        context.streamConfig.audioConfiguration,
        context.streamConfig.supportedVideoFormats,
        context.streamConfig.clientRefreshRateX100,
        context.streamConfig.encryptionFlags,
        context.riKey, rikeyId,
        context.videoCapabilities,
        context.streamConfig.colorSpace, context.streamConfig.colorRange
      )
      // const ret = this.api.startConnection("192.168.3.5",
      //                       "7.1.431.-1", "3.23.0.74", "rtsp://192.168.3.5:48010",
      //                       197377,
      //                       1280, 720,
      //                       60, 10000,
      //                       1392, 2,
      //                       197322,
      //                       1,
      //                       6000,
      //                       1,
      //             context.riKey, rikeyId,
      //                       16777216,
      //                       1,
      //                       0);
      LimeLog.info("ret=>"+ret)
    }catch (e){
      LimeLog.info("err=>"+e)
    }


  }

  async startApp() {
    const h = new NvHttp(this.context.serverAddress, this.context.httpsPort, this.uniqueId, new LimelightCertProvider());
    const serverInfo = await h.getServerInfo(true);
    const context = this.context
    context.serverAppVersion = h.getServerVersion(serverInfo);
    if (context.serverAppVersion == null) {
      context.connListener.displayMessage("Server version malformed");
      return false;
    }
    const details = h.getComputerDetailsByInfo(serverInfo);
    context.isNvidiaServerSoftware = details.nvidiaServer;
    // May be missing for older servers
    context.serverGfeVersion = h.getGfeVersion(serverInfo);
    // if (h.getPairState(serverInfo) != PairState.PAIRED) {
    //   this.context.connListener.displayMessage("Device not paired with computer");
    //   return false;
    // }
    context.serverCodecModeSupport = h.getServerCodecModeSupport(serverInfo);
    context.negotiatedHdr = (this.context.streamConfig.supportedVideoFormats & MoonBridge.VIDEO_FORMAT_MASK_10BIT) != 0;
    if ((context.serverCodecModeSupport & 0x20200) == 0 && this.context.negotiatedHdr) {
      context.connListener.displayTransientMessage("Your PC GPU does not support streaming HDR. The stream will be SDR.");
      context.negotiatedHdr = false;
    }
    //
    // Decide on negotiated stream parameters now
    //

    // Check for a supported stream resolution
    if ((context.streamConfig.width > 4096 || context.streamConfig.height > 4096) &&
    (h.getServerCodecModeSupport(serverInfo) & 0x200) == 0 && context.isNvidiaServerSoftware) {
      context.connListener.displayMessage("Your host PC does not support streaming at resolutions above 4K.");
      return false;
    }
    else if ((context.streamConfig.width > 4096 || context.streamConfig.height > 4096) &&
    (context.streamConfig.supportedVideoFormats & ~MoonBridge.VIDEO_FORMAT_MASK_H264) == 0) {
      context.connListener.displayMessage("Your streaming device must support HEVC or AV1 to stream at resolutions above 4K.");
      return false;
    }
    else if (context.streamConfig.height >= 2160 && !h.supports4K(serverInfo)) {
      // Client wants 4K but the server can't do it
      context.connListener.displayTransientMessage("You must update GeForce Experience to stream in 4K. The stream will be 1080p.");

      // Lower resolution to 1080p
      context.negotiatedWidth = 1920;
      context.negotiatedHeight = 1080;
    }
    else {
      // Take what the client wanted
      context.negotiatedWidth = context.streamConfig.width;
      context.negotiatedHeight = context.streamConfig.height;
    }

    // We will perform some connection type detection if the caller asked for it
    if (context.streamConfig.remote == StreamConfiguration.STREAM_CFG_AUTO) {
      context.negotiatedRemoteStreaming = StreamConfiguration.STREAM_CFG_AUTO
      //context.negotiatedRemoteStreaming = detectServerConnectionType();
      context.negotiatedPacketSize =
        context.negotiatedRemoteStreaming == StreamConfiguration.STREAM_CFG_REMOTE ?
        1024 : context.streamConfig.maxPacketSize;
    }
    else {
      context.negotiatedRemoteStreaming = context.streamConfig.remote;
      context.negotiatedPacketSize = context.streamConfig.maxPacketSize;
    }
    //
    // Video stream format will be decided during the RTSP handshake
    //
    let app = context.streamConfig.app;
    // If the client did not provide an exact app ID, do a lookup with the applist
    if (!context.streamConfig.app.initialized) {
      LimeLog.info("Using deprecated app lookup method - Please specify an app ID in your StreamConfiguration instead");
      app = await h.getAppByName(context.streamConfig.app.appName);
      if (app == null) {
        context.connListener.displayMessage("The app " + context.streamConfig.app.appName + " is not in GFE app list");
        return false;
      }
    }
   LimeLog.info("startLaunch")
    // If there's a game running, resume it
    if (h.getCurrentGame(serverInfo) != 0) {
      try {
        if (h.getCurrentGame(serverInfo) == app.appId) {
          if (!await h.launchApp(context, "resume", app.appId, context.negotiatedHdr)) {
            context.connListener.displayMessage("Failed to resume existing session");
            return false;
          }
        } else {
          return this.quitAndLaunch(h, context);
        }
      } catch (e) {
        if (e.getErrorCode() == 470) {
          // This is the error you get when you try to resume a session that's not yours.
          // Because this is fairly common, we'll display a more detailed message.
          context.connListener.displayMessage("This session wasn't started by this device," +
          " so it cannot be resumed. End streaming on the original " +
          "device or the PC itself and try again. (Error code: " + e.getErrorCode() + ")");
          return false;
        }
        else if (e.getErrorCode() == 525) {
          context.connListener.displayMessage("The application is minimized. Resume it on the PC manually or " +
          "quit the session and start streaming again.");
          return false;
        } else {
          LimeLog.info("err")
          return false;
        }
        LimeLog.info( e.toString())
      }
      LimeLog.info("Resumed existing game session ", )
      return true;
    }
    else {
      LimeLog.info("launchNotRunningApp")
      return await this.launchNotRunningApp(h, context);
    }
  }
  onVideoStatus(callback: (VideoStatus)=>void){
    this.api.onVideoStatus(callback)
  }
  protected async quitAndLaunch(h: NvHttp, context: ConnectionContext): Promise<boolean> {
    try {
      if (!await h.quitApp()) {
        context.connListener.displayMessage("Failed to quit previous session! You must quit it manually");
        return false;
      }
    } catch (e) {
      if (e.message == 599) {
        context.connListener.displayMessage("This session wasn't started by this device," +
        " so it cannot be quit. End streaming on the original " +
        "device or the PC itself. (Error code: " + e.getErrorCode() + ")");
        return false;
      }
      else {
        throw new Error(e);
      }
    }

    return this.launchNotRunningApp(h, context);
  }

  private async launchNotRunningApp(h: NvHttp, context: ConnectionContext): Promise<boolean> {
    // Launch the app since it's not running
    if (!await h.launchApp(context, "launch", context.streamConfig.app.appId, context.negotiatedHdr)) {
      context.connListener.displayMessage("Failed to launch application");
      return false;
    }
    console.info("Launched new game session");
    return true;
  }
}