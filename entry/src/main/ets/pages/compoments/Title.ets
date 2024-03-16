
import router from '@ohos.router'

@Component
export struct NavTitle {
  title!: string | Resource
  build() {
    Stack({alignContent:Alignment.Start}){
      Row(){
        Text(this.title).fontColor(Color.White)
          .fontSize(23).textAlign(TextAlign.Center)
      }.
      width("100%")
      .justifyContent(FlexAlign.Center)
      .height(50)
      Icon({icon:$r('app.media.arrow_left')}).onClick(()=>{
        router.back()
      })
    }.width("100%").padding(10)
  }
}

@Component
export struct Icon {
  icon!: Resource
  iconSize: number = 48
  build(){
    Image(this.icon)
      .width(this.iconSize).height(this.iconSize)
  }
}

@Component
export struct MainTitle {
  title!: string
  build() {
    Row(){
      Icon({icon:$r("app.media.settings")}).onClick(()=>{
        router.pushUrl({ url:"pages/SettingsPage" })
      })
      Icon({icon: $r('app.media.ic_add_to_queue_white_48px')}).onClick(()=>{
        router.pushUrl({ url:"pages/AddPage" })
      })
    }.justifyContent(FlexAlign.SpaceBetween)
    .width("100%").padding(10)
    .height(50)
  }
}


