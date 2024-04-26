
import dataPreferences from '@ohos.data.preferences';
import { StreamSettings } from '../../../uitls/StreamSetttings';
import { getResString } from '../../../uitls/ResString';
import Prompt from '@system.prompt';


@CustomDialog
struct SeekBarDialog{
  @Link inputValue : number
  title: string | Resource = ""
  summary?: Resource
  dialogController: CustomDialogController
  onConfirm: () => void = ()=>{}
  build(){
    Column() {
      Text(this.title)
      Text(this.summary)
      Blank().height(50)
      Slider({
        value: this.inputValue,
        min: 0,
        max: 100,
        style: SliderStyle.InSet
      })
        .blockColor('#191970')
        .trackColor('#ffe0eaec')
        .selectedColor('#c3c3c3')
        .showTips(true)
        .onChange((value: number, mode: SliderChangeMode) => {
          this.inputValue = value
          console.info('value:' + value + 'mode:' + mode.toString())
        })
      Blank().height(30)
      Button('确定').onClick(()=>{
        this.onConfirm()
        this.dialogController.close()
      })
    }.padding(20)
  }
}

@Component
export struct SeekBarPreference {
  @Consume  @Watch('onSettingsUpdated')
  settings: StreamSettings
  pKey: string = ""
  title: Resource | undefined = undefined
  summary: Resource | undefined = undefined
  @State value: string = ""
  onChange: (key:string, value: string) => void = ()=>{}
  aboutToAppear() {
  }
  onSettingsUpdated(){
    if(this.settings[this.pKey] != null){
      this.value = this.settings[this.pKey]
    }
  }
  dialogController: CustomDialogController = new CustomDialogController({
    builder: SeekBarDialog({
      inputValue: $value,
      title: this.title,
      summary: this.summary,
      onConfirm: ()=>{
        this.onChange(this.pKey, this.value)
      }
    }),
    autoCancel: true,
    alignment: DialogAlignment.Bottom,
    customStyle: false
  })
  build() {
    baseReference({
      title: this.title,
      summary: this.summary ,
      hasCheck: undefined,
      onClick: () => {
        this.dialogController.open()
        this.onChange(this.pKey, this.value)
      }
    })
  }
}


@Component
export struct ListPreference {
  @Consume  @Watch('onSettingsUpdated')
  settings: StreamSettings
  pKey: string = ""
  title?: Resource | string
  summary?: Resource | string
  @State value: string = ""
  onChange: (key:string, value: dataPreferences.ValueType) => void = ()=>{}
  private select: number = 0
  entries: string[] = []
  names: Resource[] = []
  private labels: string[] = []

  aboutToAppear() {
    this.getLabels()
  }
  onSettingsUpdated(){
    if(!this.settings || this.settings[this.pKey] == null){
      this.select == this.entries.indexOf(this.value)
    } else {
      this.value = this.settings[this.pKey]
      this.select = this.entries.indexOf(this.value)
    }
  }
  async getLabels() {
    this.onSettingsUpdated()
    for (let r of this.names) {
      this.labels.push(await getResString(this, r))
    }
  }
  build() {

    baseReference({
      title: this.title,
      summary: this.summary ,
      hasCheck: undefined,
      onClick: () => {
        TextPickerDialog.show({
          range: this.labels,
          selected: this.select,
          onAccept: (value: TextPickerResult) => {
            // 暂未开放
            if (value.index > 1) {
              Prompt.showToast({message: "敬请期待!"})
              return;
            }
            this.select = value.index as number
            this.value = this.entries[value.index as number]
            this.onChange(this.pKey, this.value)
          },
        })
      }
    })
  }
}



@Component
export struct CheckBoxPreference {
  @Consume  @Watch('onSettingsUpdated')
  settings: StreamSettings
  pKey: string = ""
  title?: Resource | string
  summary?: Resource | string
  @State value: boolean | undefined = undefined
  onChange: (key:string, value: boolean) => void = ()=>{}

  onSettingsUpdated(){
    if(this.settings[this.pKey] != null && this.value != undefined){
      this.value = this.settings[this.pKey]
    }
  }
  build() {
    Column(){
      baseReference({ title: this.title, summary: this.summary, hasCheck: this.value, onClick:() => {
        if (this.value != undefined)
          this.value = !this.value
        this.onChange(this.pKey, this.value as boolean)
      }})
    }
  }
}

export class BaseReferenceParam  {
  title?: string | Resource
  summary?: string | Resource
  hasCheck?: boolean
  onClick: (() => void) = ()=> {}
}
@Builder function baseReference(param: BaseReferenceParam) {
  Row() {
    Row() {
      Column({ space: 2 }) {
        Text(param.title).fontSize(20).fontColor(0xFFFFFF)
        Text(param.summary).fontSize(16).fontColor(0xC3C3C3)
      }.layoutWeight(1).alignItems(HorizontalAlign.Start)
      if (param.hasCheck != undefined){
        Toggle({ type: ToggleType.Checkbox, isOn: param.hasCheck }).onClick(param.onClick)
      }
    }.padding({ right: 20, left: 10}).width('100%').onClick(param.onClick)
  }
}
