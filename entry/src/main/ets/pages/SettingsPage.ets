
import { NavTitle } from './compoments/Title';
@Entry
@Component
struct SettingsPage {
  scroller: Scroller = new Scroller();
  build(){
    Column(){
      NavTitle({title:"设置"})
      Scroll(this.scroller) {
        Column({space:10}){
          Column({space:10}){
            Text($r("app.string.category_basic_settings"))
            ListPreference({pKey: "resolution_list", title:$r("app.string.title_resolution_list"), summary: $r("app.string.summary_resolution_list"), value:"1280x720"})
            ListPreference({pKey: "fps_list", title:$r("app.string.title_fps_list"),summary: $r("app.string.summary_fps_list"), value:"60"})
            SeekBarPreference({pKey: "seekbar_bitrate",title:$r("app.string.title_seekbar_bitrate"),summary: $r("app.string.summary_seekbar_bitrate"), value:""})
            ListPreference({pKey: "frame_pacing",title:$r("app.string.title_frame_pacing"),summary: $r("app.string.summary_frame_pacing"), value:""})
            CheckBoxPreference({pKey: "stretch_video", title:$r("app.string.title_checkbox_stretch_video"), summary: null, value: false})
          }.alignItems(HorizontalAlign.Start)
          Column({space:10}){
            Text($r("app.string.category_audio_settings"))
            ListPreference({pKey: "audio_config_list", title:$r("app.string.title_audio_config_list"), summary: $r("app.string.summary_audio_config_list"), value:"1280x720"})
            CheckBoxPreference({pKey: "enable_audiofx", title:$r("app.string.title_checkbox_enable_audiofx"), summary: $r("app.string.summary_checkbox_enable_audiofx"), value: false})
          }.alignItems(HorizontalAlign.Start)
          Column({space:10}){
            Text($r("app.string.category_gamepad_settings"))
            SeekBarPreference({pKey: "seekbar_deadzone", title:$r("app.string.title_seekbar_deadzone"),summary: $r("app.string.summary_seekbar_deadzone"), value:""})
            CheckBoxPreference({pKey: "xb1_driver", title:$r("app.string.title_checkbox_xb1_driver"), summary: $r("app.string.summary_checkbox_xb1_driver"), value: false})
            CheckBoxPreference({pKey: "usb_bind_all", title:$r("app.string.title_checkbox_usb_bind_all"), summary: $r("app.string.summary_checkbox_usb_bind_all"), value: false})
            CheckBoxPreference({pKey: "mouse_emulation", title:$r("app.string.title_checkbox_mouse_emulation"), summary: $r("app.string.summary_checkbox_mouse_emulation"), value: false})
            CheckBoxPreference({pKey: "vibrate_fallback", title:$r("app.string.title_checkbox_vibrate_fallback"), summary: $r("app.string.summary_checkbox_vibrate_fallback"), value: false})
            CheckBoxPreference({pKey: "flip_face_buttons", title:$r("app.string.title_checkbox_flip_face_buttons"), summary: $r("app.string.summary_checkbox_flip_face_buttons"), value: false})
            CheckBoxPreference({pKey: "gamepad_touchpad_as_mouse", title:$r("app.string.title_checkbox_gamepad_touchpad_as_mouse"),summary: $r("app.string.summary_checkbox_gamepad_touchpad_as_mouse"), value: false})
            CheckBoxPreference({pKey: "gamepad_motion_sensors", title:$r("app.string.title_checkbox_gamepad_motion_sensors"),summary: $r("app.string.summary_checkbox_gamepad_motion_sensors"), value: false})
            CheckBoxPreference({pKey: "gamepad_motion_fallback", title:$r("app.string.title_checkbox_gamepad_motion_fallback"),summary: $r("app.string.summary_checkbox_gamepad_motion_fallback"), value: false})
          }.alignItems(HorizontalAlign.Start)
          Column({space:10}){
            Text($r("app.string.category_input_settings"))
            CheckBoxPreference({pKey: "touchscreen_trackpad", title:$r("app.string.title_checkbox_touchscreen_trackpad"), summary: $r("app.string.summary_checkbox_touchscreen_trackpad"), value: false})
            CheckBoxPreference({pKey: "mouse_nav_buttons", title:$r("app.string.title_checkbox_mouse_nav_buttons"), summary: $r("app.string.summary_checkbox_mouse_nav_buttons"), value: false})
            CheckBoxPreference({pKey: "absolute_mouse_mode", title:$r("app.string.title_checkbox_absolute_mouse_mode"), summary: $r("app.string.summary_checkbox_absolute_mouse_mode"), value: false})
          }.alignItems(HorizontalAlign.Start)
          Column({space:10}){
            Text($r("app.string.category_on_screen_controls_settings"))
            CheckBoxPreference({pKey: "show_onscreen_controls", title:$r("app.string.title_checkbox_show_onscreen_controls"), summary: $r("app.string.summary_checkbox_show_onscreen_controls"), value: false})
            CheckBoxPreference({pKey: "vibrate_osc", title:$r("app.string.title_checkbox_vibrate_osc"), summary: $r("app.string.summary_checkbox_vibrate_osc"), value: false})
            CheckBoxPreference({pKey: "only_l3r3", title:$r("app.string.title_only_l3r3"), summary: $r("app.string.summary_only_l3r3"), value: false})
            CheckBoxPreference({pKey: "osc_opacity", title:$r("app.string.dialog_title_osc_opacity"), summary: $r("app.string.summary_osc_opacity"), value: false})
            CheckBoxPreference({pKey: "reset_osc", title:$r("app.string.title_reset_osc"), summary: $r("app.string.summary_reset_osc"), value: false})
          }.alignItems(HorizontalAlign.Start)
          Column({space:10}){
            Text($r("app.string.category_host_settings"))
            CheckBoxPreference({pKey: "enable_sops", title:$r("app.string.title_checkbox_enable_sops"), summary: $r("app.string.summary_checkbox_enable_sops"), value: false})
            CheckBoxPreference({pKey: "host_audio", title:$r("app.string.title_checkbox_host_audio"), summary: $r("app.string.summary_checkbox_host_audio"), value: false})
          }.alignItems(HorizontalAlign.Start)
          Column({space:10){
            Text($r("app.string.category_ui_settings"))
            CheckBoxPreference({pKey: "enable_pip", title:$r("app.string.title_checkbox_enable_pip"), summary: $r("app.string.summary_checkbox_enable_pip"), value: false})
            CheckBoxPreference({pKey: "language_list", title:$r("app.string.title_language_list"), summary: $r("app.string.summary_language_list"), value: false})
            CheckBoxPreference({pKey: "small_icon_mode", title:$r("app.string.title_checkbox_small_icon_mode"), summary: $r("app.string.summary_checkbox_small_icon_mode"), value: false})
          }.alignItems(HorizontalAlign.Start)
          Column({space:10}){
            Text($r("app.string.category_advanced_settings"))
            CheckBoxPreference({pKey: "unlock_fps", title:$r("app.string.title_unlock_fps"), summary: $r("app.string.summary_unlock_fps"), value: false})
            CheckBoxPreference({pKey: "refresh_rate", title:$r("app.string.title_checkbox_reduce_refresh_rate"), summary: $r("app.string.summary_checkbox_reduce_refresh_rate"), value: false})
            CheckBoxPreference({pKey: "disable_warnings", title:$r("app.string.title_checkbox_disable_warnings"), summary: $r("app.string.summary_checkbox_disable_warnings"), value: false})
            CheckBoxPreference({pKey: "video_format", title:$r("app.string.title_video_format"), summary: $r("app.string.summary_video_format"), value: false})
            CheckBoxPreference({pKey: "enable_hdr", title:$r("app.string.title_enable_hdr"), summary: $r("app.string.summary_enable_hdr"), value: false})
            CheckBoxPreference({pKey: "full_range", title:$r("app.string.title_full_range"), summary: $r("app.string.summary_full_range"), value: false})
            CheckBoxPreference({pKey: "enable_perf_overlay", title:$r("app.string.title_enable_perf_overlay"), summary: $r("app.string.summary_enable_perf_overlay"), value: false})
            CheckBoxPreference({pKey: "enable_post_stream_toast", title:$r("app.string.title_enable_post_stream_toast"), summary: $r("app.string.summary_enable_post_stream_toast"), value: false})
          }.alignItems(HorizontalAlign.Start)
        }.padding(10)
      }.scrollable(ScrollDirection.Vertical)
    }.height('100%').backgroundColor($r("app.color.page_background"))
  }
}
@Component
struct SeekBarPreference {
  pKey: string
  title: Resource
  summary: Resource
  value: string = ""
  build(){
    BaseReference(this.title, this.summary, false, ()=>{})
  }
}

@Builder
function BaseReference(title: Resource, summary: Resource, hasCheck: Boolean = false, onClick:()=>void){
  Row() {
    Column({space:2}){
      Text(title).fontSize(20).fontColor(0xFFFFFF)
      Text(summary).fontSize(16).fontColor(0xC3C3C3)
    }.layoutWeight(1).alignItems(HorizontalAlign.Start)
    if (hasCheck) {
      Toggle({ type: ToggleType.Checkbox, isOn: this.value == true })
    }
  }.width('100%').onClick(onClick)
}

@Component
struct ListPreference {
  pKey: string
  title: Resource
  summary: Resource
  value: string = ""
  build(){
    BaseReference(this.title, this.summary, false, ()=>{})
  }
}
@Component
struct CheckBoxPreference {
  pKey: string
  title: Resource
  summary: Resource
  @State value: boolean = false
  build(){
    BaseReference(this.title, this.summary, true, ()=>{
      this.value = !this.value
    })
  }
}