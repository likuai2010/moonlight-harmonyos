import hilog from '@ohos.hilog';
import testNapi, { CurlClient, generate_x509_certificate } from 'libentry.so'

import { Game } from 'ets/entryability/Game'
import { MoonBridge } from '../entryability/MoonBridge';
const game = new Game()
const mb = new MoonBridge()
@Entry
@Component
struct Index {
  @State message: string = 'Hello World'
  build() {
    Row() {
      Column() {
        Text(this.message)
          .fontSize(50)
          .fontWeight(FontWeight.Bold)
          .onClick(() => {
            this.message = "xxxx"
            mb.startConnection();

            //game.surfaceChanged()
            //mb.startConnection()
            hilog.info(0x0000, 'testTag', `Test NAPI 2 + 3 = ${testNapi.add(1, 2, "2222")}`);
          })
      }
      .width('100%')
    }
    .height('100%')
  }
}
