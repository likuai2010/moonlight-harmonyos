import promptAction from '@ohos.promptAction';
import { NavTitle } from './compoments/Title';

@Entry
@Component
struct SettingsPage {
  scroller: Scroller = new Scroller();
  @State reference: Map<string, string> = new Map()
  onUpdateValue(key: string, value: any) {
    this.reference
  }
  build() {
    Column() {
      NavTitle({ title: "设置" })
      Scroll(this.scroller) {
        Column({ space: 10 }) {
          Column({ space: 10 }) {
            Text($r("app.string.category_basic_settings"))
            ListPreference({
              pKey: "resolution_list",
              title: $r("app.string.title_resolution_list"),
              summary: $r("app.string.summary_resolution_list"),
              names:[$r("app.string.resolution_360p"), $r("app.string.resolution_720p"), $r("app.string.resolution_1080p"), $r("app.string.resolution_1440p"), $r("app.string.resolution_4k")],
              entries: ["640x360", "1280x720", "1920x1080", "2560x1440", "3840x2160"],
              value: "1280x720",
              onChange: this.onUpdateValue
            })
            ListPreference({
              pKey: "fps_list",
              title: $r("app.string.title_fps_list"),
              summary: $r("app.string.summary_fps_list"),
              value: "60",
              names:[$r("app.string.fps_30"), $r("app.string.fps_60"), $r("app.string.fps_90"), $r("app.string.fps_120")],
              entries: ["30", "60", "90", "120"],
              onChange: this.onUpdateValue
            })
            SeekBarPreference({
              pKey: "seekbar_bitrate",
              title: $r("app.string.title_seekbar_bitrate"),
              summary: $r("app.string.summary_seekbar_bitrate"),
              value: "",
              onChange: this.onUpdateValue
            })
            ListPreference({
              pKey: "frame_pacing",
              title: $r("app.string.title_frame_pacing"),
              summary: $r("app.string.summary_frame_pacing"),
              names:[$r("app.string.pacing_latency"), $r("app.string.pacing_balanced"), $r("app.string.pacing_balanced_alt"), $r("app.string.pacing_smoothness")],
              entries: ["latency", "balanced", "cap-fps", "smoothness"],
              value: "",
              onChange: this.onUpdateValue
            })
            CheckBoxPreference({
              pKey: "stretch_video",
              title: $r("app.string.title_checkbox_stretch_video"),
              summary: null,
              value: false,
              onChange: this.onUpdateValue
            })
          }.alignItems(HorizontalAlign.Start)

          Column({ space: 10 }) {
            Text($r("app.string.category_audio_settings"))
            ListPreference({
              pKey: "audio_config_list",
              title: $r("app.string.title_audio_config_list"),
              summary: $r("app.string.summary_audio_config_list"),
              names:[$r("app.string.audioconf_stereo"), $r("app.string.audioconf_51surround"), $r("app.string.audioconf_71surround")],
              entries: ["2", "51", "71"],
              value: "1280x720",
              onChange: this.onUpdateValue
            })
            CheckBoxPreference({
              pKey: "enable_audiofx",
              title: $r("app.string.title_checkbox_enable_audiofx"),
              summary: $r("app.string.summary_checkbox_enable_audiofx"),
              value: false,
              onChange: this.onUpdateValue
            })
          }.alignItems(HorizontalAlign.Start)

          Column({ space: 10 }) {
            Text($r("app.string.category_gamepad_settings"))
            SeekBarPreference({
              pKey: "seekbar_deadzone",
              title: $r("app.string.title_seekbar_deadzone"),
              summary: $r("app.string.summary_seekbar_deadzone"),
              value: "",
              onChange: this.onUpdateValue
            })
            CheckBoxPreference({
              pKey: "xb1_driver",
              title: $r("app.string.title_checkbox_xb1_driver"),
              summary: $r("app.string.summary_checkbox_xb1_driver"),
              value: false,
              onChange: this.onUpdateValue
            })
            CheckBoxPreference({
              pKey: "usb_bind_all",
              title: $r("app.string.title_checkbox_usb_bind_all"),
              summary: $r("app.string.summary_checkbox_usb_bind_all"),
              value: false,
              onChange: this.onUpdateValue
            })
            CheckBoxPreference({
              pKey: "mouse_emulation",
              title: $r("app.string.title_checkbox_mouse_emulation"),
              summary: $r("app.string.summary_checkbox_mouse_emulation"),
              value: false,
              onChange: this.onUpdateValue
            })
            CheckBoxPreference({
              pKey: "vibrate_fallback",
              title: $r("app.string.title_checkbox_vibrate_fallback"),
              summary: $r("app.string.summary_checkbox_vibrate_fallback"),
              value: false,
              onChange: this.onUpdateValue
            })
            CheckBoxPreference({
              pKey: "flip_face_buttons",
              title: $r("app.string.title_checkbox_flip_face_buttons"),
              summary: $r("app.string.summary_checkbox_flip_face_buttons"),
              value: false,
              onChange: this.onUpdateValue
            })
            CheckBoxPreference({
              pKey: "gamepad_touchpad_as_mouse",
              title: $r("app.string.title_checkbox_gamepad_touchpad_as_mouse"),
              summary: $r("app.string.summary_checkbox_gamepad_touchpad_as_mouse"),
              value: false,
              onChange: this.onUpdateValue
            })
            CheckBoxPreference({
              pKey: "gamepad_motion_sensors",
              title: $r("app.string.title_checkbox_gamepad_motion_sensors"),
              summary: $r("app.string.summary_checkbox_gamepad_motion_sensors"),
              value: false,
              onChange: this.onUpdateValue
            })
            CheckBoxPreference({
              pKey: "gamepad_motion_fallback",
              title: $r("app.string.title_checkbox_gamepad_motion_fallback"),
              summary: $r("app.string.summary_checkbox_gamepad_motion_fallback"),
              value: false,
              onChange: this.onUpdateValue
            })
          }.alignItems(HorizontalAlign.Start)

          Column({ space: 10 }) {
            Text($r("app.string.category_input_settings"))
            CheckBoxPreference({
              pKey: "touchscreen_trackpad",
              title: $r("app.string.title_checkbox_touchscreen_trackpad"),
              summary: $r("app.string.summary_checkbox_touchscreen_trackpad"),
              value: false,
              onChange: this.onUpdateValue
            })
            CheckBoxPreference({
              pKey: "mouse_nav_buttons",
              title: $r("app.string.title_checkbox_mouse_nav_buttons"),
              summary: $r("app.string.summary_checkbox_mouse_nav_buttons"),
              value: false,
              onChange: this.onUpdateValue
            })
            CheckBoxPreference({
              pKey: "absolute_mouse_mode",
              title: $r("app.string.title_checkbox_absolute_mouse_mode"),
              summary: $r("app.string.summary_checkbox_absolute_mouse_mode"),
              value: false,
              onChange: this.onUpdateValue
            })
          }.alignItems(HorizontalAlign.Start)

          Column({ space: 10 }) {
            Text($r("app.string.category_on_screen_controls_settings"))
            CheckBoxPreference({
              pKey: "show_onscreen_controls",
              title: $r("app.string.title_checkbox_show_onscreen_controls"),
              summary: $r("app.string.summary_checkbox_show_onscreen_controls"),
              value: false,
              onChange: this.onUpdateValue
            })
            CheckBoxPreference({
              pKey: "vibrate_osc",
              title: $r("app.string.title_checkbox_vibrate_osc"),
              summary: $r("app.string.summary_checkbox_vibrate_osc"),
              value: false,
              onChange: this.onUpdateValue
            })
            CheckBoxPreference({
              pKey: "only_l3r3",
              title: $r("app.string.title_only_l3r3"),
              summary: $r("app.string.summary_only_l3r3"),
              value: false,
              onChange: this.onUpdateValue
            })
            CheckBoxPreference({
              pKey: "osc_opacity",
              title: $r("app.string.dialog_title_osc_opacity"),
              summary: $r("app.string.summary_osc_opacity"),
              value: false,
              onChange: this.onUpdateValue
            })
            CheckBoxPreference({
              pKey: "reset_osc",
              title: $r("app.string.title_reset_osc"),
              summary: $r("app.string.summary_reset_osc"),
              value: false,
              onChange: this.onUpdateValue
            })
          }.alignItems(HorizontalAlign.Start)

          Column({ space: 10 }) {
            Text($r("app.string.category_host_settings"))
            CheckBoxPreference({
              pKey: "enable_sops",
              title: $r("app.string.title_checkbox_enable_sops"),
              summary: $r("app.string.summary_checkbox_enable_sops"),
              value: false,
              onChange: this.onUpdateValue
            })
            CheckBoxPreference({
              pKey: "host_audio",
              title: $r("app.string.title_checkbox_host_audio"),
              summary: $r("app.string.summary_checkbox_host_audio"),
              value: false,
              onChange: this.onUpdateValue
            })
          }.alignItems(HorizontalAlign.Start)

          Column({ space: 10 }) {
            Text($r("app.string.category_ui_settings"))
            CheckBoxPreference({
              pKey: "enable_pip",
              title: $r("app.string.title_checkbox_enable_pip"),
              summary: $r("app.string.summary_checkbox_enable_pip"),
              value: false,
              onChange: this.onUpdateValue
            })
            CheckBoxPreference({
              pKey: "language_list",
              title: $r("app.string.title_language_list"),
              summary: $r("app.string.summary_language_list"),
              value: false,
              onChange: this.onUpdateValue
            })
            CheckBoxPreference({
              pKey: "small_icon_mode",
              title: $r("app.string.title_checkbox_small_icon_mode"),
              summary: $r("app.string.summary_checkbox_small_icon_mode"),
              value: false,
              onChange: this.onUpdateValue
            })
          }.alignItems(HorizontalAlign.Start)

          Column({ space: 10 }) {
            Text($r("app.string.category_advanced_settings"))
            CheckBoxPreference({
              pKey: "unlock_fps",
              title: $r("app.string.title_unlock_fps"),
              summary: $r("app.string.summary_unlock_fps"),
              value: false,
              onChange: this.onUpdateValue
            })
            CheckBoxPreference({
              pKey: "refresh_rate",
              title: $r("app.string.title_checkbox_reduce_refresh_rate"),
              summary: $r("app.string.summary_checkbox_reduce_refresh_rate"),
              value: false,
              onChange: this.onUpdateValue
            })
            CheckBoxPreference({
              pKey: "disable_warnings",
              title: $r("app.string.title_checkbox_disable_warnings"),
              summary: $r("app.string.summary_checkbox_disable_warnings"),
              value: false,
              onChange: this.onUpdateValue
            })
            ListPreference({
              pKey: "video_format",
              title: $r("app.string.title_video_format"),
              summary: $r("app.string.summary_video_format"),
              names:[$r("app.string.videoformat_auto"),  $r("app.string.videoformat_hevcalways"), $r("app.string.videoformat_h264always")],
              entries: ["auto", "h265", "h264"],//$r("app.string.videoformat_av1always"), "av1",
              value: "h264",
              onChange: this.onUpdateValue
            })
            CheckBoxPreference({
              pKey: "enable_hdr",
              title: $r("app.string.title_enable_hdr"),
              summary: $r("app.string.summary_enable_hdr"),
              value: false,
              onChange: this.onUpdateValue
            })
            CheckBoxPreference({
              pKey: "full_range",
              title: $r("app.string.title_full_range"),
              summary: $r("app.string.summary_full_range"),
              value: false,
              onChange: this.onUpdateValue
            })
            CheckBoxPreference({
              pKey: "enable_perf_overlay",
              title: $r("app.string.title_enable_perf_overlay"),
              summary: $r("app.string.summary_enable_perf_overlay"),
              value: false,
              onChange: this.onUpdateValue
            })
            CheckBoxPreference({
              pKey: "enable_post_stream_toast",
              title: $r("app.string.title_enable_post_stream_toast"),
              summary: $r("app.string.summary_enable_post_stream_toast"),
              value: false,
              onChange: this.onUpdateValue
            })
          }.alignItems(HorizontalAlign.Start)
          Blank().height(30)
        }.padding(10)
      }.scrollable(ScrollDirection.Vertical)
    }.height('100%').backgroundColor($r("app.color.page_background"))
  }
}


