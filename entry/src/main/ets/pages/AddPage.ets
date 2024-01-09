
import { NavTitle } from './compoments/Title'
@Entry
@Component
struct AddPage {
  @State ip: string = ''
  build(){
    Column(){
      NavTitle({ title: "手动添加host" })
      Row(){
        TextInput({placeholder:'串流电脑的Ip地址', }).onChange((value: string) => {
          this.ip = value;
        }).layoutWeight(1).type(InputType.Normal)
        Button("确定").width(150).margin({left:5}).onClick(()=>{

        })
      }.width("100%")
    }.padding(10).height("100%").width("100%").backgroundColor(Color.Gray)
  }
}