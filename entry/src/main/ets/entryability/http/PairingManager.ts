import { NvHttp } from './NvHttp'
import cryptoCert from '@ohos.security.cert';
import util from '@ohos.util'
import { LimelightCertProvider } from '../crypto/LimelightCryptoProvider';
import {
  bytesToHex,
  concatBytes,
  decryptAes,
  encryptAes,
  extractPlainCert,
extractPlainCertBytes,
generateRandomBytes,
  hexToBytes,
  PairingHashAlgorithm,
  Sha1PairingHash,
  Sha256PairingHash,
  signData,
  verifySignature
} from '../crypto/CryptoManager';

export class PairingManager {
  private http: NvHttp;
  private serverCert: cryptoCert.X509Cert
  private clientCert: LimelightCertProvider

  constructor(http: NvHttp, cryptoProvider: LimelightCertProvider) {
    this.http = http;
    this.clientCert = cryptoProvider;
  }



  public async pair(serverInfo: string, pin: string): Promise<PairState> {
     const serverMajorVersion = this.http.getServerMajorVersion(serverInfo);
     console.log("Pairing with server generation: " + serverMajorVersion);
     let hashAlgo: PairingHashAlgorithm;
    if (serverMajorVersion >= 7) {
      // Gen 7+ uses SHA-256 hashing
      hashAlgo = new Sha256PairingHash();
    } else {
      // Prior to Gen 7, SHA-1 is used
      hashAlgo = new Sha1PairingHash();
    }
    // Generate a salt for hashing the PIN
    const salt = await generateRandomBytes(16);
    // Combine the salt and pin, then create an AES key from them
    const aesKey = await this.generateAesKey(hashAlgo, this.saltPin(salt, pin));
    // Send the salt and get the server cert. This doesn't have a read timeout
    // because the user must enter the PIN before the server responds
    const getCert = await this.http.executePairingCommand(
      "phrase=getservercert&salt=" + bytesToHex(salt) + "&clientcert=" + bytesToHex(this.clientCert.bytes),
      false);

    if (NvHttp.getXmlString(getCert, "paired", true) !== '1') {
      return PairState.FAILED;
    }
    // Save this cert for retrieval later
    const serverCertBytes = await extractPlainCertBytes(getCert);
    this.serverCert = await extractPlainCert(serverCertBytes);
    // Generate a random challenge and encrypt it with our AES key
    const randomChallenge = await generateRandomBytes(16);
    const encryptedChallenge = await encryptAes(randomChallenge, aesKey);

    const challengeResp = await this.http.executePairingCommand("clientchallenge=" + bytesToHex(encryptedChallenge), true);
    if (NvHttp.getXmlString(challengeResp, "paired", true) !== '1') {
      //this.http.unpair();
      return PairState.FAILED;
    }

    // Decode the server's response and subsequent challenge
    const encServerChallengeResponse = hexToBytes(NvHttp.getXmlString(challengeResp, "challengeresponse", true));
    const decServerChallengeResponse = await decryptAes(encServerChallengeResponse, aesKey);

    const serverResponse = decServerChallengeResponse.slice(0, hashAlgo.getHashLength());

    const serverChallenge = decServerChallengeResponse.slice(hashAlgo.getHashLength(), hashAlgo.getHashLength() + 16);

    // Using another 16 bytes secret, compute a challenge response hash using the secret, our cert sig, and the challenge
    const clientSecret = await generateRandomBytes(16);
    const challengeRespHash = await hashAlgo.hashData(concatBytes(concatBytes(serverChallenge, this.clientCert.cert.getSignature().data), clientSecret));
    const challengeRespEncrypted = await encryptAes(challengeRespHash, aesKey);

    const secretResp = await this.http.executePairingCommand("serverchallengeresp=" + bytesToHex(challengeRespEncrypted), true);
    if (NvHttp.getXmlString(secretResp, "paired", true) !== '1') {
      //http.unpair();
      return PairState.FAILED;
    }
    // Get the server's signed secret
    const serverSecretResp = hexToBytes(NvHttp.getXmlString(secretResp, "pairingsecret", true));
    const serverSecret = serverSecretResp.slice(0, 16)
    const serverSignature = serverSecretResp.slice(16, serverSecretResp.length)
    // Ensure the authenticity of the data
    if (!await verifySignature(serverSecret, serverSignature, serverCertBytes)) {
      // Cancel the pairing process
      //http.unpair();
      // Looks like a MITM
      return PairState.FAILED;
    }
    // Ensure the server challenge matched what we expected (aka the PIN was correct)
    const serverChallengeRespHash = await hashAlgo.hashData(concatBytes(concatBytes(randomChallenge, this.serverCert.getSignature().data), serverSecret));
    if (serverChallengeRespHash.every((value, index) => value === serverResponse[index])) {
      // Cancel the pairing process
      //http.unpair();

      // Probably got the wrong PIN
      return PairState.PIN_WRONG;
    }
    // Send the server our signed secret
    const clientPairingSecret = concatBytes(clientSecret, await signData(clientSecret, this.clientCert.key));
    const clientSecretResp = await this.http.executePairingCommand("clientpairingsecret=" + bytesToHex(clientPairingSecret), true);
    if (NvHttp.getXmlString(clientSecretResp, "paired", true) !== '1') {
       //http.unpair();
       return PairState.FAILED;
     }

    // Do the initial challenge (seems necessary for us to show as paired)
    const pairChallenge = await this.http.executePairingChallenge();
    if (NvHttp.getXmlString(pairChallenge, "paired", true) !== '1') {
      //http.unpair();
      return PairState.FAILED;
    }

    return PairState.PAIRED
  }

  async generateAesKey(hashAlgo: PairingHashAlgorithm, keyData: Uint8Array): Promise<Uint8Array> {
    return (await hashAlgo.hashData(keyData)).slice(0, 16)
  }

  private saltPin(salt: Uint8Array, pin: string): Uint8Array {
    const saltedPin = salt.slice(0, salt.length)
    const decoder = new util.TextEncoder("UTF-8")
    const pinBytes = decoder.encodeInto(pin)
    return Uint8Array.from([...saltedPin, ...pinBytes]);
  }
}

export enum PairState {
  NOT_PAIRED = 'NOT_PAIRED',
  PAIRED = 'OFFLINE',
  PIN_WRONG = 'PIN_WRONG',
  FAILED = 'FAILED',
  ALREADY_IN_PROGRESS = 'ALREADY_IN_PROGRESS'
}


