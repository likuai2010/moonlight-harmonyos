
import { AddressTuple } from '../computers/ComputerDetails';
import { StreamConfiguration } from './StreamConfiguration';

export class ConnectionContext {
  public serverAddress: AddressTuple;
  public httpsPort: number = 47984;
  public isNvidiaServerSoftware: boolean;
  public streamConfig: StreamConfiguration;
  public connListener: any;
  public riKey: Uint8Array;
  public riKeyId: number;


  // This is the version quad from the appversion tag of /serverinfo
  public serverAppVersion: string = "";
  public serverGfeVersion: string;
  public serverCodecModeSupport: number;

  // This is the sessionUrl0 tag from /resume and /launch
  public rtspSessionUrl: string;

  public negotiatedWidth: number;
  public negotiatedHeight: number;
  public negotiatedHdr: boolean;

  public negotiatedRemoteStreaming: number;
  public negotiatedPacketSize: number;

  public videoCapabilities: number;
  public constructor() {

  }
}