import hilog from '@ohos.hilog';

import { Game } from 'ets/entryability/nvstream/Game'
import { MoonBridge } from '../entryability/nvstream/MoonBridge';
const game = new Game()
@Entry
@Component
struct Index {
  @State message: string = 'Hello World'
  xComponentContext: any | undefined = undefined;

  build() {
    Column() {
      Column() {
        XComponent({ id: 'xcomponentId1', type: 'surface', libraryname: 'entry' })
          .onLoad((context: any) => {
            console.log("xxxxxx", context.add)
            this.xComponentContext = context
          })
          .onDestroy(() => {
            console.log("onDestroy");
          })
      }.margin({
        top: 10,
        left: 20,
        right:30,
      })
      .height("33.3%")
      Row() {
        Button("save stream")
          .fontSize(50)
          .fontWeight(FontWeight.Bold)
          .onClick(() => {
            this.message = "save stream"
            game.surfaceChanged()
          })
        Button("stream")
          .fontSize(50)
          .fontWeight(FontWeight.Bold)
          .onClick(() => {
            this.message = "stream"
            if (this.xComponentContext) {
              this.xComponentContext.loadYuv("/data/storage/el2/base/haps/entry/cache/stream.yuv");
            }
          })
        Button("video1_640_272")
          .fontSize(50)
          .fontWeight(FontWeight.Bold)
          .onClick(() => {
            this.message = "video1_640_272"
            if (this.xComponentContext) {
              this.xComponentContext.loadYuv("/data/storage/el2/base/haps/entry/cache/video1_640_272.yuv");
            }
          })
      }
      .width('100%')
    }
    .height('100%')
  }
}
