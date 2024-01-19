import { NvApp } from '../http/NvApp';
import { MoonBridge } from './MoonBridge';
import { StreamSettings } from '../../uitls/StreamSetttings';

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

  public constructor() {
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

  getWH(wh: string): number[] {
    return wh.split("x").map((s) => parseInt(s))
  }

  public build(settings: StreamSettings): StreamConfiguration {
    const wh = this.getWH(settings.resolution_list)
    this.width = wh[0]
    this.height = wh[1]
    const supportedVideoFormats = MoonBridge.VIDEO_FORMAT_H264;
    // TODO HDR
    // if (decoderRenderer.isHevcSupported()) {
    //     supportedVideoFormats |= MoonBridge.VIDEO_FORMAT_H265;
    //     if (willStreamHdr && decoderRenderer.isHevcMain10Hdr10Supported()) {
    //         supportedVideoFormats |= MoonBridge.VIDEO_FORMAT_H265_MAIN10;
    //     }
    // }
    // if (decoderRenderer.isAv1Supported()) {
    //     supportedVideoFormats |= MoonBridge.VIDEO_FORMAT_AV1_MAIN8;
    //     if (willStreamHdr && decoderRenderer.isAv1Main10Supported()) {
    //         supportedVideoFormats |= MoonBridge.VIDEO_FORMAT_AV1_MAIN10;
    //     }
    // }
    this.launchRefreshRate = parseInt(settings.fps_list)
    this.refreshRate = parseInt(settings.fps_list)

    this.bitrate = parseInt(settings.seekbar_bitrate) * 1000

    this.sops = settings.enable_sops
    this.playLocalAudio = settings.host_audio
    this.maxPacketSize = 1392;
    // NvConnection will perform LAN and VPN detection
    this.remote = StreamConfiguration.STREAM_CFG_AUTO;

    this.supportedVideoFormats = supportedVideoFormats;
    // TODO
    //this.attachedGamepadMask = gamepadMask
    // TODO
    this.clientRefreshRateX100 = 12000
    this.audioConfiguration = parseInt(settings.audio_config_list);

    this.encryptionFlags = MoonBridge.ENCFLG_AUDIO
    // TODO
    this.colorSpace = 1
    this.colorRange = 0
    this.persistGamepadsAfterDisconnect = settings.;

    return this
  }
}