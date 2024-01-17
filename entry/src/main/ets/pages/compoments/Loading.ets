

@CustomDialog
export struct Loading {
  title: string | Resource
  textValue: string | Resource  = null
  controller: CustomDialogController
  build() {
    Column(){
      Text(this.title).fontSize(18)
      Row(){
        LoadingProgress().height(100).width(80)
        Text(this.textValue).maxLines(2).height(100).textOverflow({overflow: TextOverflow.Ellipsis}).layoutWeight(1)
      }
    }.padding(20)
    .alignItems(HorizontalAlign.Start).backgroundColor(Color.White).borderRadius(10)
    .width("40%")
    .height(140)
  }
}

@CustomDialog
export struct Alert {
  title: string | Resource = "建立连接中"
  textValue: string | Resource
  controller: CustomDialogController
  build() {
    Column(){
      Text(this.title).fontSize(18)
      Text(this.textValue).maxLines(3).height(100).textOverflow({overflow: TextOverflow.Ellipsis}).layoutWeight(1)
    }.padding(10)
    .alignItems(HorizontalAlign.Start).backgroundColor(Color.White).borderRadius(10)
    .width("40%")
    .height("30%")
  }
}
