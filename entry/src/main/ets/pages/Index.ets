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
    Stack({ alignContent: Alignment.Top }){
      XComponent({ id: 'xcomponentId1', type: 'surface', libraryname: 'entry' })
        .onLoad((context: any) => {
          console.log("xxxxxx", context.add)
          this.xComponentContext = context
        })
        .onDestroy(() => {
          console.log("onDestroy");
        }).borderWidth(0).height("100%").width('100%')
      Row() {
        Button("save stream")
          .fontSize(50)
          .fontWeight(FontWeight.Bold)
          .onClick(() => {
            this.message = "save stream"
            game.surfaceChanged()
          })

      }
    }.backgroundColor(Color.Black)
    .width('100%')
    .height('100%')
  }
}
