
import { NavTitle } from './compoments/Title'
import viewModel from '../entryability/computers/ComputerManagerViewModel'
@Entry
@Component
struct AddPage {
  viewModel = viewModel
  @State ip: string = ''
  build(){
    Column(){
      NavTitle({ title: "手动添加host" })
      Row(){
        TextInput({placeholder:'串流电脑的Ip地址', text: "192.168.3.5"}).fontColor(Color.White).onChange((value: string) => {
          this.ip = value;
        }).layoutWeight(1).type(InputType.Normal)
        Button("确定").width(150).margin({left:5}).onClick(()=>{
            this.viewModel.addPc(this.ip)
        })
      }.width("100%")
    }.padding(20).height("100%").width("100%").backgroundColor($r("app.color.page_background"))
  }
}