import { MoonBridgeNapi } from 'libentry.so'
import { NvHttp } from '../http/NvHttp'
import { AddressTuple, ComputerDetails } from '../computers/ComputerDetails';
import { LimelightCertProvider} from '../crypto/LimelightCryptoProvider';
import { NvConnectionListener } from './ConnetionListener';


export class AudioConfiguration{
  public channelCount : number;
  public channelMask: number;
  public constructor(channelCount: number, channelMask: number) {
    this.channelCount = channelCount;
    this.channelMask = channelMask;
  }
  public autoConfiguration(audioConfiguration: number) {
    if ((audioConfiguration & 0xFF) != 0xCA) {
      throw Error("Audio configuration has invalid magic byte!");
    }
    this.channelCount = (audioConfiguration >> 8) & 0xFF;
    this.channelMask = (audioConfiguration >> 16) & 0xFFFF;
  }
  // Returns the integer value expected by moonlight-common-c
  // See MAKE_AUDIO_CONFIGURATION() in Limelight.h
  public toInt(): number {
    return ((this.channelMask) << 16) | (this.channelCount << 8) | 0xCA;
  }
}


export class MoonBridge {
  public static AUDIO_CONFIGURATION_STEREO = new AudioConfiguration(2, 0x3);
  public static AUDIO_CONFIGURATION_51_SURROUND = new AudioConfiguration(6, 0x3F);
  public static AUDIO_CONFIGURATION_71_SURROUND = new AudioConfiguration(8, 0x63F);
  public static readonly VIDEO_FORMAT_H264: number = 0x0001;
  public static readonly VIDEO_FORMAT_H265: number = 0x0100;
  public static readonly VIDEO_FORMAT_H265_MAIN10: number = 0x0200;
  public static readonly VIDEO_FORMAT_AV1_MAIN8: number = 0x1000;
  public static readonly VIDEO_FORMAT_AV1_MAIN10: number = 0x2000;

  public static readonly VIDEO_FORMAT_MASK_H264: number = 0x000F;
  public static readonly VIDEO_FORMAT_MASK_H265: number = 0x0F00;
  public static readonly VIDEO_FORMAT_MASK_AV1: number = 0xF000;
  public static readonly VIDEO_FORMAT_MASK_10BIT: number = 0x2200;

  public static readonly ENCFLG_NONE: number = 0;
  public static readonly ENCFLG_AUDIO: number = 1;
  public static readonly ENCFLG_ALL: number = 0xFFFFFFFF;

  public static readonly BUFFER_TYPE_PICDATA: number = 0;
  public static readonly BUFFER_TYPE_SPS: number = 1;
  public static readonly BUFFER_TYPE_PPS: number = 2;
  public static readonly BUFFER_TYPE_VPS: number = 3;

  public static readonly FRAME_TYPE_PFRAME: number = 0;
  public static readonly FRAME_TYPE_IDR: number = 1;

  public static readonly COLORSPACE_REC_601: number = 0;
  public static readonly COLORSPACE_REC_709: number = 1;
  public static readonly COLORSPACE_REC_2020: number = 2;

  public static readonly COLOR_RANGE_LIMITED: number = 0;
  public static readonly COLOR_RANGE_FULL: number = 1;

  public static readonly CAPABILITY_DIRECT_SUBMIT: number = 1;
  public static readonly CAPABILITY_REFERENCE_FRAME_INVALIDATION_AVC: number = 2;
  public static readonly CAPABILITY_REFERENCE_FRAME_INVALIDATION_HEVC: number = 4;
  public static readonly CAPABILITY_REFERENCE_FRAME_INVALIDATION_AV1: number = 0x40;

  public static readonly DR_OK: number = 0;
  public static readonly DR_NEED_IDR: number = -1;

  public static readonly CONN_STATUS_OKAY: number = 0;
  public static readonly CONN_STATUS_POOR: number = 1;

  public static readonly ML_ERROR_GRACEFUL_TERMINATION: number = 0;
  public static readonly ML_ERROR_NO_VIDEO_TRAFFIC: number = -100;
  public static readonly ML_ERROR_NO_VIDEO_FRAME: number = -101;
  public static readonly ML_ERROR_UNEXPECTED_EARLY_TERMINATION: number = -102;
  public static readonly ML_ERROR_PROTECTED_CONTENT: number = -103;
  public static readonly ML_ERROR_FRAME_CONVERSION: number = -104;

  public static readonly ML_PORT_INDEX_TCP_47984: number = 0;
  public static readonly ML_PORT_INDEX_TCP_47989: number = 1;
  public static readonly ML_PORT_INDEX_TCP_48010: number = 2;
  public static readonly ML_PORT_INDEX_UDP_47998: number = 8;
  public static readonly ML_PORT_INDEX_UDP_47999: number = 9;
  public static readonly ML_PORT_INDEX_UDP_48000: number = 10;
  public static readonly ML_PORT_INDEX_UDP_48010: number = 11;

