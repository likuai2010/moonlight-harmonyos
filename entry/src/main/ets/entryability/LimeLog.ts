import hilog from '@ohos.hilog'

class LimeLog {
  public info(msg: string) {
    hilog.info(0x0000, "LimeLog", msg);
  }
  public warning(msg: string) {
    hilog.warn(0x0000, "LimeLog", msg);
  }
  public error(msg: string) {
    hilog.error(0x0000, "LimeLog", msg);
  }
}

export default new LimeLog()