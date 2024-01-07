import hilog from '@ohos.hilog';
import router from '@ohos.router';
import { LimelightCertProvider } from '../entryability/crypto/LimelightCryptoProvider';
import { AddressTuple } from '../entryability/http/ComputerDetails';
import { NvHttp } from '../entryability/http/NvHttp';
import  { openSlEsPlayer_sendPcmData } from 'libentry.so'

@Entry
@Component
struct Index {
  @State textValue: string = ''
 
  onCancel() {
    console.info('Callback when the first button is clicked')
  }

  onAccept() {
    console.info('Callback when the second button is clicked')
  }

  build() {
    Stack({ alignContent: Alignment.Top }){
      Row() {
        Button("start")
          .fontSize(50)
          .fontWeight(FontWeight.Bold)
          .onClick(() => {
            router.pushUrl({ url:"pages/GamePage" })
          })
        Button("pair")
          .fontSize(50)
          .fontWeight(FontWeight.Bold)
          .onClick(async  () => {
            try {
              const lime = new LimelightCertProvider()
              await lime.initCertKeyPair()
              const http = new NvHttp(new AddressTuple("192.168.3.5", 47989), 47984, null, lime)
              const server = await http.getServerInfo(true)
              await http.pm.pair(server, "12345")
            }catch (e){
              console.log(e);
            }
          })
        Button("audio")
          .fontSize(50)
          .fontWeight(FontWeight.Bold)
          .onClick(async  () => {
            try {
              let size = 240 * 2 * 2
              let offset = 0;
              getContext(this).resourceManager.getRawFileContent('mix.pcm').then((d)=>{
                for (let index = 0; offset + size <= d.byteLength; index++) {
                  const dd =  d.buffer.slice(offset, offset + size)
                  const data = new Uint8Array(dd)
                  openSlEsPlayer_sendPcmData(data)
                  offset += size;
                }
              })

            }catch (e){
              console.log(e);
            }
          })
      }
    }
    .width('100%')
    .height('100%')
  }
}
