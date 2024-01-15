import { NvApp } from '../http/NvApp';
import { NvHttp } from '../http/NvHttp'
import { PairingManager, PairState } from '../http/PairingManager'
export enum ComputerState {
  ONLINE = 'ONLINE',
  OFFLINE = 'OFFLINE',
  UNKNOWN = 'UNKNOWN'
}
export class ComputerDetails {


  // Persistent attributes
  uuid: string;
  name: string;
  localAddress: AddressTuple;
  remoteAddress: AddressTuple;
  manualAddress: AddressTuple;
  ipv6Address: AddressTuple;
  macAddress: string;
  //serverCert: X509Certificate;

  // Transient attributes
  state: ComputerState;
  activeAddress: AddressTuple;
  httpsPort: number;
  externalPort: number;
  pairState: PairState;
  runningGameId: number;
  rawAppList: string;
  appList: NvApp[];
  nvidiaServer: boolean;

  constructor() {
    // Use defaults
    this.state = ComputerState.UNKNOWN;
  }

  guessExternalPort(): number {
    if (this.externalPort !== 0) {
      return this.externalPort;
    } else if (this.remoteAddress !== null) {
      return this.remoteAddress.port;
    } else if (this.activeAddress !== null) {
      return this.activeAddress.port;
    } else if (this.ipv6Address !== null) {
      return this.ipv6Address.port;
    } else if (this.localAddress !== null) {
      return this.localAddress.port;
    } else {
      return NvHttp.DEFAULT_HTTP_PORT;
    }
  }

  update(details: ComputerDetails): void {
    this.state = details.state;
    this.name = details.name;
    this.uuid = details.uuid;
    if (details.activeAddress != null) {
      this.activeAddress = makeTuple(details.activeAddress.address, details.activeAddress.port);
    }
    // We can get IPv4 loopback addresses with GS IPv6 Forwarder
    if (details.localAddress != null && !details.localAddress.address.startsWith("127.")) {
      this.localAddress = details.localAddress;
    }
    if (details.remoteAddress != null) {
      this.remoteAddress = details.remoteAddress;
    } else if (this.remoteAddress != null && details.externalPort !== 0) {
      // If we have a remote address already (perhaps via STUN) but our updated details
      // don't have a new one (because GFE doesn't send one), propagate the external
      // port to the current remote address. We may have tried to guess it previously.
      this.remoteAddress.port = details.externalPort;
    }
    if (details.manualAddress != null) {
      this.manualAddress = makeTuple(details.manualAddress.address, details.manualAddress.port);
    }
    if (details.ipv6Address != null) {
      this.ipv6Address = details.ipv6Address;
    }
    if (details.macAddress != null && details.macAddress !== "00:00:00:00:00:00") {
      this.macAddress = details.macAddress;
    }
    // if (details.serverCert !== null) {
    //   this.serverCert = details.serverCert;
    // }
    this.externalPort = details.externalPort;
    this.httpsPort = details.httpsPort;
    this.pairState = details.pairState;
    this.runningGameId = details.runningGameId;
    this.nvidiaServer = details.nvidiaServer;
    if(details.rawAppList){
      this.rawAppList = details.rawAppList;
    }
    if(details.rawAppList){
      this.rawAppList = details.rawAppList;
    }
  }

  toString(): string {
    let str = "";
    str += `Name: ${this.name}\n`;
    str += `State: ${this.state}\n`;
    str += `Active Address: ${this.activeAddress.toString()}\n`;
    str += `UUID: ${this.uuid}\n`;
    str += `Local Address: ${this.localAddress}\n`;
    str += `Remote Address: ${this.remoteAddress}\n`;
    str += `IPv6 Address: ${this.ipv6Address}\n`;
    str += `Manual Address: ${this.manualAddress.toString()}\n`;
    str += `MAC Address: ${this.macAddress}\n`;
    str += `Pair State: ${this.pairState}\n`;
    str += `Running Game ID: ${this.runningGameId}\n`;
    str += `HTTPS Port: ${this.httpsPort}\n`;
    return str;
  }
}

export class AddressTuple {
  address: string;
  port: number;

  constructor(address: string, port: number) {
    if (address === null) {
      throw new Error("Address cannot be null");
    }
    if (port <= 0) {
      throw new Error("Invalid port");
    }

    // If this was an escaped IPv6 address, remove the brackets
    if (address.startsWith("[") && address.endsWith("]")) {
      address = address.substring(1, address.length - 1);
    }

    this.address = address;
    this.port = port;
  }

  toString(): string {
    if (this.address.includes(":")) {
      // IPv6
      return `[${this.address}]:${this.port}`;
    } else {
      // IPv4 and hostnames
      return `${this.address}:${this.port}`;
    }
  }
}

export function makeTuple(address: string, port: number) {
  if (address == null || address == "") {
    return null;
  }
  return new AddressTuple(address, port);
}
