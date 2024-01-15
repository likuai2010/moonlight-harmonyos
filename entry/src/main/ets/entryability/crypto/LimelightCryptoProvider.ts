import cryptoCert from '@ohos.security.cert';
import fs from '@ohos.file.fs';
import { generate_x509_certificate } from 'libentry.so';

export class LimelightCertProvider {
  static filesDir: string = ""
  public certPath: string;
  public keyPath: string;
  public cerKeyPath: string;

  public cert: cryptoCert.X509Cert;
  public bytes: Uint8Array;
  public keyBytes: Uint8Array;

  constructor() {
  }

  async initCertKeyPair(filesDir: string) {
    LimelightCertProvider.filesDir = filesDir
    this.certPath = LimelightCertProvider.filesDir + "/client.pem"
    this.keyPath = LimelightCertProvider.filesDir + "/private.pem"

    if (!fs.accessSync(this.certPath) || !fs.accessSync(this.keyPath) ) {
      generate_x509_certificate(this.certPath, this.keyPath)
    }
    const certBytes = readFile(this.certPath)
    this.cert = await cryptoCert.createX509Cert(
      { data: certBytes, encodingFormat: cryptoCert.EncodingFormat.FORMAT_PEM })
    this.bytes = certBytes
    this.keyBytes = readFile(this.keyPath)
  }
}
export function readFile(filePath: string): Uint8Array {
  const file = fs.openSync(filePath)
  const stats = fs.statSync(filePath)
  let bufSize = stats.size;
  let buf = new ArrayBuffer(bufSize);
  fs.readSync(file.fd, buf, { offset: 0, length: bufSize });
  fs.closeSync(file);
  return new Uint8Array(buf);
}
export function writeFile(filePath: string, data: Uint8Array) {
  const file = fs.openSync(filePath, fs.OpenMode.CREATE | fs.OpenMode.READ_WRITE)
  fs.writeSync(file.fd, data);
  fs.fsyncSync(file.fd)
  fs.closeSync(file);
}
const provider = new LimelightCertProvider()
export default provider
