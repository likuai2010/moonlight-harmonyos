import { NvApp } from '../http/NvApp';
import { AudioConfiguration, MoonBridge } from './MoonBridge';
import { StreamSettings } from '../../uitls/StreamSetttings';
import LimeLog from '../LimeLog';
enum FormatOption {
  AUTO,
  FORCE_AV1,
  FORCE_HEVC,
  FORCE_H264,
};
 const FRAME_PACING_MIN_LATENCY = 0;
 const FRAME_PACING_BALANCED = 1;
 const FRAME_PACING_CAP_FPS = 2;
 const FRAME_PACING_MAX_SMOOTHNESS = 3;

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
    const roundedRefreshRate = 60
    const fps = parseInt(settings.fps_list)
    let chosenFrameRate = fps
    let framePacing = this.getFramePacingValue(settings.frame_pacing)
    if (framePacing == FRAME_PACING_CAP_FPS) {
      if (fps >= roundedRefreshRate) {
        if (fps > roundedRefreshRate + 3) {
          // Use frame drops when rendering above the screen frame rate
          framePacing = FRAME_PACING_BALANCED;
          LimeLog.info("Using drop mode for FPS > Hz");
        } else if (roundedRefreshRate <= 49) {
          // Let's avoid clearly bogus refresh rates and fall back to legacy rendering
          framePacing = FRAME_PACING_BALANCED;
          LimeLog.info("Bogus refresh rate: " + roundedRefreshRate);
        }
        else {
          chosenFrameRate = roundedRefreshRate - 1;
          LimeLog.info("Adjusting FPS target for screen to " + chosenFrameRate);
        }
      }
    }

    this.launchRefreshRate = parseInt(settings.fps_list)
    this.refreshRate = chosenFrameRate

    this.bitrate = parseInt(settings.seekbar_bitrate) * 1000
    if(this.bitrate == 0)
      this.bitrate = this.getDefaultBitrate(settings.resolution_list, settings.fps_list)

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
    this.audioConfiguration = this.getAudioConfiguration(settings.audio_config_list).toInt();
    this.encryptionFlags = MoonBridge.ENCFLG_AUDIO
    // TODO
    this.colorSpace = 1
    this.colorRange = 0
    this.persistGamepadsAfterDisconnect = false;

    return this
  }
  getAudioConfiguration(audioConfig: string):AudioConfiguration{
    if (audioConfig == "71") {
      return  MoonBridge.AUDIO_CONFIGURATION_71_SURROUND;
    }
    else if (audioConfig == "51") {
      return  MoonBridge.AUDIO_CONFIGURATION_51_SURROUND;
    }
    else /* if (audioConfig.equals("2")) */ {
      return  MoonBridge.AUDIO_CONFIGURATION_STEREO;
    }
  }
  getDefaultBitrate(resString: string, fpsString: string): number {
    const wh = this.getWH(resString)
    const width: number = wh[0];
    const height: number = wh[1];
    const fps: number = parseInt(fpsString);

    // This logic is shamelessly stolen from Moonlight Qt:
    // https://github.com/moonlight-stream/moonlight-qt/blob/master/app/settings/streamingpreferences.cpp

    // Don't scale bitrate linearly beyond 60 FPS. It's definitely not a linear
    // bitrate increase for frame rate once we get to values that high.
    const frameRateFactor: number = (fps <= 60 ? fps : (Math.sqrt(fps / 60) * 60)) / 30;

    // TODO: Collect some empirical data to see if these defaults make sense.
    // We're just using the values that the Shield used, as we have for years.
    const pixelVals: number[] = [
      640 * 360,
      854 * 480,
      1280 * 720,
      1920 * 1080,
      2560 * 1440,
      3840 * 2160,
      -1,
    ];
    const factorVals: number[] = [
      1,
      2,
      5,
      10,
      20,
      40,
      -1
    ];

    // Calculate the resolution factor by linear interpolation of the resolution table
    let resolutionFactor: number;
    const pixels: number = width * height;
    for (let i = 0; ; i++) {
      if (pixels == pixelVals[i]) {
        // We can bail immediately for exact matches
        resolutionFactor = factorVals[i];
        break;
      }
      else if (pixels < pixelVals[i]) {
        if (i == 0) {
          // Never go below the lowest resolution entry
          resolutionFactor = factorVals[i];
        }
        else {
          // Interpolate between the entry greater than the chosen resolution (i) and the entry less than the chosen resolution (i-1)
          resolutionFactor = ((pixels - pixelVals[i-1]) / (pixelVals[i] - pixelVals[i-1])) * (factorVals[i] - factorVals[i-1]) + factorVals[i-1];
        }
        break;
      }
      else if (pixelVals[i] == -1) {
        // Never go above the highest resolution entry
        resolutionFactor = factorVals[i-1];
        break;
      }
    }

    return Math.round(resolutionFactor * frameRateFactor) * 1000;
  }
  getFramePacingValue(str: string){

    switch (str) {
      case "latency":
        return FRAME_PACING_MIN_LATENCY;
      case "balanced":
        return FRAME_PACING_BALANCED;
      case "cap-fps":
        return FRAME_PACING_CAP_FPS;
      case "smoothness":
        return FRAME_PACING_MAX_SMOOTHNESS;
      default:
      // Should never get here
        return FRAME_PACING_MIN_LATENCY;
    }
  }
  getVideoFormatValue( str: string){
    if (str === "auto") {
      return FormatOption.AUTO;
    } else if (str === "forceav1") {
      return FormatOption.FORCE_AV1;
    } else if (str === "forceh265") {
      return FormatOption.FORCE_HEVC;
    } else if (str === "neverh265") {
      return FormatOption.FORCE_H264;
    } else {
      // Should never get here
      return FormatOption.AUTO;
    }
  }
}
