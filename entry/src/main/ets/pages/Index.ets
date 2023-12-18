import hilog from '@ohos.hilog';
import testNapi from 'libentry.so'

import { Game } from 'ets/entryability/nvstream/Game'
import { MoonBridge } from '../entryability/nvstream/MoonBridge';
const game = new Game()
@Entry
@Component
struct Index {
  @State message: string = 'Hello World'
  build() {
    Column() {
      Column() {

      }.margin({
        top: 10,
        left: 20,
        right:30,
      })
      .height("33.3%")
      Row() {
        Button("开始绘制")
          .fontSize(50)
          .fontWeight(FontWeight.Bold)
          .onClick(() => {
            this.message = "xxxx"

            //game.surfaceChanged()
            //mb.startConnection()
            hilog.info(0x0000, 'testTag', `Test NAPI 2 + 3 = ${testNapi.add(1, 2, "2222")}`);

            // if (this.xComponentContext) {
            //   this.xComponentContext.drawRectangle();
            // }
          })
      }
      .width('100%')
    }
    .height('100%')
  }
}
