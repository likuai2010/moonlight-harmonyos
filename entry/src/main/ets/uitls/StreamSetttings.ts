import dataPreferences from '@ohos.data.preferences';
import common from '@ohos.app.ability.common';
export class StreamSettings{
  resolution_list: string = "1280x720";
  fps_list: string = "60";
  seekbar_bitrate: string = "10";
  frame_pacing: string = "balanced";
  stretch_video: boolean = false;
  audio_config_list: string = "2";
  enable_audiofx: boolean = false;
  seekbar_deadzone: string = "";
  xb1_driver: boolean = false;
  usb_bind_all: boolean = false;
  mouse_emulation: boolean = false;
  vibrate_fallback: boolean = false;
  flip_face_buttons: boolean = false;
  gamepad_touchpad_as_mouse: boolean = false;
  gamepad_motion_sensors: boolean = false;
  gamepad_motion_fallback: boolean = false;
  touchscreen_trackpad: boolean = false;
  mouse_nav_buttons: boolean = false;
  absolute_mouse_mode: boolean = false;
  show_onscreen_controls: boolean = false;
  vibrate_osc: boolean = false;
  only_l3r3: boolean = false;
  osc_opacity: boolean = false;
  reset_osc: boolean = false;
  enable_sops: boolean = false;
  host_audio: boolean = false;
  enable_pip: boolean = false;
  language_list: boolean = false;
  small_icon_mode: boolean = false;
  unlock_fps: boolean = false;
  refresh_rate: boolean = false;
  disable_warnings: boolean = false;
  video_format: string = "h264";
  enable_hdr: boolean = false;
  full_range: boolean = false;
  enable_perf_overlay: boolean = false;
  enable_post_stream_toast: boolean = false;
}

export async function loadSettings(context: common.Context): Promise<StreamSettings>{
  const preferences = await dataPreferences.getPreferences(context, "StreamSettings")
  const all = await preferences.getAll()
  const settings: StreamSettings = Object.assign(new StreamSettings(), all)
  return settings;
}
