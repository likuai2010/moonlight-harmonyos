
import { NavTitle } from './compoments/Title'
import web_webview from '@ohos.web.webview'


@Entry
@Component
struct PrivacyPage {
  controller: web_webview.WebviewController = new web_webview.WebviewController()
  build(){
    Column(){
      NavTitle({ title: "隐私政策"})
      Web({ src: 'www.baidu.com', controller: this.controller }).width("100%").layoutWeight(1);
    }.padding(20).height("100%").width("100%").backgroundColor($r("app.color.page_background"))
  }
}