  public static readonly ML_PORT_FLAG_ALL: number = 0xFFFFFFFF;
  public static readonly ML_PORT_FLAG_TCP_47984: number = 0x0001;
  public static readonly ML_PORT_FLAG_TCP_47989: number = 0x0002;
  public static readonly ML_PORT_FLAG_TCP_48010: number = 0x0004;
  public static readonly ML_PORT_FLAG_UDP_47998: number = 0x0100;
  public static readonly ML_PORT_FLAG_UDP_47999: number = 0x0200;
  public static readonly ML_PORT_FLAG_UDP_48000: number = 0x0400;
  public static readonly ML_PORT_FLAG_UDP_48010: number = 0x0800;

  public static readonly ML_TEST_RESULT_INCONCLUSIVE: number = 0xFFFFFFFF;

  public static readonly SS_KBE_FLAG_NON_NORMALIZED: number = 0x01;

  public static readonly LI_ERR_UNSUPPORTED: number = -5501;

  public static readonly LI_TOUCH_EVENT_HOVER: number = 0x00;
  public static readonly LI_TOUCH_EVENT_DOWN: number = 0x01;
  public static readonly LI_TOUCH_EVENT_UP: number = 0x02;
  public static readonly LI_TOUCH_EVENT_MOVE: number = 0x03;
  public static readonly LI_TOUCH_EVENT_CANCEL: number = 0x04;
  public static readonly LI_TOUCH_EVENT_BUTTON_ONLY: number = 0x05;
  public static readonly LI_TOUCH_EVENT_HOVER_LEAVE: number = 0x06;
  public static readonly LI_TOUCH_EVENT_CANCEL_ALL: number = 0x07;

  public static readonly LI_TOOL_TYPE_UNKNOWN: number = 0x00;
  public static readonly LI_TOOL_TYPE_PEN: number = 0x01;
  public static readonly LI_TOOL_TYPE_ERASER: number = 0x02;

  public static readonly LI_PEN_BUTTON_PRIMARY: number = 0x01;
  public static readonly LI_PEN_BUTTON_SECONDARY: number = 0x02;
  public static readonly LI_PEN_BUTTON_TERTIARY: number = 0x04;

  public static readonly LI_TILT_UNKNOWN: number = 0xFF;
  public static readonly LI_ROT_UNKNOWN: number = 0xFFFF;

  public static readonly LI_CTYPE_UNKNOWN: number = 0x00;
  public static readonly LI_CTYPE_XBOX: number = 0x01;
  public static readonly LI_CTYPE_PS: number = 0x02;
  public static readonly LI_CTYPE_NINTENDO: number = 0x03;

  public static readonly LI_CCAP_ANALOG_TRIGGERS: number = 0x01;
  public static readonly LI_CCAP_RUMBLE: number = 0x02;
  public static readonly LI_CCAP_TRIGGER_RUMBLE: number = 0x04;
  public static readonly LI_CCAP_TOUCHPAD: number = 0x08;
  public static readonly LI_CCAP_ACCEL: number = 0x10;
  public static readonly LI_CCAP_GYRO: number = 0x20;
  public static readonly LI_CCAP_BATTERY_STATE: number = 0x40;
  public static readonly LI_CCAP_RGB_LED: number = 0x80;

  public static readonly LI_MOTION_TYPE_ACCEL: number = 0x01;
  public static readonly LI_MOTION_TYPE_GYRO: number = 0x02;

  public static readonly LI_BATTERY_STATE_UNKNOWN: number = 0x00;
  public static readonly LI_BATTERY_STATE_NOT_PRESENT: number = 0x01;
  public static readonly LI_BATTERY_STATE_DISCHARGING: number = 0x02;
  public static readonly LI_BATTERY_STATE_CHARGING: number = 0x03;
  public static LI_BATTERY_STATE_NOT_CHARGING: number = 0x04; // Connected to power but not charging
  public static LI_BATTERY_STATE_FULL: number = 0x05;
  public static LI_BATTERY_PERCENTAGE_UNKNOWN: number = 0xFF;
  public api = new MoonBridgeNapi()
  public listener: NvConnectionListener

  public async startConnection() {
    var lime =  new LimelightCertProvider()
    //await lime.initCertKeyPair()
    var http = new NvHttp(new AddressTuple("192.168.3.5", NvHttp.DEFAULT_HTTP_PORT), 47984, null, lime);
    var serInfo = await http.getServerInfo(true)
    //const d = await http.pm.pair(serInfo, "12345")
    const list = await http.getAppListRaw()
    console.log(list);
    // http.pm.pair(serInfo, "12345")
  }
  public register(listener: NvConnectionListener){
    this.listener = listener;
    this.api.bridgeClStageStarting(listener.stageStarting)
    this.api.bridgeClStageComplete(listener.stageStarting)
    this.api.bridgeClStageFailed(listener.stageFailed)
  }
}








