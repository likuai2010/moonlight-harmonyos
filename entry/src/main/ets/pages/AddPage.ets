
import { NavTitle } from './compoments/Title'
import viewModel from '../entryability/ComputerManagerViewModel'
import router from '@ohos.router'
import { Loading } from './compoments/Loading'



@Entry
@Component
struct AddPage {
  viewModel = viewModel
  @State ip: string = ''
  loadingDialog: CustomDialogController = new CustomDialogController({
    builder: Loading({
      title:"连接中",
      textValue: "建立连接中建立连接中建立连接中建立连接中建立连接中建立连接中建立连接中建立连接中",
    }),
    autoCancel: false,
    alignment: DialogAlignment.Center,
    customStyle: true
  })
  alertDialog: CustomDialogController = new CustomDialogController({
    builder: Loading({
      title:"连接中",
      textValue: "",
    }),
    autoCancel: false,
    alignment: DialogAlignment.Center,
    customStyle: true
  })
  build(){
    Column(){
      NavTitle({ title: "手动添加电脑" })
      Row(){
        TextInput({placeholder:'串流电脑的ip地址', text: "221.217.48.5"}).fontColor(Color.White).onChange((value: string) => {
          this.ip = value;
        }).layoutWeight(1).type(InputType.Normal)
        Button("确定").width(150).margin({left:5}).onClick(()=>{
            this.loadingDialog.open()
            this.viewModel.addPc(this.ip).then((result)=>{
                this.loadingDialog.close()
                if(result){
                  router.back()
                }else{

                }
            })
        })
      }.width("100%")
    }.padding(20).height("100%").width("100%").backgroundColor($r("app.color.page_background"))
  }
}