@Builder
function BaseReference(title: Resource, summary: Resource, hasCheck: Boolean = false, onClick: () => void) {
  Row() {
    Column({ space: 2 }) {
      Text(title).fontSize(20).fontColor(0xFFFFFF)
      Text(summary).fontSize(16).fontColor(0xC3C3C3)
    }.layoutWeight(1).alignItems(HorizontalAlign.Start)

    if (hasCheck) {
      Toggle({ type: ToggleType.Checkbox, isOn: this.value == true })
    }
  }.width('100%').onClick(onClick)
}


@CustomDialog
struct SeekBarDialog{
  @Link inputValue : number
  dialogController: CustomDialogController
  onConfirm: () => void
  build(){
    Column() {
      Slider({
        value: this.inputValue,
        min: 0,
        max: 100,
        style: SliderStyle.InSet
      })
        .blockColor('#191970')
        .trackColor('#ADD8E6')
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
struct SeekBarPreference {
  pKey: string
  title: Resource
  summary: Resource
  @State value: string = ""
  onChange: (key, value) => void

  dialogController: CustomDialogController = new CustomDialogController({
    builder: SeekBarDialog({
      inputValue: $value,
      onConfirm: ()=>{

      }
    }),
    autoCancel: true,
    alignment: DialogAlignment.Bottom,
    customStyle: false
  })
  build() {
    BaseReference(this.title, this.summary, false, () => {
      this.dialogController.open()
      this.onChange(this.pKey, this.value)
    })
  }
}

import dialogRequest from '@ohos.app.ability.dialogRequest';
import Want from '@ohos.app.ability.Want';
import { getResString } from '../uitls/ResString';

@Component
struct ListPreference {
  pKey: string
  title: Resource
  summary: Resource
  @State value: string = ""
  onChange: (key, value) => void
  private select: number = 0
  entries: string[] = []
  names: Resource[]
  private labels: string[] = []

  aboutToAppear() {
    this.getLabels()
  }

  async getLabels() {
    this.select == this.entries.indexOf(this.value)
    for (let r of this.names) {
      this.labels.push(await getResString(this, r))
    }
  }

  build() {
    BaseReference(this.title, this.summary, false, () => {
      TextPickerDialog.show({
        range: this.labels,
        selected: this.select,
        onAccept: (value: TextPickerResult) => {
          this.select = value.index
          this.value = this.entries[value.index]
          this.onChange(this.pKey, this.value)
        },
      })
    })
  }
}

@Component
struct CheckBoxPreference {
  pKey: string
  title: Resource
  summary: Resource
  @State value: boolean = false
  onChange: (key, value) => void

  build() {
    BaseReference(this.title, this.summary, true, () => {
      this.value = !this.value
      this.onChange(this.pKey, this.value)
    })
  }
}