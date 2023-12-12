export class NvApp {
  public appName: string;
  public appId: number;
  public initialized: boolean;
  public hdrSupported: boolean;

  public constructor() {
    this.initialized = false;
  }

  public toString(): String {
    let str = "";
    str += `Name: ${this.appName}\n`;
    str += `HDR Supported:  ${this.hdrSupported}\n`;
    str += `ID: ${this.appId}\n`;
    return str;
  }
}