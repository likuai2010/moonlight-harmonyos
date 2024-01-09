import hilog from '@ohos.hilog';
import router from '@ohos.router';
import LimelightCertProvider from '../entryability/crypto/LimelightCryptoProvider';
import { AddressTuple } from '../entryability/computers/ComputerDetails';
import { NvHttp } from '../entryability/http/NvHttp';

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
              const http = new NvHttp(new AddressTuple("192.168.3.5", 47989), 47984, null, LimelightCertProvider)
              const server = await http.getServerInfo(true)
              await http.pm.pair(server, "12345")
            }catch (e){
              console.log(e);
            }
          })
        Button("pc")
          .fontSize(50)
          .fontWeight(FontWeight.Bold)
          .onClick(async  () => {
            try {
              router.pushUrl({ url:"pages/PcPage" })
            }catch (e){
              console.log(e);
            }
          })
      }
    }
    .width('100%')
    .height('100%').backgroundColor($r("app.color.page_background"))
  }
}
