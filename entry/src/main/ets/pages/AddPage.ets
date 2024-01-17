
import { NavTitle } from './compoments/Title'
import viewModel from '../entryability/ComputerManagerViewModel'
import router from '@ohos.router'
import { Loading } from './compoments/Loading'
import promptAction from '@ohos.promptAction'
import {getResString} from '../uitls/ResString'


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
        TextInput({placeholder:'串流电脑的ip地址', text: "[2409:8a00:79a1:5ae1:27d5:8223:fcba:4b17]"}).fontColor(Color.White).onChange((value: string) => {
          this.ip = value;
        }).layoutWeight(1).type(InputType.Normal)
        Button("确定").width(150).margin({left:5}).onClick(()=>{
            this.loadingDialog.open()
            this.viewModel.addPc(this.ip).then((result)=>{
                this.loadingDialog.close()
                if(result.success){
                  router.back()
                }else{
                  promptAction.showDialog({ title:  $r('app.string.conn_error_title'), message: result.message})
                }
            }).catch(()=>{
                promptAction.showDialog({ title:  $r('app.string.conn_error_title'), message: "未知错误"})
                this.loadingDialog.close()
            })
        })
      }.width("100%")
    }.padding(20).height("100%").width("100%").backgroundColor($r("app.color.page_background"))
  }
}