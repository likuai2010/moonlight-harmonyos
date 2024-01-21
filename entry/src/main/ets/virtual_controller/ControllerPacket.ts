export class ControllerPacket {
  static A_FLAG: number = 0x1000;
  static B_FLAG: number = 0x2000;
  static X_FLAG: number = 0x4000;
  static Y_FLAG: number = 0x8000;
  static UP_FLAG: number = 0x0001;
  static DOWN_FLAG: number = 0x0002;
  static LEFT_FLAG: number = 0x0004;
  static RIGHT_FLAG: number = 0x0008;
  static LB_FLAG: number = 0x0100;
  static RB_FLAG: number = 0x0200;
  static PLAY_FLAG: number = 0x0010;
  static BACK_FLAG: number = 0x0020;
  static LS_CLK_FLAG: number = 0x0040;
  static RS_CLK_FLAG: number = 0x0080;
  static SPECIAL_BUTTON_FLAG: number = 0x0400;

  // Extended buttons (Sunshine only)
  static PADDLE1_FLAG: number = 0x010000;
  static PADDLE2_FLAG: number = 0x020000;
  static PADDLE3_FLAG: number = 0x040000;
  static PADDLE4_FLAG: number = 0x080000;
  static TOUCHPAD_FLAG: number = 0x100000; // Touchpad buttons on Sony controllers
  static MISC_FLAG: number = 0x200000; // Share/Mic/Capture/Mute buttons on various controllers
}