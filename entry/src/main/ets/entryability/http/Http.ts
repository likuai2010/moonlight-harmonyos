import axios, { AxiosError, AxiosResponse } from '@ohos/axios'
import { LimelightCertProvider } from '../crypto/LimelightCryptoProvider';

export function getHttps(url: string ="https://192.168.3.5:47984/applist?uniqueid=0123456789ABCDEF&uuid=9983ad6c-0365-4f09-8085-9b3231eb4379&"): string{
  const capath = LimelightCertProvider.filesDir + "/ca.pem"

  // @ts-ignore
  axios({
    url,
    method: 'get',
    caPath: capath,
    // @ts-ignore
    clientCert: {
      certPath: '/data/storage/el2/base/haps/entry/cache/client.pem', //客户端证书路径
      certType: 'pem', // 客户端证书类型，包括pem、der、p12三种
      keyPath: '/data/storage/el2/base/haps/entry/cache/private.key', //客户端私钥路径
    }
  }).then((s)=>{
    console.log(s +"");
  }).catch((err: AxiosError) => {
    console.log(err + "");
  })
    return ""
}
