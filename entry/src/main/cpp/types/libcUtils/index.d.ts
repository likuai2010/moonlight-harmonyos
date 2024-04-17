export const add: (a: number, b: number) => number;
export const drawRectangle:()=> number;
export const loadYuv:(file: string)=> number;
export const drawLine:()=> number;

export const generate_x509_certificate: (a: string, b: string) => number;
export const verify_signature: (a: Uint8Array, b: Uint8Array, c: Uint8Array) => boolean;
export const sign_message: (message: Uint8Array, key: Uint8Array,) => Uint8Array;
export const decrypt: (message: Uint8Array, key: Uint8Array,) => Uint8Array;
export const encrypt: (message: Uint8Array, key: Uint8Array,) => Uint8Array;
export const openSlEsPlayer_sendPcmData: (message: Uint8Array) => void;

export declare class CurlClient {
  close()
  get(url: string, timeout: Number, client: string, key: string): Promise<Uint8Array>;
}
export declare class VideoStatus{
  decoder: string
  totalFps: Number
  receivedFps: Number
  decodedFps: Number
  renderedFps: Number
  networkDroppedRate: number
  networkDroppedFrames: number
  decodeTime: number
  receivedTime: number
}
