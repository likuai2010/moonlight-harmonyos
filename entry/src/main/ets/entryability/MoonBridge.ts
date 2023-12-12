import { MoonBridgeNapi } from 'libentry.so'
import hilog from '@ohos.hilog';
import Buffer from '@ohos.buffer'
import crypto from '@ohos.security.cryptoFramework'
import { NvApp } from './http/NvApp'
import { NvHttp } from './http/NvHttp'
import { AddressTuple, ComputerDetails } from './http/ComputerDetails';
import { LimelightCertProvider, readFile, writeFile } from './crypto/LimelightCryptoProvider';
import { getHttps } from './http/Http';
import { bytesToHex, hexToBytes } from './crypto/CryptoManager';


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
  public static api = new MoonBridgeNapi()


  public async startConnection() {
    var lime =  new LimelightCertProvider()
    await lime.initCertKeyPair()
    //var http = new NvHttp(new AddressTuple("192.168.3.5", NvHttp.DEFAULT_HTTP_PORT), 47984, null, null,lime);
    //var serverCert = await http.getServerCert(lime.bytes)
    var cp = "2D2D2D2D2D424547494E2043455254494649434154452D2D2D2D2D0A4D494943306A4343416271674177494241674955584A59566C46626E6E5A636D37546D5A334E636E65304D58516334774451594A4B6F5A496876634E4151454C0A42514177497A45684D42384741315545417777595533567563326870626D5567523246745A584E30636D56686253424962334E304D4234584454497A4D5445780A4D5441324D5467304D566F584454517A4D5445774E6A41324D5467304D566F77497A45684D42384741315545417777595533567563326870626D5567523246740A5A584E30636D56686253424962334E304D494942496A414E42676B71686B6947397730424151454641414F43415138414D49494243674B4341514541773242530A6652636C4E4379695173385846454E354E5956486D4A7A314E47544E39343945644F6A4E57595834614C7A4C4F79486D754441573831547374536756514662680A5944614D6A41546A7655647A42654E616A5245494D6E7A4B7A5269683264674C73442B7A6734454479374D31776D78506F5861354736465055416656683442510A67767969653959626649617067613550464953514C5546446B327031366B364D652B7A326B525052497A53574D584E44677944704134435072646F52424363450A477039516B56666E6365324A414E516C352B394E6F44355A414C455670737338545458734E4A46516A2F366F78734F48313469554C6E3674786E4362667477310A504F3244584C3079485376516F37384B62346C4E6D6C794854416477427366744356724F6532673677334978393549316A49384548456A376731634555474A4F0A786B364477384C545A326B4669576B6133774944415141424D413047435371475349623344514542437755414134494241514132456C473572714E69672F6C350A63736A6E5A345A456B6E502F5863367137784C4C7A4C53616A5776334F664C75477A38396A617151583056665338784A793858446B694952597357514E7A43310A5564654F35734B6168716C2F6741384255304A766171537239774161564547333942356F5342745A78774E4832574D495A67366E2B635958614176736E41386B0A634C516C6D6F4D7A6A61726A7845434234645377454A30357A774348554D6D324B6F5756796D78506E52682B6E4A5A77467954464E57626E646A76756572414A0A72572B4F71394753733477303646474E6E6A75305832334F3847654A636A4562304778336C346E6F4A6E326E58527A565843496B313259686C736B38433743500A4D2F4B6C6B6677624E6E4435434B3178614942422F6353733932326B45395064717156424E5979504A4C72333574654C794963446E634E6F4A3265695A6D75640A4B535659696536520A2D2D2D2D2D454E442043455254494649434154452D2D2D2D2D0A0A"
    var ca = `-----BEGIN CERTIFICATE-----
MIIC0jCCAbqgAwIBAgIUXJYVlFbnnZcm7TmZ3Ncne0MXQc4wDQYJKoZIhvcNAQEL
BQAwIzEhMB8GA1UEAwwYU3Vuc2hpbmUgR2FtZXN0cmVhbSBIb3N0MB4XDTIzMTEx
MTA2MTg0MVoXDTQzMTEwNjA2MTg0MVowIzEhMB8GA1UEAwwYU3Vuc2hpbmUgR2Ft
ZXN0cmVhbSBIb3N0MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAw2BS
fRclNCyiQs8XFEN5NYVHmJz1NGTN949EdOjNWYX4aLzLOyHmuDAW81TstSgVQFbh
YDaMjATjvUdzBeNajREIMnzKzRih2dgLsD+zg4EDy7M1wmxPoXa5G6FPUAfVh4BQ
gvyie9YbfIapga5PFISQLUFDk2p16k6Me+z2kRPRIzSWMXNDgyDpA4CPrdoRBCcE
Gp9QkVfnce2JANQl5+9NoD5ZALEVpss8TTXsNJFQj/6oxsOH14iULn6txnCbftw1
PO2DXL0yHSvQo78Kb4lNmlyHTAdwBsftCVrOe2g6w3Ix95I1jI8EHEj7g1cEUGJO
xk6Dw8LTZ2kFiWka3wIDAQABMA0GCSqGSIb3DQEBCwUAA4IBAQA2ElG5rqNig/l5
csjnZ4ZEknP/Xc6q7xLLzLSajWv3OfLuGz89jaqQX0VfS8xJy8XDkiIRYsWQNzC1
UdeO5sKahql/gA8BU0JvaqSr9wAaVEG39B5oSBtZxwNH2WMIZg6n+cYXaAvsnA8k
cLQlmoMzjarjxECB4dSwEJ05zwCHUMm2KoWVymxPnRh+nJZwFyTFNWbndjvuerAJ
rW+Oq9GSs4w06FGNnju0X23O8GeJcjEb0Gx3l4noJn2nXRzVXCIk12Yhlsk8C7CP
M/KlkfwbNnD5CK1xaIBB/cSs922kE9PdqqVBNYyPJLr35teLyIcDncNoJ2eiZmud
KSVYie6R
-----END CERTIFICATE-----
`
    writeFile(LimelightCertProvider.filesDir + "/ca.pem", ca)

    const dd = readFile(LimelightCertProvider.filesDir + "/ca.pem",)
    const cphex = hexToBytes(cp)
    console.log(cp)
    getHttps()

  }
}








