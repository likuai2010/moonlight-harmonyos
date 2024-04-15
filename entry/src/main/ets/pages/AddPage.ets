
import { NavTitle } from './compoments/Title'
import viewModel from '../entryability/ComputerManagerViewModel'
import router from '@ohos.router'
import { Loading,Alert } from './compoments/Loading'
import promptAction from '@ohos.promptAction'


@Entry
@Component
struct AddPage {
  viewModel = viewModel
  @State ip: string = ''
  loadingDialog: CustomDialogController = new CustomDialogController({
    builder: Loading({
      title: $r("app.string.title_add_pc"),
      text: $r("app.string.msg_add_pc"),
    }),
    autoCancel: false,
    alignment: DialogAlignment.Center,
    customStyle: true
  })
   addPc(){
    this.loadingDialog.open()
    this.viewModel.addPc(this.ip).then((result)=>{
      this.loadingDialog.close()
      if(result.success){
        router.back()
      }else{
        promptAction.showDialog({ title:  $r('app.string.conn_error_title'), message: result.message})
      }
    }).catch(()=>{
      promptAction.showDialog({ title:  $r('app.string.conn_error_title'), message: "添加失败"})
      this.loadingDialog.close()
    })
  }
  build(){
    Column(){
      NavTitle({ title: $r("app.string.title_add_pc")})
      Blank().height(10)
      Row(){
        TextInput({placeholder:'请输入电脑的ip地址', text: "223.72.35.11"}).placeholderColor(Color.Gray).fontColor(Color.White).onChange((value: string) => {
          this.ip = value.trim()
        }).layoutWeight(1).type(InputType.Normal).onSubmit(()=>{
            this.addPc()
        })
        Button("确定").width(150).margin({left:5}).onClick(()=>{
          this.addPc()
        })
      }.width("100%")
      Blank().layoutWeight(1)
    }.padding(20).height("100%").width("100%").backgroundColor($r("app.color.page_background"))
  }
}