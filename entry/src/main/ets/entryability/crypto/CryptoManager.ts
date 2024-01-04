import cryptoCert from '@ohos.security.cert';
import crypto from '@ohos.security.cryptoFramework';
import { NvHttp } from '../http/NvHttp';
import Buffer from '@ohos.buffer';
import { verify_signature } from 'libentry.so'

export interface PairingHashAlgorithm {
  getHashLength(): number
  hashData(data: Uint8Array): Promise<Uint8Array>
}

export class Sha1PairingHash implements PairingHashAlgorithm {
  async hashData(data: Uint8Array): Promise<Uint8Array> {
    const md = crypto.createMd("SHA-1")
    await md.update({ data })
    const dig = await md.digest()
    return dig.data
  }

  getHashLength(): number {
    return 20;
  }
}

export class Sha256PairingHash implements PairingHashAlgorithm {
  async hashData(data: Uint8Array): Promise<Uint8Array> {
    const md = crypto.createMd("SHA256")
    await md.update({ data })
    const dig = await md.digest()
    return dig.data
  }

  getHashLength(): number {
    return 32;
  }
}

export async function signData(data: Uint8Array, key: crypto.PriKey): Promise<Uint8Array> {
  try {
    // der
    const sign = crypto.createSign("RSA2048|PKCS1|SHA256")
    await sign.init(key)
    return (await sign.sign({ data })).data;
  } catch (e) {
    throw new Error(e);
  }
}

export async function verifySignature(data: Uint8Array, signature: Uint8Array, key: Uint8Array): Promise<boolean> {
  return verify_signature(data, signature, key)
  // try {
  //   const verify = crypto.createVerify('RSA2048|PKCS1|SHA256')
  //   // der
  //   await verify.init(key);
  //   return (await verify.verify({ data }, { data: signature }));
  // } catch (e) {
  //   throw new Error(e);
  // }
}

export async function generateRandomBytes(length: number): Promise<Uint8Array> {
  const random = crypto.createRandom()
  const dataBlob = await random.generateRandom(length)
  return dataBlob.data;
}

export async function encryptAes(plaintextData: Uint8Array, aesKey: Uint8Array): Promise<Uint8Array> {
  const cipher = crypto.createCipher("AES128|ECB|NoPadding")
  const key = await crypto.createSymKeyGenerator("AES128").convertKey({ data: aesKey })
  await cipher.init(crypto.CryptoMode.ENCRYPT_MODE, key, null)
  const bytes = await cipher.update({ data: plaintextData })
  return (await cipher.doFinal(bytes)).data
}

export async function decryptAes(plaintextData: Uint8Array, aesKey: Uint8Array): Promise<Uint8Array> {
  const cipher = crypto.createCipher("AES128|ECB|NoPadding")
  const key = await crypto.createSymKeyGenerator("AES128").convertKey({ data: aesKey })
  await cipher.init(crypto.CryptoMode.DECRYPT_MODE, key, null)
  const bytes = await cipher.update({ data: plaintextData })
  return (await cipher.doFinal(bytes)).data
}


export async function generateAesKey(hashAlgo: PairingHashAlgorithm, keyData: Uint8Array): Promise<Uint8Array> {
  return (await hashAlgo.hashData(keyData)).slice(0, 16)
}
export async function generateCertKeyPair() {
  const snBytes = await generateRandomBytes(8)
  const asy = crypto.createAsyKeyGenerator("RSA2048|PRIMES_2")
  const keyPair = await asy.generateKeyPair()

}
export async function extractPlainCertBytes(text: string): Promise<Uint8Array> {
  // Plaincert may be null if another client is already trying to pair
  const certText = NvHttp.getXmlString(text, "plaincert", false);
  if (certText != null) {
    return hexToBytes(certText);
  }
  else {
    return null;
  }
}

export async function extractPlainCert(bytes: Uint8Array): Promise<cryptoCert.X509Cert> {
  // Plaincert may be null if another client is already trying to pair
  if (bytes != null) {
    try {
      return await cryptoCert.createX509Cert({
        data: bytes,
        encodingFormat: cryptoCert.EncodingFormat.FORMAT_PEM
      })
    }
    catch (e) {
      e.printStackTrace();
    }
  }
  else {
    return null;
  }
}

export function bytesToHex(bytes: Uint8Array) {
  return Array.prototype.map
    .call(bytes, (x) => ('00' + x.toString(16)).slice(-2))
    .join('');
}

export function hexToBytes(hex: string) {
  var bytes = [];
  for (var i = 0; i < hex.length; i += 2) {
    bytes.push(parseInt(hex.substr(i, 2), 16));
  }
  return new Uint8Array(bytes);
}


export function concatBytes(a: Uint8Array, b: Uint8Array): Uint8Array {
 const dd = Buffer.allocUninitialized(a.length + b.length)
  dd.fill(a)
  dd.fill(b)
  return new Uint8Array(dd.buffer);
}


export async function generateRiKey(): Promise<Uint8Array>  {
  const symKeyGenerator = crypto.createSymKeyGenerator("AES128")
  const key = await symKeyGenerator.generateSymKey()
  const data = key.getEncoded().data
  key.clearMem();
  return data;
}
export async function generateRiKeyId(): Promise<number> {
  const bytes = await generateRandomBytes(4)
  let buffer = Buffer.from(bytes);
  let int = buffer.readUInt32BE()
  return int;
}