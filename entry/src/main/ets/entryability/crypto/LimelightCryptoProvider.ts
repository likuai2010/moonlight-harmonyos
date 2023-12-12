import cryptoCert from '@ohos.security.cert';
import crypto from '@ohos.security.cryptoFramework';
import { generate_x509_certificate } from 'libentry.so'
import UIAbility from '@ohos.app.ability.UIAbility';
import fs from '@ohos.file.fs';
import cert from '@ohos.security.cert';
import { KeyCode } from '@ohos.multimodalInput.keyCode';
import { hexToBytes } from './CryptoManager';
import buffer from '@ohos.buffer';

export class LimelightCertProvider {
  static filesDir: string = ""
  public certPath: string;
  public keyPath: string;

  public cert: cryptoCert.X509Cert;
  public bytes: Uint8Array;
  public key: crypto.PriKey;

  constructor() {
    this.certPath = LimelightCertProvider.filesDir + "/client.pem"
    this.keyPath = LimelightCertProvider.filesDir + "/private.key"
  }

  async initCertKeyPair() {
    if (!fs.accessSync(this.certPath) || !fs.accessSync(this.keyPath)) {
      generate_x509_certificate(this.certPath, this.keyPath)
    }
    const certBytes = readFile(this.certPath)
    this.cert = await cryptoCert.createX509Cert(
      { data: certBytes, encodingFormat: cryptoCert.EncodingFormat.FORMAT_PEM })
    const kg = crypto.createAsyKeyGenerator("RSA2048|PRIMES_2")
    this.bytes = certBytes
    // der 格式
    //this.key = (await kg.convertKey(null, { data: readFile(this.keyPath) })).priKey
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
export function writeFile(filePath: string, data: string) {
  const file = fs.openSync(filePath, fs.OpenMode.CREATE | fs.OpenMode.READ_WRITE)
  fs.writeSync(file.fd, data);
  fs.fsyncSync(file.fd)
  fs.closeSync(file);
}