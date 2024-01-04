

@CustomDialog
export struct Loading {
  @Link textValue: string
  controller: CustomDialogController
  build() {
    Column(){
      Text("建立连接中")
      Row(){
        LoadingProgress().width("30%")
        Text("message")
      }.padding({bottom: 5})
    }.alignItems(HorizontalAlign.Start).backgroundColor(Color.White)
    .width("40%")
    .height("20%")
  }
}
