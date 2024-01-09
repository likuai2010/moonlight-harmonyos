
import router from '@ohos.router'
@Component
export struct NavTitle {
  title: string
  build() {
    Row(){
      Text(this.title)
        .fontSize(18).textAlign(TextAlign.Center)
    }.width("100%").justifyContent(FlexAlign.Center)
    .height(50)
  }
}

@Component
export struct Icon{
  icon: Resource
  build(){
    Image(this.icon)
      .width(50).height(50)
  }
}

@Component
export struct MainTitle {
  title: string
  build() {
    Row(){
      Icon({icon:$r("app.media.ic_public_settings")}).onClick(()=>{
        router.pushUrl({ url:"pages/SettingsPage" })
      })
      Icon({icon: $r('app.media.ic_public_add_norm')}).onClick(()=>{
        router.pushUrl({ url:"pages/AddPage" })
      })
    }.justifyContent(FlexAlign.SpaceBetween)
    .width("100%")
    .height(40)
  }
}
