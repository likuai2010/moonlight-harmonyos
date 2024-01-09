
@Entry
@Component
struct SettingsPage {
  scroller: Scroller = new Scroller();
  private arr: number[] = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9];
  build(){
    Scroll(this.scroller) {
      Column(){
        ForEach(this.arr, (item) => {
          Row() {
            Text('Bluetooth' + item.toString()).fontSize(18)
            Blank()
            Toggle({ type: ToggleType.Checkbox, isOn: true })
          }.margin({ top: 10 }).backgroundColor(0xFFFFFF).borderRadius(15).padding({ left: 12 }).width('100%')
        }, item => item)
      }
    }.scrollable(ScrollDirection.Vertical).edgeEffect(EdgeEffect.Spring)
  }
}