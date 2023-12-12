import { NvApp } from './http/NvApp';
import { MoonBridge } from './MoonBridge';
export class StreamConfiguration {
  public static INVALID_APP_ID = 0;

  public static STREAM_CFG_LOCAL = 0;
  public static STREAM_CFG_REMOTE = 1;
  public static STREAM_CFG_AUTO = 2;
  public app: NvApp;
  public width: number;
  public height: number;
  public refreshRate: number;
  public launchRefreshRate: number;
  public clientRefreshRateX100: number;
  public bitrate: number;
  public sops: boolean;
  public enableAdaptiveResolution: boolean;
  public playLocalAudio: boolean;
  public maxPacketSize: number;
  public remote: number;
  public audioConfiguration: number;
  public supportedVideoFormats: number;
  public attachedGamepadMask: number;
  public encryptionFlags: number;
  public colorRange: number;
  public colorSpace: number;
  public persistGamepadsAfterDisconnect: boolean;

  public constructor(){
    // Set default attributes
    this.width = 1280;
    this.height = 720;
    this.refreshRate = 60;
    this.launchRefreshRate = 60;
    this.bitrate = 10000;
    this.maxPacketSize = 1024;
    this.remote = StreamConfiguration.STREAM_CFG_AUTO;
    this.sops = true;
    this.enableAdaptiveResolution = false;
    this.audioConfiguration = MoonBridge.AUDIO_CONFIGURATION_STEREO.toInt();
    this.supportedVideoFormats = MoonBridge.VIDEO_FORMAT_H264;
    this.attachedGamepadMask = 0;
  }
}